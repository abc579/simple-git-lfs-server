#!/bin/bash

# Run this after `make` and after creating your own cert and private key.

LFS_HOST="localhost"
LFS_PORT="9999"
LFS_FILE_DIRECTORY="files"
LFS_ADMINUSER="temp"
LFS_ADMINPASSWD="temp"
LFS_CERT="mine.crt"
LFS_KEY="mine.key"
LFS_SCHEME="https"
LFS_STORAGE_OPTION="local" # For now, only local is supported.

export LFS_HOST LFS_PORT LFS_FILE_DIRECTORY LFS_ADMINUSER LFS_ADMINPASSWD LFS_CERT LFS_KEY LFS_SCHEME LFS_STORAGE_OPTION
