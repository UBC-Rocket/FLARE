# Main code. Feeds data to Arduino. Hit any button to exit program.

import time #for timing
import sys #to be able to exit
import csv #for access to the pressure data set
import os # for files
import math # for logarithms
import random # for random value generation

import matplotlib.pyplot as plt #plotting
import numpy as np # for the linear algebra


class kalFilt: # Kalman Filter
    def __init__(self, tInit, xInit, PInit):
        self.xNew = xInit
        self.PNew = PInit
        self.xOld = xInit
        self.POld = PInit
        self.tOld = tInit
        self.R = np.array([
                [0.9, 0], #0.9 in upper left is uncertainty of barometer
                [0, 0.9*2/0.05] # I think that's how propagation of uncertainty works for velocity calc?
            ])
        #process noise
        # Including gravity leaves us with only drag. Near apogee, velocity is close to zero.
        # Assume v = 10 m/s, cD = 0.2, rho = 0.4135 (https://www.engineeringtoolbox.com/standard-atmosphere-d_604.html),
        # A = 0.022 m^2, m = 23.5 kg, you get about 4 mm/s^2 of acceleration, which is smaller than the amount of
        # graviational variation between ground and apogee.
        # I'm going to unjustifiably choose 0.1 m/s^2 of unknown acceleration
        # Over 0.05 s, velocity changes by 0.005 m/s,
        # Error in position is  (0.5 * a * t^2, since x = v0 * t + 0.5*a*t^2
        self.Q = np.array([0.5*0.1*0.005 ** 2, 0.005])

        self.R = np.array([
            [0.9, 0], #0.9 in upper left is uncertainty of barometer
            [0, 0.9*2/0.05] # I think that's how propagation of uncertainty works for velocity calc?
        ])


    def nextData(self, tNew, zMeas):
        dt = tNew - self.tOld

        self.xOld = xNew
        self.POld = PNew
        self.tOld = tNew

        F = np.array(
            [[1, dt],
            [0, 1]]
        )

        #Control vector to incorporate effects of gravity
        kalmanB = np.array(
            [-0.5*CONST_g * dt ** 2, -CONST_g * dt],
            ndmin = 2)

        xPred = (F @ self.xOld) + kalmanB.T
        PPred = F @ self.POld @ F.T + self.Q

        y = zMeas - xPred
        S = PPred + self.R

        K = PPred @ np.linalg.inv(S)
        self.xNew = xPred + K @ y
        self.PNew = (np.identity(2) - K) * PPred

        return((self.xNew, self.PNew))

class movAvgFilt:
    CONST_PRES_AVG_SET_SIZE = 15

    def __init__(self, tInit, altInit):
        self.altSet = [altInit] * self.CONST_PRES_AVG_SET_SIZE
        self.timeSet = [0.05] * self.CONST_PRES_AVG_SET_SIZE
        self.tOld = tInit
        self.altOld = altInit

    def nextData(self, tNew, zMeas):
        dt = tNew - tOld
        tOld = tNew

        del altSet[0]
        del timeSet[0]
        movAvgAltSet.append(presToAlt(csvData[1]))
        movAvgTimeSet.append(dt)

        self.altNew = np.mean(movAvgAltSet)
        return(altNew, (altNew - altOld) / np.mean(timeSet))
        self.altOld = self.altNew

# ---------------------  Constants  --------------------------
FILE_NAME = 'SkyPilot_IREC_2019_Dataset_Post_Motor_Burnout.csv'
# Available file names:
# SkyPilot_IREC_2019_Dataset_Post_Motor_Burnout.csv

CONST_R = 287.058
CONST_T = 30 + 273.15
CONST_g = 9.8

CONST_APOGEE_CHECKS = 5


CONST_NUM_POST_APOGEE_VALS = 20*30 #30 seconds
# # -------------------------------------------- --------
# print(__file__)
# input()


print("Opening file")
try:
    testDataFile = open(os.path.join(__file__, '..', FILE_NAME), 'r')
    testDataReader = csv.reader(testDataFile, dialect='excel', quoting=csv.QUOTE_NONNUMERIC)
except Exception as e:
    print("Failed to open file. Error:")
    print(e)
    print("Press any key to exit")
    input()
    sys.exit()

time.sleep(0.5) # let file settle

print("File opened. Press any key to begin...")
input()

# Setup file -------------------------------

# Version 1: First row is version number.
# 2nd row contains initial altitude, velocity, covariance matrix,
#           (row major order), initial time (ms), baseline pressure
try:
    csvData = next(testDataReader) #First row is 'version', version number
    assert(csvData[0] == 1) #only have version 1 so far

    csvData = next(testDataReader) #Collect data from 2nd row
    xNew = np.reshape(csvData[0:2], (2,1))
    PNew = np.reshape(csvData[2:6], (2,2))
    tOld = csvData[6]
    CONST_BASE_PRESSURE = csvData[7]


except StopIteration:
    print("No data available. Press any key to exit.")
    input()
    sys.exit()
except AssertionError:
    print("Unrecognized file version. Press any key to exit.")
    input()
    sys.exit()

kf = kalFilt(tOld, xNew, PNew)
mvavg = movAvgFilt(tOld, xNew[0])

# https://en.wikipedia.org/wiki/Hypsometric_equation
presToAlt = lambda pres : CONST_R * CONST_T / CONST_g * math.log(CONST_BASE_PRESSURE / pres)

apogeeCount = 0

# Plotting variables ----------------------------------
tPlot = []
measPlot = []
xPlot = []
vPlot = []
movAvgXPlot = []

# Moving average filter -----------------
movAvgApogeeCount = 0


kalmanApogee = False
movAvgApogee = False

numPostApogeeVals = 0

#main loop ------------------------------------
while True:
    try: #Pull next set of data
        csvData = next(testDataReader)
    except StopIteration:
        print("Reached end of file. Press any key to exit.")
        input()
        sys.exit()

    #Take in measurement
    tNew = csvData[0] / 1000
    z[0] =  presToAlt(csvData[1])
    z[1] = (z[0] - xOld[0]) / dt

    #update filters
    kf.nextData(tNew, z)


    # Save for plotting
    tPlot.append(tNew)
    xPlot.append(xNew[0])
    vPlot.append(xNew[1])
    measPlot.append(presToAlt(csvData[1]))

    if (xNew[0] < xOld[0]):
        apogeeCount += 1
    else:
        apogeeCount = 0

    if apogeeCount >= CONST_APOGEE_CHECKS and not kalmanApogee:
        print("Kalman filter detected apogee at time {}".format(tNew))
        kalmanApogee = True

    #Moving average filter --------------


    movAvgXPlot.append(movAvgNewAlt)

    if movAvgVel < 0:
        movAvgApogeeCount += 1
    else:
        movAvgApogeeCount = 0

    if movAvgApogeeCount >= CONST_APOGEE_CHECKS and not movAvgApogee:
        print("Moving average detected apogee at time {}".format(tNew))
        movAvgApogee = True

    if kalmanApogee and movAvgApogee:
        numPostApogeeVals += 1

    if numPostApogeeVals > CONST_NUM_POST_APOGEE_VALS:
        break

#Reached apogee; plot results
plt.subplot(121)
plt.plot(tPlot, measPlot, 'g', tPlot, xPlot, 'b', tPlot, movAvgXPlot, 'r')
plt.subplot(122)
plt.plot(tPlot, vPlot)
plt.show()

input()
