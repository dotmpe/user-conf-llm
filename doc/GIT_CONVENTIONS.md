## **gitignore** and work tree cleanup

One important role for ignores, is leaving files alone during `git clean -df`
(delete all untracked directories and files). This leaves important setup and
configuration that aren't versioned alone and safe in place. (`git clean -dfx`
would actually do reset the entire tree to a checkout state.)

Cleanable files should usually not be in the `gitignore` globs, but to avoid
committing editor cache and info files some are ignored as well.
