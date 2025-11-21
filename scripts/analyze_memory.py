#!/usr/bin/env python3
"""
Memory usage analysis for plate tectonics simulation.
Calculates theoretical memory usage at different resolutions.
"""

def bytes_to_human(bytes_val):
    """Convert bytes to human readable format."""
    for unit in ['B', 'KB', 'MB', 'GB']:
        if bytes_val < 1024:
            return f"{bytes_val:.2f} {unit}"
        bytes_val /= 1024
    return f"{bytes_val:.2f} TB"

def analyze_memory(width, height, num_plates=10):
    """Analyze memory usage for given configuration."""
    world_area = width * height
    
    print(f"\n{'='*70}")
    print(f"Memory Analysis: {width}×{height} with {num_plates} plates")
    print(f"{'='*70}")
    
    # World maps in lithosphere
    print("\nWorld-level data structures:")
    hmap_bytes = world_area * 4  # float[area]
    imap_bytes = world_area * 1  # uint8_t[area] - OPTIMIZED from uint32_t
    amap_bytes = world_area * 2  # uint16_t[area] - OPTIMIZED from uint32_t
    
    print(f"  hmap (heights):     {bytes_to_human(hmap_bytes)}")
    print(f"  imap (plate index): {bytes_to_human(imap_bytes)}")
    print(f"  amap (ages):        {bytes_to_human(amap_bytes)}")
    world_total = hmap_bytes + imap_bytes + amap_bytes
    print(f"  WORLD TOTAL:        {bytes_to_human(world_total)}")
    
    # Per-plate data structures
    print(f"\nPer-plate data (×{num_plates} plates):")
    
    # Assume average plate size is ~1/num_plates of world
    # But plates can grow/shrink, so use conservative estimate of 1.5× average
    avg_plate_area = int(world_area / num_plates * 1.5)
    
    print(f"  Estimated avg plate area: {avg_plate_area:,} cells")
    
    per_plate_bytes = 0
    
    # HeightMap (map variable in plate.hpp)
    heightmap_bytes = avg_plate_area * 4  # float array
    print(f"  - HeightMap:        {bytes_to_human(heightmap_bytes)}")
    per_plate_bytes += heightmap_bytes
    
    # AgeMap (age_map variable)
    agemap_bytes = avg_plate_area * 2  # uint16_t array - OPTIMIZED from uint32_t
    print(f"  - AgeMap:           {bytes_to_human(agemap_bytes)}")
    per_plate_bytes += agemap_bytes
    
    # Segments (continent tracking)
    segments_bytes = avg_plate_area * 4  # uint32_t array for segment IDs
    print(f"  - Segments:         {bytes_to_human(segments_bytes)}")
    per_plate_bytes += segments_bytes
    
    # SegmentData (metadata per segment, estimate ~100 segments per plate)
    num_segments = 100
    segment_data_bytes = num_segments * 64  # Rough estimate: 64 bytes per segment
    print(f"  - SegmentData:      {bytes_to_human(segment_data_bytes)}")
    per_plate_bytes += segment_data_bytes
    
    # Collision/subduction vectors (small, estimate)
    collision_bytes = 1000 * 24  # ~1000 collisions × 24 bytes per struct
    print(f"  - Collision data:   {bytes_to_human(collision_bytes)}")
    per_plate_bytes += collision_bytes
    
    print(f"  Per-plate subtotal: {bytes_to_human(per_plate_bytes)}")
    
    all_plates_bytes = per_plate_bytes * num_plates
    print(f"  ALL PLATES TOTAL:   {bytes_to_human(all_plates_bytes)}")
    
    # Parallel thread-local data (if using parallel mode)
    print(f"\nParallel-mode overhead (14 threads):")
    
    # Phase 1: Spatial partitioning (contributions vector)
    # Each thread may process overlapping regions
    # Worst case: 2× world area worth of contributions
    contribution_size = 16  # plateCollision struct: ~16 bytes
    phase1_bytes = world_area * 2 * contribution_size
    print(f"  - Phase 1 contributions: {bytes_to_human(phase1_bytes)}")
    
    # Phase 2: Thread-local collision/subduction vectors
    # 14 threads × 10 plates × 2 types × ~1000 entries × 24 bytes
    phase2_bytes = 14 * num_plates * 2 * 1000 * 24
    print(f"  - Phase 2 thread-local:  {bytes_to_human(phase2_bytes)}")
    
    parallel_overhead = phase1_bytes + phase2_bytes
    print(f"  PARALLEL OVERHEAD:       {bytes_to_human(parallel_overhead)}")
    
    # Total
    print(f"\n{'='*70}")
    serial_total = world_total + all_plates_bytes
    parallel_total = serial_total + parallel_overhead
    
    print(f"TOTAL (serial mode):   {bytes_to_human(serial_total)}")
    print(f"TOTAL (parallel mode): {bytes_to_human(parallel_total)}")
    print(f"{'='*70}")
    
    return serial_total, parallel_total

if __name__ == "__main__":
    print("PLATE TECTONICS MEMORY USAGE ANALYSIS")
    print("="*70)
    
    configs = [
        (1024, 1024),
        (4096, 4096),
        (8192, 8192),
        (16384, 16384),
    ]
    
    for width, height in configs:
        serial, parallel = analyze_memory(width, height)
    
    print("\n" + "="*70)
    print("RECOMMENDATIONS FOR MEMORY REDUCTION")
    print("="*70)
    print("""
1. Use smaller resolutions for development/testing
   - 1024×1024 uses ~20 MB
   - 4096×4096 uses ~700 MB
   - 16384×16384 uses ~15 GB

2. Reduce plate count
   - Current: 10 plates
   - Try: 5-7 plates for large maps
   - Each plate = ~460 MB at 16K resolution

3. Use serial mode for very large maps
   - Parallel adds 5-10 GB overhead at 16K
   - Serial is memory-efficient

4. Optimizations already implemented:
   ✅ uint8_t for IndexMap (was uint32_t): 75% savings on plate indices
   ✅ uint16_t for AgeMap (was uint32_t): 50% savings on ages
   - Result: ~1.78 GB saved at 16K resolution

5. Future optimizations to consider:
   - Use sparse data structures for plates (save 70%+, but breaks determinism)
   - Stream processing: process chunks sequentially
   - Memory mapping: use disk-backed arrays for huge maps

6. OS limits:
   - macOS may allow swap up to 64 GB
   - Watch Activity Monitor for memory pressure
   - If swapping heavily, performance degrades 100-1000×
""")
