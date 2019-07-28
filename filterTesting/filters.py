import numpy as np # for the linear algebra]

CONST_APOGEE_CHECKS = 5
CONST_g = 9.8

class kalFilt: # Kalman Filter
    def __init__(self, tInit, xInit, PInit):
        self.xNew = xInit
        self.PNew = PInit
        self.xOld = xInit
        self.POld = PInit
        self.tOld = tInit
        self.altOld = xInit[0]
        self.R = np.array([
                [0.9, 0], #0.9 in upper left is uncertainty of barometer
                [0, 0.9*2/0.05] # I think that's how propagation of uncertainty works for velocity calc?
            ])
        #process noise
        # Including gravity leaves us with only drag. Near apogee, velocity is close to zero.
        # Assume v = 10 m/s, cD = 0.4, rho = 0.4135 (https://www.engineeringtoolbox.com/standard-atmosphere-d_604.html),
        # A = 0.022 m^2, m = 23.5 kg, you get about 8 mm/s^2 of acceleration, which is smaller than the amount of
        # graviational variation between ground and apogee.

        # Error in position is  (0.5 * a * t^2, since x = v0 * t + 0.5*a*t^2
        self.Q = np.array([0.5*0.1*0.05 ** 2,0.005])

        self.apogeeCount = 0
        self.atApogee = False

    def nextData(self, tNew, altNew):
        dt = tNew - self.tOld

        zMeas = np.empty(2)
        zMeas[0] = altNew

        # Pick one
        zMeas[1] = (altNew - self.xOld[0]) / dt
        # zMeas[1] = (altNew - self.altOld) / dt

        self.altOld = altNew

        self.xOld = self.xNew
        self.POld = self.PNew
        self.tOld = tNew

        F = np.array(
            [[1, dt],
            [0, 1]]
        )

        #Control vector to incorporate effects of gravity
        kalmanB = np.array(
            [-0.5*CONST_g * dt ** 2, -CONST_g * dt])

        xPred = (F @ self.xOld) + kalmanB.T
        PPred = F @ self.POld @ F.T + self.Q

        y = zMeas - xPred
        S = PPred + self.R

        K = PPred @ np.linalg.inv(S)
        self.xNew = xPred + K @ y
        self.PNew = (np.identity(2) - K) @ PPred

        if (self.xNew[0] < self.xOld[0]):
            self.apogeeCount += 1
        else:
            self.apogeeCount = 0

        if self.apogeeCount >= CONST_APOGEE_CHECKS and not self.atApogee:
            print("Kalman filter detected apogee at time {}".format(tNew))
            self.atApogee = True

        return((self.xNew, self.PNew))

class movAvgFilt:
    CONST_PRES_AVG_SET_SIZE = 15

    def __init__(self, tInit, altInit):
        self.altSet = [altInit] * self.CONST_PRES_AVG_SET_SIZE
        self.timeSet = [0.05] * self.CONST_PRES_AVG_SET_SIZE
        self.tOld = tInit
        self.altOld = altInit
        self.apogeeCount = 0
        self.atApogee = False

    def nextData(self, tNew, altMeas):
        dt = tNew - self.tOld
        self.tOld = tNew

        del self.altSet[0]
        del self.timeSet[0]
        self.altSet.append(altMeas)
        self.timeSet.append(dt)

        self.altNew = np.mean(self.altSet)
        v = (self.altNew - self.altOld) / np.mean(self.timeSet)
        self.altOld = self.altNew

        if v < 0:
            self.apogeeCount += 1
        else:
            self.apogeeCount = 0

        if self.apogeeCount >= CONST_APOGEE_CHECKS and not self.atApogee:
            print("Moving average detected apogee at time {}".format(tNew))
            self.atApogee = True

        return(self.altNew, v)

