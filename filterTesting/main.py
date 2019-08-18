from fileName import FILE_NAME
import fileWrapper
import filters

import statistics
import matplotlib.pyplot as plt  # plotting
import numpy as np  # for the linear algebra


def getScores(rawTimes, scores):
    typ = statistics.median(rawTimes)  # typical value
    for index, rawTime in enumerate(rawTimes):
        score = typ - rawTime  # the earlier, the better
        if score > 3:
            # crude determiner that the score is too high
            score *= 2

        try:  # is this how EAFP works
            scores[index] += score
        except TypeError:
            scores[index] = score


#  ---------------------  Constants  --------------------------
CONST_APOGEE_CHECKS = 5
scores = [None] * 2    # 2 filts, currently

# define filters
kf = filters.kalFilt()
mvavg = filters.movAvgFilt()


# Run all filters through all files
for files in FILE_NAME:
    testDataReader, CONST_BASE_PRESSURE, tInit, xInit, PInit  \
        = fileWrapper.setupFile(FILE_NAME[0])

    kf.reset(tInit, xInit, PInit)
    mvavg.reset(tInit, xInit[0])

    fileWrapper.runFile(testDataReader, CONST_BASE_PRESSURE, False, kf, mvavg)

    rawTimes = [kf.apogeeTime, mvavg.apogeeTime]
    getScores(rawTimes, scores)

ind = np.arange(2)
plt.bar(ind, scores)
plt.show()
print(scores)


input()
