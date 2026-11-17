---
title: Project conventions (+User-Tools baseline edition)
category: [ convention ]
audience: [ developer ]
tag: baseline
version: 0.1.3
---

This is a baseline for other derived or layered conventions.
(The original copy includes this header with above metadata block, and a closing comment-line repeating these basic Id's: )
From organisation User-Tools, the User-Conf-LLM project; GIT `git@github.com:user-tools/user-conf-llm.git` `doc/CONVENTIONS+baseline.md`

Apply the following rules progressively, start at the top and apply subsequent items when appropriate:

- Describe the project in the README.md, but succinctly.
  Include if possible a background or context, and/or state the objective, add individual goals, methods, and an overview of setup or state and architecture.

- Keep concerns separate and focus on responsibilities, use local AGENTS.md and CONVENTIONS.md leafs like this document if appropriate.
  These should describe the project contents and how the names and conventions are used and applied.

  This conventions for example does not say anything about source code languages in particular, or OPSEC, or even give much specific SCM instructions.
  And it is also not that suitable (as standalone conventions) for larger projects.

- Treat PLAN.md as the current task sketch cq. progression of a task.
  Keep it out of SCM, or plan to migrate its contents (to documentation here or in another project/organisation) to make place.

- If STATUS.md exists, keep it out of SCM, and treat it as the next commit message or some unfinished release log.
  
- Keep comments, open points and tasks mentioned, remarks and margin notes, and such etc. in place while relevant;
  the TODO/FIXME/XXX/BUG/NOTE/... tag may be used inline (code comments, strings, or in plain text) to mark link ends for issues, or other kinds of cards.

- Manage tasks (and the project plan, design or schedule) in TODO.txt entries, and move to DONE.txt on close.
  TBD is an specification for categories, kinds and priorities that is then to-be described and referenced here.

  - TODO.txt entries can use continuation lines for additional free-form body text (this to keep documents/code free from too verbose remarks, and transient or dynamic notes);
    keep these continuation blocks always indented for better parsing (both visually and technically).

    Indicate this additional content on the header line with a classic "line continuation" mark, ie. backslash before the EOL;
    so that the presence is transparent even through line-based consumers, like standard TODO.txt or while e.g. grepping/filtering the header lines only.

  See github.com/todotxt/todo.txt for the basic syntax rules, and github.com/dotmpe/todo-txt for a more extensive and layered definition including limitations on the above with current implementations.

- To make README.md less definite or specific, or to make it more easily readable and targetted at a generic user audience, consider moving or and keeping all main techical reference in HACKING.md (or something else deemed more suitable than that)--even better, provide an INSTALL.md for package managers, and a RELEASE.md for the developers with any more serious project.

- Finally a version should come with some sort of changelog (ie. CHANGELOG.md, and not rely on Git log) but many simple project and experiment that would follow (some of) the above may never get to that, and never have that file.

  In any case follow a simple semver versioning scheme: start at 0.1.0.
  And set 0.1.0-alpha, -beta or some other preferred, selected tag to announce but not commit the stated version yet at all.

----
[//]:  # Id: CONVENTIONS+baseline.md v0.1.3 +User-Tools 
