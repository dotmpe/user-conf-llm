#! /bin/sh
#
# run-linkml.sh
# Copyright (C) 2026 hari <hari@t470p>
#
# Distributed under terms of the MIT license.
#

gen-project  \
  var/schema/usertools.yaml \
  --dir .local/build/schema/usertools \
  --config-file etc/linkml_generator_config.yaml

tree .local/build/schema/usertools
#
