# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import os

# -- Project information -----------------------------------------------------

project = 'ADT'
copyright = '2026, Conny Gustafsson'
author = 'Conny Gustafsson'
release = '0.3.0'

# -- General configuration ---------------------------------------------------

templates_path = ['_templates']

extensions = [
    'sphinx_design',
    'sphinx.ext.githubpages',
    'sphinxcontrib.mermaid',
]


exclude_patterns = [
    '_build',
    '.venv',
    'README.md',
    'requirements.txt'
]


# -- Options for HTML output -------------------------------------------------

html_theme = 'furo'
html_title = 'ADT Documentation'
html_static_path = ['_static']
html_css_files = [
    'custom.css',
]

source_suffix = {
    '.rst': 'restructuredtext',
    '.md': 'markdown',
}
