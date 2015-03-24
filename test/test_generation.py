import unittest
import platec

class TestGeneration(unittest.TestCase):

    def setUp(self):
        pass

    def test_create(self):
        seed = 1
        width = 100
        height = 100
        p = platec.create(seed, width, height, 0.65, 60, 0.02, 1000000, 0.33, 2, 10)
        platec.destroy(p)

    def test_get_heightmap(self):
        seed = 1
        width = 100
        height = 100
        p = platec.create(seed, width, height, 0.65, 60, 0.02, 1000000, 0.33, 2, 10)
        hm = platec.get_heightmap(p)
        platec.destroy(p)
        self.assertAlmostEqual(0.10000000149011612, hm[0])
        self.assertAlmostEqual(0.10000000149011612, hm[1000])
        self.assertAlmostEqual(0.10000000149011612, hm[2000])
        self.assertAlmostEqual(1.7753722667694092,  hm[3000])
        self.assertAlmostEqual(0.10000000149011612, hm[4000])
        self.assertAlmostEqual(0.10000000149011612, hm[5000])
        self.assertAlmostEqual(0.10000000149011612, hm[6000])
        self.assertAlmostEqual(0.10000000149011612, hm[7000])
        self.assertAlmostEqual(1.7841405868530273, hm[8000])
        self.assertAlmostEqual(0.10000000149011612, hm[9000])
        self.assertAlmostEqual(0.10000000149011612, hm[9999])

    def test_get_platesmap(self):
        seed = 1
        width = 100
        height = 100
        p = platec.create(seed, width, height, 0.65, 60, 0.02, 1000000, 0.33, 2, 10)
        pm = platec.get_platesmap(p)
        platec.destroy(p)
        self.assertEqual(2, pm[0])
        self.assertEqual(2, pm[1000])
        self.assertEqual(0, pm[2000])
        self.assertEqual(9, pm[3000])
        self.assertEqual(9, pm[4000])
        self.assertEqual(9, pm[5000])
        self.assertEqual(6, pm[6000])
        self.assertEqual(6, pm[7000])
        self.assertEqual(6, pm[8000])
        self.assertEqual(2, pm[9000])
        self.assertEqual(2, pm[9999])

    def test_step(self):
        pass

    def test_is_finished(self):
        seed = 1
        width = 100
        height = 100
        p = platec.create(seed, width, height, 0.65, 60, 0.02, 1000000, 0.33, 2, 10)
        platec.destroy(p)
        self.assertEqual(False, platec.is_finished(p))
