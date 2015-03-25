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
        self.assertEqual(10000, len(hm))
        for v in hm:
            self.assertTrue(v >= 0.0)

    def test_get_platesmap(self):
        seed = 1
        width = 100
        height = 100
        p = platec.create(seed, width, height, 0.65, 60, 0.02, 1000000, 0.33, 2, 10)
        pm = platec.get_platesmap(p)
        platec.destroy(p)
        self.assertEqual(10000, len(pm))
        for v in pm:
            self.assertTrue(10 > v >= 0)

    def test_step(self):
        pass

    def test_is_finished(self):
        seed = 1
        width = 100
        height = 100
        p = platec.create(seed, width, height, 0.65, 60, 0.02, 1000000, 0.33, 2, 10)
        platec.destroy(p)
        self.assertEqual(False, platec.is_finished(p))
