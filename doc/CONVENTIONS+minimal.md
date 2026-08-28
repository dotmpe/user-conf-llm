---
title: Project conventions (+User-Tools minimal edition)
status: draft
category: [ convention ]
audience: [ developer ]
tag: minimal
---

- The +User-Tools organisation encompasses several projects.

  It's overall scope is *any console or system with interpreter*, and started
  specifically as a *toolkit for Bash profile and script environments*

- The primary project in a way (currently) is +User-Scripts
  that has distribution surface
  the ns02 library layer

- And +User-Conf and other derived, or auxiliary projects

- The goal is a canonical source organisation, for the project and for users
  that want to adopt/extend to allow [for ad hoc to canonical]? 

  The +User-Scripts and selected other projects
  provide the full set of distribution packages to a set of core functions
  within the scope:

  - load on a vanilla bash, and into a system/user profile 
  - provide a selection of modules for interactive setups (ie. PS1, TERM, some Bash/vte/user convenience)
  - and as well for TUI

  and the seed/skeleton repos and data that is used to dev/CI the project
  under some TBD mixed license source/data setup

- The intended end result is local, private systems with part public and private, licensed source

## Scope

- Debian, Apt.
- Bash 4.3+

Status: pre-existing dev, reorganisation.
