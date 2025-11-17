# PyPlatec - Python Bindings for Plate Tectonics Library

Python bindings for the plate-tectonics library - a C++ library to simulate plate tectonics.

## Installation

```bash
pip install PyPlatec
```

## Requirements

- Python 3.9 or higher
- C++17 compatible compiler

## Quick Start

```python
import platec

# Create a simulation
p = platec.create(seed=3)

# Run the simulation
while platec.is_finished(p) == 0:
    platec.step(p)

# Get the heightmap
hm = platec.get_heightmap(p)

# Clean up
platec.destroy(p)
```

## Advanced Usage

For more control over the simulation parameters:

```python
import platec

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
