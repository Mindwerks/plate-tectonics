# PyPlatec - Python Bindings for Plate Tectonics Library

Python bindings for the plate-tectonics library - a C++ library to simulate plate tectonics.

## Installation

```bash
pip install PyPlatec
```

## Requirements

- Python 3.9 or higher
- No compiler needed when installing from PyPI (pre-built wheels available)

## Quick Start

```python
import platec

# Create a simulation with keyword arguments (recommended)
p = platec.create(
    seed=3,
    width=512,
    height=512,
    sea_level=0.65,
    erosion_period=60,
    folding_ratio=0.02,
    aggr_overlap_abs=1000000,
    aggr_overlap_rel=0.33,
    cycle_count=2,
    num_plates=10
)

# Run the simulation
while platec.is_finished(p) == 0:
    platec.step(p)

# Get the heightmap
hm = platec.get_heightmap(p)

# Clean up
platec.destroy(p)
```

## API Reference

### platec.create()

**Important:** All 10 parameters are required. You can use either positional or keyword arguments.

```python
platec.create(seed, width, height, sea_level, erosion_period, folding_ratio,
              aggr_overlap_abs, aggr_overlap_rel, cycle_count, num_plates)
```

**Parameters:**
- `seed` (int): Random seed for the simulation
- `width` (int): Map width in pixels
- `height` (int): Map height in pixels  
- `sea_level` (float): Sea level (0.0-1.0, typically 0.65)
- `erosion_period` (int): Erosion period (typically 60)
- `folding_ratio` (float): Folding ratio (typically 0.02)
- `aggr_overlap_abs` (int): Absolute overlap threshold (typically 1000000)
- `aggr_overlap_rel` (float): Relative overlap threshold (typically 0.33)
- `cycle_count` (int): Number of cycles (typically 2)
- `num_plates` (int): Number of plates (typically 10)

**Example with custom parameters:**

```python
import platec

# Keyword arguments make the code more readable
p = platec.create(
    seed=3,
    width=1000,
    height=800,
    sea_level=0.65,
    erosion_period=60,
    folding_ratio=0.02,
    aggr_overlap_abs=1000000,
    aggr_overlap_rel=0.33,
    cycle_count=2,
    num_plates=10
)

# Or use positional arguments if preferred
p = platec.create(3, 1000, 800, 0.65, 60, 0.02, 1000000, 0.33, 2, 10)
# Run simulation...
```

## Building from Source

```bash
python setup.py build
python setup.py install
```

For development:
```bash
pip install -e .
```

## For Maintainers: Creating a Release

See [RELEASE.md](RELEASE.md) for detailed instructions on:
- Building wheels with GitHub Actions
- Downloading built artifacts
- Publishing releases to PyPI with twine
- Testing with TestPyPI
- Building wheels for multiple platforms

Quick release process:
```bash
# 1. Update version in setup.py and pyproject.toml
# 2. Commit changes
git commit -am "Bump version to 1.4.2"

# 3. Create and push a tag
git tag v1.4.2
git push origin master
git push origin v1.4.2

# 4. Wait for GitHub Actions to build all wheels
# 5. Download artifacts from GitHub Release or Actions tab
# 6. Upload to PyPI: twine upload *.whl *.tar.gz
```

## License

LGPL-3.0-or-later - See LICENSE file in the repository root.

## Links

- [GitHub Repository](https://github.com/Mindwerks/plate-tectonics)
- [WorldEngine](https://github.com/Mindwerks/worldengine) - A world generator using this library
- [Original Project](http://sourceforge.net/projects/platec/)
