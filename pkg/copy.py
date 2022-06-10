#!/usr/bin/env python3

from os import path, getenv, makedirs
from sys import argv
from shutil import copyfile

# Get absolute input and output paths
input_path = path.join(
    getenv('MESON_SOURCE_ROOT'),
    getenv('MESON_SUBDIR'),
    argv[1]
)

output_path = path.join(
    getenv('MESON_BUILD_ROOT'),
    getenv('MESON_SUBDIR'),
    argv[2]
)

# Ensure destination directory exists
makedirs(path.dirname(output_path), exist_ok=True)

# Copy the file
copyfile(input_path, output_path) 
