from fileName import FILE_NAMES
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

# define filters
kf = filters.kalFilt(5, False)
kf.name = "Kevin"
kf2 = filters.kalFilt(1, False)
kf2.name = "Martha"
# mvavg = filters.movAvgFilt()

filts = [kf, kf2]
scores = [None] * len(filts)

# Run all filters through all files
for files in FILE_NAMES:
    testDataReader, CONST_BASE_PRESSURE, tInit, xInit, PInit  \
        = fileWrapper.setupFile(FILE_NAMES[0])

    for filt in filts:
        filt.reset(tInit, xInit, PInit)

    # fileWrapper.runFile(testDataReader, CONST_BASE_PRESSURE, filts, True)
    fileWrapper.runFile(testDataReader, CONST_BASE_PRESSURE, filts, False)

    rawTimes = [filt.apogeeTime for filt in filts]
    getScores(rawTimes, scores)

filtNames = [filt.name for filt in filts]

ind = np.arange(len(filts))
plt.bar(ind, scores, tick_label=filtNames)
plt.show()
print(scores)


input()
