from fileName import FILE_NAME
import filters

import time  # for timing
import sys  # to be able to exit
import csv  # for access to the pressure data set
import os  # for files
import math  # for logarithms
import random  # for random value generation

import matplotlib.pyplot as plt  # plotting
import numpy as np  # for the linear algebra

#  ---------------------  Constants  --------------------------
CONST_R = 287.058
CONST_T = 30 + 273.15
CONST_g = 9.8

CONST_APOGEE_CHECKS = 5

CONST_NUM_POST_APOGEE_VALS = 20*30  # 20 Hz * 30 seconds
def presToAlt(pres): return CONST_R * CONST_T / CONST_g * \
    math.log(CONST_BASE_PRESSURE / pres)



print("Opening file")
try:
    testDataFile = open(os.path.join(__file__, '..', FILE_NAME), 'r')
    testDataReader = csv.reader(
        testDataFile, dialect='excel', quoting=csv.QUOTE_NONNUMERIC)
except Exception as e:
    print("Failed to open file. Error:")
    print(e)
    print("Press any key to exit")
    input()
    sys.exit()

time.sleep(0.5)  # let file settle

print("File opened.")
# input()

# Setup file -------------------------------

try:
    # First row is reserved for version
    csvData = next(testDataReader)  # First row is 'version', version number
    assert(csvData[0] <= 2)

    # Version 1:
    # 2nd row contains initial altitude, velocity, covariance matrix,
    #           (row major order), initial time (ms), baseline pressure
    # Naively pull all data from initial row
    if(csvData[0] == 1):
        csvData = next(testDataReader)  # Collect data from 2nd row
        xInit = np.reshape(csvData[0:2], (2))
        PInit = np.reshape(csvData[2:6], (2, 2))
        tInit = csvData[6] / 1000
        CONST_BASE_PRESSURE = csvData[7]

    # Version 2:
    # Only the baseline pressure is provided, in position 7 (index from 0) for compatibility with previous versions. Remaining values are ignored (replace with zeros for new file).
    #
    # This would be equivalent to a hard transition between moving average and Kalman filter (no information is passed between the two filters).
    else:
        csvData = next(testDataReader)
        CONST_BASE_PRESSURE = csvData[7]

        # Pull next two values to initialize data
        initData1 = next(testDataReader)
        initData2 = next(testDataReader)

        tInit = initData2[0] / 1000
        initAlt1 = presToAlt(initData1[1])
        initAlt2 = presToAlt(initData2[1])

        xInit = np.array((initAlt2,
            (initAlt2 - initAlt1) / (initData2[0] - initData1[0])
        ))
        PInit = filters.CONST_BAROM_UNCER



except StopIteration:
    print("No data available. Press any key to exit.")
    input()
    sys.exit()
except AssertionError:
    print("Unrecognized file version. Press any key to exit.")
    input()
    sys.exit()

kf = filters.kalFilt(tInit, xInit, PInit)
mvavg = filters.movAvgFilt(tInit, xInit[0])

# https://en.wikipedia.org/wiki/Hypsometric_equation




# Plotting variables ----------------------------------
tPlot = []
measPlot = []
xPlot = []
vPlot = []
movAvgXPlot = []


numPostApogeeVals = 0
# main loop ------------------------------------
while True:
    try:  # Pull next set of data
        csvData = next(testDataReader)
    except StopIteration:
        print("Reached end of file. Press any key to exit.")
        input()
        sys.exit()

    #Take in measurement
    tNew = csvData[0] / 1000
    altNew = presToAlt(csvData[1])

    # update filters
    kf.nextData(tNew, altNew)
    mvavg.nextData(tNew, altNew)

    # Save for plotting
    tPlot.append(tNew)

    measPlot.append(presToAlt(csvData[1]))
    xPlot.append(kf.xNew[0])
    movAvgXPlot.append(mvavg.altNew)

    vPlot.append(kf.xNew[1])

    if kf.atApogee and mvavg.atApogee:
        numPostApogeeVals += 1

    if numPostApogeeVals > CONST_NUM_POST_APOGEE_VALS:
        break

# Reached apogee; plot results
plt.subplot(121)
plt.plot(tPlot, measPlot, 'g', tPlot, xPlot, 'b', tPlot, movAvgXPlot, 'r')
plt.subplot(122)
plt.plot(tPlot, vPlot)
plt.show()

input()
