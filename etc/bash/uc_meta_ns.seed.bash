
declare -gA \
uc_meta_ns0=(
  [Batteries]=Batt
  [Operating-System]=OS
  [Shell]=Sh
  [System]=Sys
  [Terminal]=Term
  [User-Config]=User-Conf
)

declare -gA \
uc_meta_ns0_1=(
  [User-Conf]="usertools::userconf"
  [User-Scripts]="usertools::userscripts"
  [User-Tools]="usertools"
)

declare -gA \
uc_meta_ns0_1_1=(
  [User-Conf]=usrtools_usrconf
  [User-Tools]=usrtools
  [User-Scripts]=usrtools_usrscr
)

declare -gA uc_meta_ns0_1_2=(
  [User-Scripts]=usrscr
  [User-Conf]=usrconf
)

declare -gA uc_meta_ns0_1_3=(
  [User-Scripts]=us
  [User-Conf]=uc
)


declare -gA uc_meta_ns1_2_0=(
  [usrtools]=User-Tools
  [usrconf]=User-Conf
  [usrscr]=User-Scripts
)

declare -gA uc_meta_ns1_3_0=(
  [uc]=User-Conf
  [us]=User-Scripts
)


# Id: meta,loader,uc data                        vim:set ft=bash sw=2 sts=2 et:
