#! /bin/sh
#
# update.sh
# Copyright (C) 2026 hari <hari@t470p>
#
# Distributed under terms of the MIT license.
#


      # bashunit+llm+docs+update: |
        mkdir -p .local/agent &&
        curl -s https://bashunit.com/llms.txt \
          -o .local/agent/bashunit-docs-index.txt &&
        curl -s https://bashunit.com/llms-full.txt \
          -o .local/agent/bashunit-docs.txt

