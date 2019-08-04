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


# https://en.wikipedia.org/wiki/Hypsometric_equation
def presToAlt(pres, basePres): return CONST_R * CONST_T / CONST_g * \
    math.log(basePres / pres)

# Setup file -------------------------------


def setupFile(fileName, debug=False):

    if debug:
        print("Opening file {}".format(fileName))

    try:
        testDataFile = open(os.path.join(__file__, '..', fileName), 'r')
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

    try:
        # First row is reserved for version
        csvData = next(testDataReader)
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
            basePressure = csvData[7]

        # Version 2:
        # Only the baseline pressure is provided, in position 7 (index from 0) for compatibility with previous versions. Remaining values are ignored (replace with zeros for new file).
        #
        # This would be equivalent to a hard transition between moving average and Kalman filter (no information is passed between the two filters).
        else:
            csvData = next(testDataReader)
            basePressure = csvData[7]

            # Pull next two values to initialize data
            initData1 = next(testDataReader)
            initData2 = next(testDataReader)

            tInit = initData2[0] / 1000
            initAlt1 = presToAlt(initData1[1], basePressure)
            initAlt2 = presToAlt(initData2[1], basePressure)

            xInit = np.array((initAlt2,
                              (initAlt2 - initAlt1) /
                              (initData2[0] - initData1[0])
                              ))
            PInit = filters.CONST_BAROM_UNCER

    except StopIteration:
        print("No data available in {}. Press any key to exit.".format(fileName))
        input()
        sys.exit()
    except AssertionError:
        print("Unrecognized file version for {}. Press any key to exit.".format(fileName))
        input()
        sys.exit()

    return testDataReader, basePressure, tInit, xInit, PInit


def runFile(testDataReader, basePressure, plot=True, *args):
    if plot:
        # Plotting variables ----------------------------------
        tPlot = []
        measPlot = []

        xPlots = [list() for x in range(len(args))]
        # vPlots = []

        # xUncerPlot = []
        # vUncerPlot = []

        numPostApogeeVals = 0
        totalPostApVals = CONST_NUM_POST_APOGEE_VALS

    allApogeed = False

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
        altNew = presToAlt(csvData[1], basePressure)

        # update filters
        allApogeed = True
        for filt in args:
            filt.nextData(tNew, altNew)
            if not filt.atApogee:
                allApogeed = False

        # kf.nextData(tNew, altNew)
        # mvavg.nextData(tNew, altNew)

        if plot:
            # Save for plotting
            tPlot.append(tNew)
            measPlot.append(presToAlt(csvData[1], basePressure))

            for count, filt in enumerate(args):
                xPlots[count].append(filt.xNew[0])

            # Take square root to get standard deviation, and multiply by 2 for 95% range
            # xUncerPlot.append(kf.PNew[0][0]**0.5 * 2)
            # vUncerPlot.append(kf.PNew[1][1]**0.5 * 2)
            # movAvgXPlot.append(mvavg.altNew)
            # vPlot.append(kf.xNew[1])

            if allApogeed:
                numPostApogeeVals += 1

            if numPostApogeeVals > totalPostApVals:
                break

        elif allApogeed:
            break

    if plot:
        # Reached apogee; plot results
        # plt.subplot(121)
        plt.plot(tPlot, measPlot, 'g')
        for count, filt in enumerate(args):
            plt.plot(tPlot, xPlots[count])
            plt.scatter(filt.apogeeTime, filt.apogeeHeight, s=200, marker='x')

        # plt.errorbar(tPlot, xPlot, yerr=xUncerPlot, fmt='b')  # Kalman
        # plt.subplot(122)
        # plt.errorbar(tPlot, vPlot, yerr=vUncerPlot)
        plt.show()


testDataReader, CONST_BASE_PRESSURE, tInit, xInit, PInit  \
    = setupFile(FILE_NAME)

kf = filters.kalFilt(tInit, xInit, PInit)
mvavg = filters.movAvgFilt(tInit, xInit[0])
runFile(testDataReader, CONST_BASE_PRESSURE, True, kf, mvavg)

input()
