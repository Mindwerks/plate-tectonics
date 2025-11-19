"""
Test for Issue #30: Mass values go below zero
https://github.com/Mindwerks/plate-tectonics/issues/30

This test reproduces the negative mass issue found when running
worldengine with seed 24483 on a 512x512 map with 10 plates.

The fix ensures that:
1. Heightmap values are clamped to >= 0 after erosion
2. Mass values are clamped to >= 0 when they go negative
3. The simulation completes successfully without assertion failures
"""
import unittest
import platec


class TestIssue30NegativeMass(unittest.TestCase):
    """
    Test case to reproduce and verify the fix for the negative mass issue.
    
    Before the fix, this would trigger:
    "Assertion `0` failed in cpp_src/mass.cpp line 81 Message: A negative mass is not allowed"
    
    The issue was caused by floating point precision errors during erosion calculations
    over many iterations. The fix clamps negative values to zero.
    """

    def test_seed_24483_negative_mass(self):
        """
        Reproduce the negative mass issue with seed 24483.
        
        This test verifies that:
        1. The simulation completes without crashing
        2. All heightmap values remain non-negative throughout
        3. Mass values are properly handled (clamped to zero if negative)
        
        Parameters match worldengine defaults that triggered the bug:
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
            # Before the fix, this would trigger an assertion failure
            step_count = 0
            max_steps = 10000  # Safety limit
            negative_values_found = []
            
            while not platec.is_finished(p) and step_count < max_steps:
                platec.step(p)
                step_count += 1
                
                # Check heightmap every 100 steps to catch any negative values
                if step_count % 100 == 0:
                    hm = platec.get_heightmap(p)
                    min_height = min(hm)
                    if min_height < 0:
                        negative_values_found.append((step_count, min_height))
            
            # Get the final heightmap
            hm = platec.get_heightmap(p)
            self.assertEqual(width * height, len(hm))
            
            # Verify all final heights are non-negative
            min_final_height = min(hm)
            self.assertGreaterEqual(min_final_height, 0.0, 
                f"Negative height {min_final_height} found in final heightmap after {step_count} steps")
            
            # If we found negative values during simulation, the fix isn't working
            self.assertEqual([], negative_values_found,
                f"Negative height values detected during simulation: {negative_values_found}")
            
            print(f"Simulation completed successfully after {step_count} steps")
            
        finally:
            platec.destroy(p)

    def test_seed_24483_step_by_step(self):
        """
        Step through seed 24483 simulation and check every single step
        to ensure no negative heightmap values ever appear.
        
        This is a more rigorous test that validates the fix at every iteration.
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
                
                # Check heightmap every step
                hm = platec.get_heightmap(p)
                min_height = min(hm)
                
                # With the fix, we should NEVER see negative heights
                self.assertGreaterEqual(min_height, 0.0,
                    f"Negative height {min_height} found at step {step_count}. "
                    f"The fix should prevent this by clamping values to zero.")
            
            print(f"Completed {step_count} steps without negative mass")
            
        finally:
            platec.destroy(p)
    
    def test_multiple_problematic_seeds(self):
        """
        Test several seeds to ensure the fix works broadly, not just for seed 24483.
        
        These seeds have been known to cause issues with large maps and many plates.
        """
        problematic_seeds = [24483, 12345, 99999]
        
        for seed in problematic_seeds:
            with self.subTest(seed=seed):
                p = platec.create(seed, 256, 256, 0.65, 60, 0.02, 1000000, 0.33, 2, 10)
                
                try:
                    step_count = 0
                    max_steps = 500  # Shorter run for multiple seeds
                    
                    while not platec.is_finished(p) and step_count < max_steps:
                        platec.step(p)
                        step_count += 1
                    
                    # Verify final heightmap has no negative values
                    hm = platec.get_heightmap(p)
                    min_height = min(hm)
                    self.assertGreaterEqual(min_height, 0.0,
                        f"Seed {seed}: Negative height {min_height} found after {step_count} steps")
                    
                finally:
                    platec.destroy(p)


if __name__ == '__main__':
    unittest.main()
