# Main code. Feeds data to Arduino. Hit any button to exit program.

import time #for timing
import msvcrt #console stuff
import sys #to be able to exit
import csv #for access to the pressure data set
import random # for random value generation
import numpy as np # for the linear algebra

# ---------------------  Constants  --------------------------
FILE_NAME = 'SkyPilot_IREC_2019_Dataset_Post_Motor_Burnout.csv'
# Available file names:
# SkyPilot_IREC_2019_Dataset_Post_Motor_Burnout.csv

# -------------------------------------------- --------

print("Opening file")
try:
    testDataFile = open(FILE_NAME, 'r')
    testDataReader = csv.reader(testDataFile, dialect='excel', quoting=csv.QUOTE_NONNUMERIC)
except:
    print("Failed to open file. Press any key to exit.")
    input()
    sys.exit()

time.sleep(1) # let file settle

xOld = np.empty((2, 1))
xNew = np.empty((2, 1))
F = np.empty((2,2))
PNew = np.empty((2,2))
Q = np.empty((2,2))


# Version 1: First row is version number.
# 2nd row contains initial altitude, velocity, and covariance (row major order)
try:
    csvData = next(testDataReader) #First row is 'version', version number
    assert csvData[1] = 1

    csvData = next(testDataReader)
    csvData[0]
    csvData = next(testDataReader)
except StopIteration:
    print("No data available. Press any key to exit.")
    input()
    sys.exit()
except AssertionError:
    print("Unrecognized file version. Press any key to exit.")
    input()
    sys.exit()

while True: #main loop
    
    if (msvcrt.kbhit() and msvcrt.getche().decode("utf-8") == "q"): #Program exit sequence
        print("Exiting")
        time.sleep(0.5)
        sys.exit()
