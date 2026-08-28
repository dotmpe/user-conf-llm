# ~/.bashrc: executed by bash(1) for non-login shells.

: "${ENV_BASE:=rc}"
: "${ENV_CTX:=$0[$$]:~/.bashrc}"
ENV_SRC=${ENV_SRC-}${ENV_SRC:+ }${HOME:-~}/.bashrc

case $- in
    *i*) ;;
      *) return;;
esac

# ! ((user_build)) && return 0

if [[ -d ~/.local/shell/bashrc.d ]]; then
  for i in ~/.local/shell/bashrc.d/[!_]*.{ba,}sh; do
    # FIXME: skip if variant already loaded
    if [[ -r $i ]]; then
      # echo ". $i" >> "$US_USER_INTERACTIVE_SHELL_CACHE"
      . $i
    fi
  done
  unset i
fi

echo Bash RC loaded
# Id: -user-bashrc                               vim:set ft=bash sw=2 sts=2 et:
