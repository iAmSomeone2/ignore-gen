#!/usr/bin/env python3

from requests import get
from os import path, getenv
import hashlib

_DB_DL_URL = "https://ignore-gen-db.us-east-1.linodeobjects.com/ignore.db"
_DB_FILE_PATH = path.join(getenv('MESON_SOURCE_ROOT', default="."), 'data', 'ignore.db')
_DB_SHA256_SUM = "4c0d1160ab9ad78a41a50632a54fb231490a9a000d15bd8dad01280cb31af606"

# Check if file already exists and is correct
should_dl = True
if (path.exists(_DB_FILE_PATH)):
    print("\"{}\" already exists. Verifying...".format(_DB_FILE_PATH))
    existing_file_sum = ""
    with open(_DB_FILE_PATH, "rb") as db_file:
        hash_sha256 = hashlib.sha256()
        hash_sha256.update(db_file.read())
        existing_file_sum = hash_sha256.hexdigest()
    should_dl = existing_file_sum != _DB_SHA256_SUM

    if should_dl:
        print("Existing file verified.")
    else:
        print("Verification failed. File will be replaced.")

if not should_dl:
    exit(0)

# New file must be downloaded
with open(_DB_FILE_PATH, "wb") as db_file:
    print("Downloading file...")
    response = get(_DB_DL_URL)
    db_file.write(response.content)

print("done")
