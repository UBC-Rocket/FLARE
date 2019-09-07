import numpy as np  # for the linear algebra

CONST_BAROM_RAW_UNCER = 0.9
CONST_BAROM_UNCER = np.array([
    [0.9, 0],  # 0.9 in upper left is uncertainty of barometer
    # I think that's how propagation of uncertainty works for velocity calc?
    [0, 0.9*2/0.05]
])
# CONST_BAROM_UNCER_NO_VEL = np.array([
#     [0.9, 0],
#     [0, 10000000]
# ])

CONST_APOGEE_CHECKS = 5
CONST_g = 9.8


class kalFilt:  # Kalman Filter
    # @param vLen Timespan over which velocity is measured
    # @param vSkip If true, filter will measure velocity once every vLen data points, and ignore the intermediate times. If not, filter will measure velocity at every point, using the data from vLen measurements ago as the base.
    # @param vDetect The velocity under which the filter will consider is apogee. E.g. if vDetect is +10, then the rocket will consider apogee to be when it is moving 10 m/s upwards.
    def __init__(self, vLen=1, vSkip=True, vDetect=0):

        # Setup default values

        # Timesteps between measuring velocity. If 1, equivalent to measuring every timestep. If 2, uses data point 2 timesteps ago instead of 1, etc.
        self._vLen = vLen

        # Whether to measure velocity at every time. If true, it will measure velocity once every self.vLen data points; if false it will measure velocity every  measurements, comparing self.vLen datapoints back
        self._vSkip = vSkip

        # Will start counting
        self._vDetect = vDetect

        self.name = None

    def reset(self, dataInit, quiet=True):
        tInit, xInit, PInit = dataInit
        self._quiet = quiet
        self.xNew = xInit
        self.PNew = PInit
        self._xOld = xInit
        self._POld = PInit
        self._tOld = tInit
        self._vtOld = tInit
        self._altOld = xInit[0]
        self._R = CONST_BAROM_UNCER
        # process noise
        # Including gravity leaves us with only drag. Near apogee, velocity is close to zero.
        # Assume v = 10 m/s, cD = 0.4, rho = 0.4135 (https://www.engineeringtoolbox.com/standard-atmosphere-d_604.html),
        # A = 0.022 m^2, m = 23.5 kg, you get about 8 mm/s^2 of acceleration, which is smaller than the amount of
        # graviational variation between ground and apogee.

        # Error in position is  (0.5 * a * t^2, since x = v0 * t + 0.5*a*t^2
        self._Q = np.array([0.5*0.1*0.05 ** 2, 0.005])

        self._apogeeCount = 0
        self.atApogee = False

        if self._vSkip:
            self._vCounter = self._vLen
        else:  # First initialization is automatically done by the way calcV is setup
            self._vtList = []
            self._vxList = []

        # clean up reporting variables
        self.apogeeTime = None
        self.apogeeHeight = None

    # Helper for velocity calculations
    # returns (v measurement, v uncertainty)
    def _calcV(self, tNew, altNew):
        if (self._vSkip):
            # Use existing implementation
            self._vCounter -= 1
            if (self._vCounter <= 0):
                vMeas = (altNew - self._altOld) / (tNew - self._vtOld)
                vUncer = 0.9 * 2 / (self._vLen * 0.05)
                self._vtOld = tNew

            else:
                vMeas = self._xOld[1]
                vUncer = 10000000

        else:  # not _vSkip
            # I'm like 70 % sure that this makes sense
            # Should be converted into circular buffer
            self._vtList.append(self._tOld)
            self._vxList.append(self.xNew[0])

            if (len(self._vtList) >= self._vLen):
                tOld = self._vtList.pop(0)
                altOld = self._vxList.pop(0)

                vMeas = (altNew - altOld) / (tNew - tOld)
                vUncer = 0.9 * 2 / (self._vLen * 0.05)
            else:
                vMeas = self._xOld[1]
                vUncer = 10000000

        return vMeas, vUncer

    # use to get next data value
    def nextData(self, tNew, altNew):
        dt = tNew - self._tOld

        zMeas = np.empty(2)
        zMeas[0] = altNew
        zMeas[1], self._R[1][1] = self._calcV(tNew, altNew)

        self._xOld = self.xNew
        self._POld = self.PNew
        self._tOld = tNew

        F = np.array(
            [[1, dt],
             [0, 1]]
        )
        # Control vector to incorporate effects of gravity
        kalmanB = np.array([-0.5*CONST_g * dt ** 2, -CONST_g * dt])

        xPred = (F @ self._xOld) + kalmanB.T
        PPred = F @ self._POld @ F.T + self._Q

        y = zMeas - xPred
        S = PPred + self._R

        K = PPred @ np.linalg.inv(S)
        self.xNew = xPred + K @ y
        self.PNew = (np.identity(2) - K) @ PPred

        # Needs to be done at end since 1. we want the nice, corrected xNew data and the easiest way is to measure _vCounter
        if self._vSkip and self._vCounter <= 0:
            self._altOld = self.xNew[0]
            self._vCounter = self._vLen

        self.addressApogee(tNew)

        return self.xNew, self.PNew

    # Helper function to deal with apogee detection and the related reporting (time/altitude recording, debug statements)
    def addressApogee(self, tNew):
        if (self.xNew[0] < self._xOld[0]):
            self._apogeeCount += 1
        elif self._apogeeCount > 0:
            self._apogeeCount -= 1

        if self._apogeeCount >= CONST_APOGEE_CHECKS and not self.atApogee:
            if not self._quiet:
                print("Kalman filter detected apogee at time {}".format(tNew))

            self.apogeeTime = tNew
            self.apogeeHeight = self.xNew[0]
            self.atApogee = True


class movAvgFilt:
    CONST_PRES_AVG_SET_SIZE = 15

    def reset(self, tInit, altInit, quiet=True):
        self._altSet = [altInit] * self.CONST_PRES_AVG_SET_SIZE
        self._timeSet = [0.05] * self.CONST_PRES_AVG_SET_SIZE
        self._tOld = tInit
        self._altOld = altInit
        self._apogeeCount = 0
        self.atApogee = False
        self.xNew = np.empty(2)

        self._quiet = quiet

    def nextData(self, tNew, altMeas):
        dt = tNew - self._tOld
        self._tOld = tNew

        del self._altSet[0]
        del self._timeSet[0]
        self._altSet.append(altMeas)
        self._timeSet.append(dt)

        self.altNew = np.mean(self._altSet)
        v = (self.altNew - self._altOld) / np.mean(self._timeSet)
        self._altOld = self.altNew

        if v < 0:
            self._apogeeCount += 1
        else:
            self._apogeeCount = 0

        if self._apogeeCount >= CONST_APOGEE_CHECKS and not self.atApogee:
            if not self._quiet:
                print("Moving average detected apogee at time {}".format(tNew))

            self.apogeeTime = tNew
            self.apogeeHeight = self.altNew
            self.atApogee = True

        self.xNew[0] = self.altNew
        return (self.altNew, v)
