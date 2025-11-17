# Release Process for PyPlatec

This document describes how to create and publish a new release of PyPlatec to PyPI.

## Prerequisites

### 1. PyPI Account Setup
- Create an account on [PyPI](https://pypi.org) if you don't have one
- Create an account on [TestPyPI](https://test.pypi.org) for testing releases
- Enable 2FA on both accounts (required for PyPI)

### 2. Generate API Tokens

#### For PyPI:
1. Go to [https://pypi.org/manage/account/token/](https://pypi.org/manage/account/token/)
2. Click "Add API token"
3. Name it (e.g., "PyPlatec GitHub Actions")
4. Set scope to "Project: PyPlatec" (after first manual upload) or "Entire account" (for first release)
5. Copy the token (starts with `pypi-`)

#### For TestPyPI:
1. Go to [https://test.pypi.org/manage/account/token/](https://test.pypi.org/manage/account/token/)
2. Follow the same steps as above

### 3. Configure Local Authentication

For manual uploads, you have two options:

**Option A: Use API tokens directly with twine**
- When running `twine upload`, you'll be prompted
- Username: `__token__`
- Password: `pypi-your-api-token-here`

**Option B: Create `~/.pypirc` file (recommended)**
```ini
[distutils]
index-servers =
    pypi
    testpypi

[pypi]
username = __token__
password = pypi-your-api-token-here

[testpypi]
repository = https://test.pypi.org/legacy/
username = __token__
password = pypi-your-test-api-token-here
```

## Release Process (Semi-Automated)

The GitHub Actions workflow automatically builds wheels for all platforms. You then download the artifacts and manually upload them to PyPI.

### Steps:

1. **Update the version number** in the following files:
   - `pybindings/setup.py` - Update `version = '1.4.1'`
   - `pybindings/pyproject.toml` - Update `version = "1.4.1"`

2. **Update the changelog** (if you have one):
   ```bash
   # Edit changelog.txt or create CHANGELOG.md
   ```

3. **Commit your changes**:
   ```bash
   git add pybindings/setup.py pybindings/pyproject.toml
   git commit -m "Bump version to 1.4.1"
   ```

4. **Create and push a git tag**:
   ```bash
   git tag v1.4.1
   git push origin master
   git push origin v1.4.1
   ```

5. **Wait for GitHub Actions to build**:
   - Go to the [Actions tab](https://github.com/Mindwerks/plate-tectonics/actions) in your repository
   - Wait for the workflow to complete (builds wheels for Linux, macOS, Windows × Python 3.9-3.13)
   - The workflow will automatically create a GitHub Release with all artifacts

6. **Download the built artifacts**:
   - Go to the [Releases page](https://github.com/Mindwerks/plate-tectonics/releases)
   - Find the release for your tag (v1.4.1)
   - Download all the wheel files and source distribution (.tar.gz)
   - Or download from the Actions run: Click on the workflow run → Scroll to "Artifacts" → Download "final-python-builds"

7. **Upload to PyPI manually**:
   ```bash
   # Navigate to where you downloaded the files
   cd ~/Downloads/final-python-builds  # or wherever you extracted them
   
   # Upload to PyPI using twine
   twine upload *.whl *.tar.gz
   
   # Or if you want to test first on TestPyPI:
   twine upload --repository testpypi *.whl *.tar.gz
   ```

8. **Verify the release**:
   - Check [PyPI](https://pypi.org/project/PyPlatec/) for the new version
   - Test installation: `pip install --upgrade PyPlatec`
   - Verify all wheels are present for different platforms and Python versions

## Local Development and Testing

If you need to build and test locally before creating a release:

### Prerequisites

You **do not need** pyenv for releases. For local development and testing, you can use any Python version manager (pyenv, conda, etc.) or just your system Python.

### Install build tools:

```bash
pip install --upgrade pip build twine
```

### Build the distributions:

```bash
cd pybindings

# Clean previous builds
rm -rf dist/ build/ *.egg-info cpp_src/

# Build source distribution and wheel
python -m build
```

This creates:
- `dist/PyPlatec-1.4.1.tar.gz` (source distribution)
- `dist/PyPlatec-1.4.1-*.whl` (wheel for your platform)

### Test the distribution locally:

```bash
# Create a test virtual environment
python -m venv test_env
source test_env/bin/activate  # On Windows: test_env\Scripts\activate

# Install from the built distribution
pip install dist/PyPlatec-1.4.1.tar.gz

# Test it works
python -c "import platec; print('Success!')"

# Clean up
deactivate
rm -rf test_env
```

### Upload to TestPyPI (recommended before production):

```bash
# Upload to TestPyPI
twine upload --repository testpypi dist/*

# Install from TestPyPI to verify
pip install --index-url https://test.pypi.org/simple/ PyPlatec==1.4.1
```

### Upload to PyPI:

```bash
# Upload to PyPI
twine upload dist/*

# You'll be prompted for authentication if you haven't set up ~/.pypirc
# Username: __token__
# Password: pypi-...your-token...
```

**Note**: For production releases, use the wheels built by GitHub Actions (see main release process above), not locally built wheels. This ensures consistency across all platforms.

### Using .pypirc for authentication:

This is configured in the prerequisites section above. With `~/.pypirc` set up, you can upload with:

```bash
twine upload dist/*  # Uses [pypi] section
twine upload --repository testpypi dist/*  # Uses [testpypi] section
```

## Uploading Downloaded GitHub Actions Artifacts

After downloading the artifacts from GitHub Actions:

```bash
# Extract the downloaded zip if needed
unzip final-python-builds.zip

# Navigate to the directory
cd final-python-builds

# Verify all expected files are present
ls -lh
# You should see:
# - PyPlatec-1.4.1.tar.gz (source distribution)
# - Multiple .whl files for different platforms and Python versions

# Upload to TestPyPI first (recommended)
twine upload --repository testpypi *.whl *.tar.gz

# Test installation from TestPyPI
pip install --index-url https://test.pypi.org/simple/ --no-deps PyPlatec==1.4.1

# If everything works, upload to production PyPI
twine upload *.whl *.tar.gz
```

## Building Wheels for Multiple Platforms

The GitHub Actions workflow handles this automatically. If you need to build locally:

### Using cibuildwheel (recommended for multi-platform wheels):

```bash
pip install cibuildwheel

# Build wheels for current platform
cibuildwheel --platform auto

# Build for specific platform (if on that platform)
cibuildwheel --platform linux
cibuildwheel --platform macos
cibuildwheel --platform windows
```

The configuration in `pyproject.toml` controls which Python versions and architectures are built.

## Post-Release Checklist

- [ ] Verify the package appears on [PyPI](https://pypi.org/project/PyPlatec/)
- [ ] Check that all wheels are present (Linux, macOS, Windows × Python 3.9-3.13)
- [ ] Test installation on at least one platform: `pip install PyPlatec==1.4.1`
- [ ] Verify the PyPI page shows correct metadata and README
- [ ] Update documentation/website with new version info
- [ ] Announce the release (GitHub, Discord, mailing list, etc.)

## Troubleshooting

### "File already exists" error on PyPI
- You cannot replace a release on PyPI
- Bump the version number (e.g., 1.4.1 → 1.4.2) and try again
- Or use patch versions (1.4.1.post1) for fixes

### Missing files in source distribution
- Check `MANIFEST.in` includes all necessary files
- Test: `tar -tzf dist/PyPlatec-*.tar.gz | less`
- Verify all .cpp and .hpp files are listed

### Build failures in GitHub Actions
- Check the Actions tab for detailed logs
- Common issues:
  - Missing C++ compiler on target platform
  - Missing dependencies
  - Syntax errors in source files

### PyPI upload fails with 403 error
- Verify your API token is correct
- Check token has correct scope (project-specific or account-wide)
- Ensure token is added to GitHub Secrets correctly

## Version Numbering

We follow [Semantic Versioning](https://semver.org/):

- **MAJOR** version (1.x.x): Incompatible API changes
- **MINOR** version (x.4.x): New functionality, backwards compatible
- **PATCH** version (x.x.1): Bug fixes, backwards compatible

Examples:
- `1.4.1` → `1.4.2`: Bug fix release
- `1.4.1` → `1.5.0`: New features added
- `1.4.1` → `2.0.0`: Breaking API changes

## Support

For questions or issues with the release process:
- Open an issue: https://github.com/Mindwerks/plate-tectonics/issues
- Contact maintainers: psi29a@gmail.com
