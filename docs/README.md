## Prerequisites

The documentation toolchain requires **Sphinx** (Python) and **Doxygen** to extract C docstrings.

### Installing Doxygen

- **Linux (Debian / Ubuntu / Linux Mint)**:
  ```bash
  sudo apt install doxygen
  ```
- **Windows (PowerShell)**:
  ```powershell
  winget install Doxygen.Doxygen
  # or
  choco install doxygen.install
  ```
- **macOS**:
  ```bash
  brew install doxygen
  ```

## Python Environment Setup

### Linux

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements-dev.txt
```

### Windows (PowerShell)

```powershell
python -m venv .venv
.\.venv\Scripts\activate
pip install -r requirements-dev.txt
```

## Building the Documentation

### Single Build

Build the HTML documentation once:

```bash
python -m sphinx -b html . _build/html
```

### Live Reload Preview (Development)

To rebuild automatically when documentation files change and preview the site with live reload, run:

```bash
sphinx-autobuild . _build/html
```

The preview server is available at <http://127.0.0.1:8000> by default. Stop it with `Ctrl+C`.
