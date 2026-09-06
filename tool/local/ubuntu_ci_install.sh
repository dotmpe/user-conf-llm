#!/bin/sh

sudo apt-get update
sudo apt-get install -y shellcheck

# Install bashunit
curl -s https://bashunit.com/install.sh | bash

# Install redo from dotmpe/redo
REDO_TMP="$(mktemp -d)"
git clone --branch ifdone --depth 1 https://github.com/dotmpe/redo.git "$REDO_TMP"
(
  cd "$REDO_TMP"
  ./do -j10 build
  sudo DESTDIR= PREFIX=/usr/local ./do -j10 install
)
sudo rm -rf "$REDO_TMP"
