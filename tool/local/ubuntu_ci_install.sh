#!/bin/sh

sudo apt-get update
sudo apt-get install -y shellcheck

# Install bashunit
curl -s https://bashunit.com/install.sh | bash

# Install redo from dotmpe/redo
REDO_TMP="$(mktemp -d)"
# NOTE: need tags and cannot specify --depth 1
# TODO: check out github distributions +redo
git clone --branch ifdone https://github.com/dotmpe/redo.git "$REDO_TMP"
(
  cd "$REDO_TMP"
  ./do -j10 build
  sudo DESTDIR= PREFIX=/usr/local ./do -j10 install
)
sudo rm -rf "$REDO_TMP"
