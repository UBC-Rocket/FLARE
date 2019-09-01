from fileName import FILE_NAMES
import fileWrapper
import filters

import statistics
import matplotlib.pyplot as plt  # plotting
import numpy as np  # for the linear algebra


def updateScores(scores, rawTimes, apogeeTime):
    #scores: lower is better
    correctedTimes = [abs(time - apogeeTime/1000) for time in rawTimes]
    if scores == []:
        return correctedTimes
    else:
        return [sum(x) for x in zip(scores, correctedTimes)]


#  ---------------------  Constants  --------------------------
CONST_APOGEE_CHECKS = 5

# define filters
kf = filters.kalFilt(1000, False)
kf.name = "Kevin"
kf2 = filters.kalFilt(1, False)
kf2.name = "Martha"
# mvavg = filters.movAvgFilt()

filts = [kf, kf2]
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

filtNames = [filt.name for filt in filts]

ind = np.arange(len(filts))
plt.bar(ind, scores, tick_label=filtNames)
plt.show()
print(scores)


input()
