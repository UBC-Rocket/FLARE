import filters
import numpy as np
import unittest


# def floatEqual(f1, f2):
#     if abs(f1 - f2) < 0.01:
#         return True
#     else:
#         return False


class testKFcalcV(unittest.TestCase):

    def test_simpleAscender(self):
        xInit = np.array((0, 20))
        PInit = np.array(((1, 0), (0, 1)))
        testkf = filters.kalFilt(1)
        testkf.reset(0, xInit, PInit)

        # 1 m / s constant
        for i in range(1, 20):
            vMeas, vUncer = testkf._calcV(i * 0.05, i*0.05)
            testkf.xNew = np.array([i * 0.05, 1])

            if testkf._vSkip and testkf._vCounter <= 0:
                testkf._altOld = testkf.xNew[0]
                testkf._vCounter = testkf._vLen

            # testkf._altOld = i * 0.05
            # testkf._tNew = i * 0.05
            self.assertAlmostEqual(vMeas, 1, 5)

    def test_skipv(self):
        xInit = np.array((0, 1))
        PInit = np.array(((1, 0), (0, 1)))
        testkf = filters.kalFilt(3)  # measures every 3rd
        testkf.reset(0, xInit, PInit)

        # 1 m / s constant
        for i in range(1, 20):
            vMeas, vUncer = testkf._calcV(i * 0.05, i*0.05)
            testkf.xNew = np.array([i * 0.05, 1])

            if testkf._vSkip and testkf._vCounter <= 0:
                testkf._altOld = testkf.xNew[0]
                testkf._vCounter = testkf._vLen

            if vUncer < 1000:  # means it's giving a trusted answer
                self.assertAlmostEqual(vMeas, 1, 5)

    def test_noskipv(self):
        xInit = np.array((0, 1))
        PInit = np.array(((1, 0), (0, 1)))

        testkf = filters.kalFilt(3, False)  # measures every 3rd
        testkf.reset(0, xInit, PInit)

        # 1 m / s constant
        for i in range(1, 20):
            tNew = i * 0.05
            vMeas, vUncer = testkf._calcV(tNew, i*0.05)

            testkf.xNew = np.array([i * 0.05, 1])
            testkf._tOld = tNew

            testkf._altOld = testkf.xNew[0]
            testkf._vCounter = testkf._vLen

            if vUncer < 1000:  # means it's giving a trusted answer
                self.assertAlmostEqual(vMeas, 1, 5)

    def test_fancyv(self):
        xInit = np.array((0, 1))
        PInit = np.array(((1, 0), (0, 1)))

        testData = [0, 40, 50, 40, 70, 56, 98, 165, 135, 160, 157, 135]

        kfSkip = filters.kalFilt(3)  # measures every 3rd
        kfSkip.reset(0, xInit, PInit)
        kfNoSkip = filters.kalFilt(3, False)  # measures every 3rd
        kfNoSkip.reset(0, xInit, PInit)

        for i, alt in enumerate(testData, 1):
            tNew = i * 0.05
            # array indexes start at 0, but for other reasons enumerate needs to start at 1
            if i >= 4:
                expectedAlt = testData[i - 1] - testData[i - 4]
            else:
                expectedAlt = testData[i - 1]

            expectedT = 0.05 * 3
            expectedV = expectedAlt / expectedT

            # Yes Skips -----
            vSkipMeas, vSkipUncer = kfSkip._calcV(tNew, alt)
            kfSkip.xNew = np.array([testData[i - 1], 1])

            if kfSkip._vSkip and kfSkip._vCounter <= 0:
                kfSkip._altOld = kfSkip.xNew[0]
                kfSkip._vCounter = kfSkip._vLen

            if vSkipUncer < 1000:
                self.assertAlmostEqual(vSkipMeas, expectedV, 5)

            # No skips ------
            vNoSkipMeas, vNoSkipUncer = kfNoSkip._calcV(tNew, alt)

            kfNoSkip.xNew = np.array([testData[i - 1], 1])
            kfNoSkip._tOld = tNew

            kfNoSkip._altOld = kfNoSkip.xNew[0]
            kfNoSkip._vCounter = kfNoSkip._vLen

            if vNoSkipUncer < 1000:  # means it's giving a trusted answer
                self.assertAlmostEqual(vNoSkipMeas, expectedV, 5)


if __name__ == '__main__':
    unittest.main()
