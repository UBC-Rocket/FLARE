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
            # testkf._altOld = i * 0.05
            # testkf._tNew = i * 0.05
            self.assertAlmostEqual(vMeas, 1, 5)

    def test_skip2(self):
        xInit = np.array((0, 20))
        PInit = np.array(((1, 0), (0, 1)))
        testkf = filters.kalFilt(2)
        testkf.reset(0, xInit, PInit)

        # 1 m / s constant
        for i in range(1, 20):
            vMeas, vUncer = testkf._calcV(i * 0.05, i*0.05)
            testkf.xNew = np.array([i * 0.05, 1])
            # testkf._tNew = i * 0.05
            if vUncer < 10000:  # means it's giving a trusted answer
                self.assertAlmostEqual(vMeas, 1, 5)
            else:
                print(vUncer)

    def test_sampleTest(self):
        self.assertEqual(1, 1)


if __name__ == '__main__':
    unittest.main()
