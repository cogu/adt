# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import os

# -- Project information -----------------------------------------------------

project = 'ADT'
copyright = '2026, Conny Gustafsson'
author = 'Conny Gustafsson'
release = '0.3.5'

# -- General configuration ---------------------------------------------------

templates_path = ['_templates']

extensions = [
    'sphinx_design',
    'sphinx.ext.githubpages',
    'sphinxcontrib.mermaid',
    'breathe',
]

# -- Breathe configuration ---------------------------------------------------

breathe_projects = {
    'ADT': '_build/doxygen/xml',
}
breathe_default_project = 'ADT'
breathe_domain_by_extension = {"h": "c"}


# Run Doxygen automatically if installed
import shutil
import subprocess

docs_dir = os.path.abspath(os.path.dirname(__file__))
doxyfile_path = os.path.join(docs_dir, 'Doxyfile')
doxygen_cmd = shutil.which('doxygen')

# Also check for local virtualenv binary if not in system PATH
if not doxygen_cmd:
    venv_doxygen = os.path.abspath(os.path.join(docs_dir, '..', '.venv', 'bin', 'doxygen'))
    if os.path.exists(venv_doxygen):
        doxygen_cmd = venv_doxygen

if doxygen_cmd and os.path.exists(doxyfile_path):
    os.makedirs(os.path.join(docs_dir, '_build', 'doxygen'), exist_ok=True)
    subprocess.run([doxygen_cmd, 'Doxyfile'], cwd=docs_dir, check=False)



exclude_patterns = [
    '_build',
    '.venv',
    'README.md',
    'requirements*.txt'
]


# -- Options for HTML output -------------------------------------------------

html_theme = 'furo'
html_title = f"{project} {release} documentation"
html_static_path = ['_static']
html_css_files = [
    'custom.css',
]

source_suffix = {
    '.rst': 'restructuredtext',
    '.md': 'markdown',
}
