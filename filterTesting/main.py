from fileName import FILE_NAMES
import fileWrapper
import filters

import statistics
import matplotlib.pyplot as plt  # plotting
import numpy as np  # for the linear algebra

filters.kalFilt()


def updateScores(scores, rawTimes, apogeeTime):
    #scores: lower is better
    correctedTimes = [(time - apogeeTime/1000) ** 2 for time in rawTimes]
    if scores == []:
        return correctedTimes
    else:
        return [sum(x) for x in zip(scores, correctedTimes)]


#  ---------------------  Constants  --------------------------
CONST_APOGEE_CHECKS = 5

# define filters
filtSkips = [filters.kalFilt(i) for i in range(1, 41, 10)]
for i, filt in enumerate(filtSkips):
    filt.name = "Skip" + str(i)

filtNoSkips = [filters.kalFilt(i, False) for i in range(1, 20, 5)]
for i, filt in enumerate(filtNoSkips):
    filt.name = "NoSkip" + str(i)
filts = filtSkips# + filtNoSkips

scores = []

# Run all filters through all files
for fileName in FILE_NAMES:
    fileDict = fileWrapper.setupFile(fileName)

    for filt in filts:
        filt.reset(fileDict['datInit'])

    fileWrapper.runFile(fileDict['dataReader'],
                        fileDict['basePressure'], filts, True)

    rawTimes = [filt.apogeeTime for filt in filts]
    scores = updateScores(scores, rawTimes, fileDict['apogeeTime'])

# Normalize scores based on number of files
scores = [score / len(FILE_NAMES) for score in scores]

filtNames = [filt.name for filt in filts]

ind = np.arange(len(filts))
plt.bar(ind, scores, tick_label=filtNames)
plt.show()
print(scores)


input()
