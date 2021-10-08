#
# Copyright (c) ZENETYS and contributors
# License: MIT (https://opensource.org/licenses/MIT)
# Authors: Benoit DOLEZ @ ZENETYS, Julien THOMAS @ ZENETYS
#

# nscan ssh module
# vim: set tabstop=2 shiftwidth=2:

# NOTES:
# - Recursion is not implemented (scan IP from leanred arp, etc..)
# - Do not use sshpass, passwords will be printed in nscan output

SSH=${SSH:-ssh}

require-cmd "${SSH##*/}"
require-cmd sshpass

SSH_TIMEOUT=1 # ConnectTimeout
SSH_ARGS=(
  -o StrictHostKeyChecking=no
  -o UserKnownHostsFile=/dev/null
  -o LogLevel=ERROR
)

# Usage: _ssh [-X] rest...
# Option -X masks the memory
function _ssh() {
  local IFS=$MYIFS
  [[ $1 == -X ]] && { declare -A MEMORY_VAL=(); shift; } # mask memory
  local SSHPASS=${SSHPASS:-${MEMORY_VAL[credential.ssh.$ip.pass]}}
  local cmd=( ${SSHPASS:+sshpass${IFS}-e} "$SSH" ) opts=()
  if [[ -n $SSHPASS ]]; then
    opts+=( -o PreferredAuthentications=password
            -o PubkeyAuthentication=no )
  else
    opts+=( -o PreferredAuthentications=publickey
            -o PubkeyAuthentication=yes )
  fi
  opts+=( "${SSH_ARGS[@]}" ${MEMORY_VAL[credential.ssh.$ip.opts]} "$@" )
  SSHPASS="$SSHPASS" debug-exec -v 2 "${cmd[@]}" "${opts[@]}"
}

# Normalizer function for short form of ssh credentials
# Supported arguments:
# - [stop]
# - user [stop]
# - user keyfile [stop]
# $@: spec parts
# Reply result as kv pairs in AREPLY array
function credential-ssh-norm() {
  if [[ ${@: -1} == stop ]]; then
    AREPLY[stop]=1
    set -- "${@:1:$#-1}"
  fi
  (( $# > 0 )) && AREPLY[User]=$1; shift
  (( $# > 0 )) && AREPLY[IdentityFile]=$1; shift
  return 0
}

# Normalizer function for short form of sshpass credentials
# Supported arguments:
# - user password [stop]
# $@: spec parts
# Reply result as kv pairs in AREPLY array
function credential-sshpass-norm() {
  if [[ ${@: -1} == stop ]]; then
    AREPLY[stop]=1
    set -- "${@:1:$#-1}"
  fi
  (( $# == 2 )) || return 1
  AREPLY[User]=$1
  AREPLY[Password]=$2
  return 0
}

# Checker function for ssh credentials. When this function gets called,
# KV properties of the credential to check are available in AREPLY.
function credential-sshpass-check() { credential-ssh-check "$@"; }
function credential-ssh-check() {
  local opts=() pass= out

  # allow per target timeout override
  opts+=( -o ConnectTimeout="${AREPLY[ConnectTimeout]:-$SSH_TIMEOUT}" )
  unset 'AREPLY[ConnectTimeout]'
  # other ssh optios configured for the credential
  for i in "${!AREPLY[@]}"; do
    [[ $i == Password ]] && { pass=${AREPLY[$i]}; continue; }
    opts+=( -o "$i=${AREPLY[$i]}" )
  done

  # use -X to disable memory for this _ssh call
  out=$(SSHPASS="$pass" _ssh -X "${opts[@]}" "$ip" 'uname -a') || return 1
  learn credential "ssh.$ip.opts" "${opts[@]}"
  learn credential "ssh.$ip.pass" "$pass"
  learn credential "ssh.$ip.uname" "$out"
  return 0
}

function cb-device-ssh() {
  local table=$1; shift # ignored
  local ip=$1; shift
  local uname="${MEMORY_VAL[credential.ssh.$ip.uname]}"
  echo "$uname"
  case "${uname,,}" in
    *linux*) learn ssh-linux "$ip" ;;
  esac
  return 0
}

function remote_linux() {
  #df -P
  #free
  #ps -ax -o pid,ppid,user,pcpu,cputime,lstart,rsz,vsz,comm,cmd
  #cat /proc/net/dev
  /sbin/ip address show || /sbin/ifconfig -a
  /sbin/ip route show table all || /sbin/route -n
  #/sbin/ip rule show
  /sbin/ip neigh show || /usr/sbin/arp -an
  #cat /etc/resolv.conf
  #declare -p | grep -i proxy
  #/bin/netstat -anp
}

function cb-device-ssh-linux() {
  local table=$1; shift
  local ip=$1; shift
  { echo 'unset LANG'
    declare -f remote_linux
    echo 'remote_linux'
  } |_ssh "$ip" "bash -x" 2>&1
}

pqueue-register device cb-find-credential ssh
pqueue-register device cb-find-credential sshpass ssh
pqueue-register ssh cb-device-ssh
pqueue-register ssh-linux cb-device-ssh-linux
