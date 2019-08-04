from fileName import FILE_NAME
import fileWrapper
import filters

import matplotlib.pyplot as plt  # plotting
import numpy as np  # for the linear algebra

#  ---------------------  Constants  --------------------------
CONST_APOGEE_CHECKS = 5


testDataReader, CONST_BASE_PRESSURE, tInit, xInit, PInit  \
    = fileWrapper.setupFile(FILE_NAME)

kf = filters.kalFilt(tInit, xInit, PInit)
mvavg = filters.movAvgFilt(tInit, xInit[0])
fileWrapper.runFile(testDataReader, CONST_BASE_PRESSURE, True, kf, mvavg)

input()
