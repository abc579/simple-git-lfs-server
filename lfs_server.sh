#!/bin/bash

# Run this after `make`.

LFS_HOST="127.0.0.1"
LFS_PORT="8080"
LFS_FILE_DIRECTORY="files"
LFS_ADMINUSER="temp"
LFS_ADMINPASSWD="temp"
# LFS_CERT="mine.crt"
# LFS_KEY="mine.key"
# LFS_SCHEME="https"

export LFS_HOST LFS_PORT LFS_FILE_DIRECTORY LFS_ADMINUSER LFS_ADMINPASSWD

./out/lfs_server

