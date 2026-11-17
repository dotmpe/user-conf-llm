---
title: Project conventions (+User-Tools source baseline edition)
category: [ convention ]
audience: [ developer ]
tag: baseline
version: 0.1.0-dev
---
---- excised from baseline:

- Keep the project base directory tidy; don't use plurals or case sensitive naming, explain new base directories.

  - Source goes into `src/<package>` or `src/<lang>/...`
  - config goes into `config/<package>`
  - documents go into `doc/<package>`

  If there is but only one language, and one package, then those elements can be left out for simpler organizations.

- For development tracks, a source based installation should be a valid deployment option.

  - Tooling support, common leaf utils and such script extra's can go into `tool/<package>` or `tool/<lang>/...`
  - Executable scripts that want to be included at PATH-type name spaces should go into `tool/<lang>/exec` or be grouped at another `tool/*/exec` path; these may be symlinks to scripts with one or more entrypoints.

- Keep it lean, choose and follow conventions.
  For all further project tooling and devops, like provisioning, testing, environment setup and deployment an external project should be appointed the standard; this project just follows versions, and holds bespoke configuration.

----
[//]:  # Id: CONVENTIONS+baseline+source.md v0.1.0-dev +User-Tools 
