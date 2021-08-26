# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
import sys

#import stratega python bindings
#sys.path.append(os.path.abspath('../out/bindings'))
#sys.path.append(os.path.abspath('../out'))

#print("imported paths", sys.path)
#
#pypath = '../out/build'
#for dir_name in os.listdir(pypath):
#    dir_path = os.path.join(pypath, dir_name)
#    if os.path.isdir(dir_path):
#        sys.path.insert(0, dir_path)
#
#print("imported paths", sys.path)
# -- Manually execute Doxygen, since ReadTheDocs doesn't execute our CMake file
import subprocess

def install_and_import(package):
    import importlib
    try:
        importlib.import_module(package)
    except ImportError:
        import pip
        pip.main(['install', package])
    finally:
        globals()[package] = importlib.import_module(package)

def configureDoxyfile(input_dir, output_dir):
    with open('Doxyfile.in', 'r') as file:
        filedata = file.read()

    filedata = filedata.replace('@DOXYGEN_INPUT_DIR@', input_dir)
    filedata = filedata.replace('@DOXYGEN_OUTPUT_DIR@', output_dir)

    with open('Doxyfile', 'w') as file:
        file.write(filedata)

# Check if we're running on Read the Docs' servers
read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'
breathe_projects = {}
if read_the_docs_build:
    input_dir = '../Stratega'
    output_dir = 'build'
    configureDoxyfile(input_dir, output_dir)
    subprocess.call('doxygen', shell=True)
    breathe_projects['Stratega'] = output_dir + '/xml'
else:
    install_and_import('stratega')


# -- Project information -----------------------------------------------------

project = 'Stratega'
copyright = '2021, Diego Perez-Liebana, Alexander Dockhorn, Jorge Hurtado Grueso, Dominik Jeurissen'
author = 'Diego Perez-Liebana, Alexander Dockhorn, Jorge Hurtado Grueso, Dominik Jeurissen'

# The full version, including alpha/beta/rc tags
release = '01.01.2021'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'breathe',    
    'sphinx.ext.autodoc',
    'sphinx.ext.intersphinx',
    'sphinx.ext.autosummary',
    'sphinx.ext.napoleon',
    #'sphinx_code_tabs',
    'sphinx_tabs.tabs'
    ]
autosummary_generate = True

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']


# -- Breathe Configuration ---------------------------------------------------
breathe_default_project = "Stratega"

# The suffix(es) of source filenames.
# You can specify multiple suffix as a list of string:
# source_suffix = ['.rst', '.md']
source_suffix = '.rst'

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

# Example configuration for intersphinx: refer to the Python standard library.
intersphinx_mapping = {'https://docs.python.org/': None}