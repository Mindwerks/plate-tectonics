"""
Test for Issue #30: Mass values go below zero
https://github.com/Mindwerks/plate-tectonics/issues/30

This test reproduces the negative mass issue found when running
worldengine with seed 24483 on a 512x512 map with 10 plates.
"""
import unittest
import platec


class TestIssue30NegativeMass(unittest.TestCase):
    """
    Test case to reproduce the negative mass assertion failure.
    
    The issue manifests as:
    "Assertion `0` failed in cpp_src/mass.cpp line 81 Message: A negative mass is not allowed"
    
    This happens due to floating point precision issues during erosion calculations
    over many iterations, causing mass values to become slightly negative (beyond
    the epsilon of -0.01).
    """

    def test_seed_24483_negative_mass(self):
        """
        Reproduce the negative mass issue with seed 24483.
        
        Parameters match worldengine defaults:
        - seed: 24483
        - width: 512
        - height: 512
        - num_plates: 10
        - sea_level: 0.65
        - erosion_period: 60
        - folding_ratio: 0.02
        - aggr_overlap_abs: 1000000
        - aggr_overlap_rel: 0.33
        - cycle_count: 2
        """
        seed = 24483
        width = 512
        height = 512
        num_plates = 10
        
        # Create simulation with problematic seed
        # platec.create(seed, width, height, sea_level, erosion_period, folding_ratio,
        #               aggr_overlap_abs, aggr_overlap_rel, cycle_count, num_plates)
        p = platec.create(seed, width, height, 0.65, 60, 0.02, 1000000, 0.33, 2, num_plates)
        
        try:
            # Step through the simulation until completion
            # This should trigger the negative mass assertion
            step_count = 0
            max_steps = 10000  # Safety limit
            
            while not platec.is_finished(p) and step_count < max_steps:
                platec.step(p)
                step_count += 1
            
            # Get the final heightmap to ensure it's valid
            hm = platec.get_heightmap(p)
            self.assertEqual(width * height, len(hm))
            
            # Verify all heights are non-negative
            for v in hm:
                self.assertGreaterEqual(v, 0.0, 
                    f"Negative height found after {step_count} steps")
            
            print(f"Simulation completed successfully after {step_count} steps")
            
        finally:
            platec.destroy(p)

    def test_seed_24483_step_by_step(self):
        """
        Step through seed 24483 simulation more carefully to catch where
        the negative mass issue first appears.
        """
        seed = 24483
        width = 512
        height = 512
        
        p = platec.create(seed, width, height, 0.65, 60, 0.02, 1000000, 0.33, 2, 10)
        
        try:
            step_count = 0
            max_steps = 10000
            
            while not platec.is_finished(p) and step_count < max_steps:
                platec.step(p)
                step_count += 1
                
                # Check heightmap periodically
                if step_count % 100 == 0:
                    hm = platec.get_heightmap(p)
                    min_height = min(hm)
                    if min_height < 0:
                        self.fail(
                            f"Negative height {min_height} found at step {step_count}")
            
            print(f"Completed {step_count} steps without negative mass")
            
        finally:
            platec.destroy(p)


if __name__ == '__main__':
    unittest.main()
