#!/bin/bash
## 
## Simple CGI library to be defined as shebang
## Copyright 2020 - Zenetys
## License: MIT

function show_debug_info() {
  local vars=(
    SCRIPT
    PWD
    UID EUID
    HOSTNAME
    CONTENT_LENGTH
    CONTENT_TYPE
    CONTEXT_DOCUMENT_ROOT
    CONTEXT_PREFIX
    DOCUMENT_ROOT
    GATEWAY_INTERFACE
    QUERY_STRING
    REMOTE_{ADDR,PORT}
    REQUEST_{METHOD,SCHEME,URI}
    SCRIPT_FILENAME
    SCRIPT_NAME
    ${!SERVER_*}
    ${!HTTP_*}
    ${!_GET_*}
    ${!_POST_*}
    ${!_TEMP_*}
  )
  local var next

  for var in ${vars[@]} ; do
    eval -- "[[ \${${var}:60:1} ]] && next='...' || next=''"
    eval -- echo "$var=\${${var}:0:60}$next"
  done
}

function url_decode() {
  local prefix="$1" ; shift
  local sub var val
  
  for sub in $*; do
    [[ $sub ]] || continue
    var=${sub%%=*}             # get variable name
    var=${var//[^[:alnum:]]/_} # replace non alnum by '_'
    val=${sub#*=}              # get value
    val=${val//+/\\x20}        # replace '+' with <SP>
    val=${val//%/\\x}          # replace '%' with \\x (%XX => \xXX)
    val=${val//\\x\\x/%}       # replace \\x\\x (initial double '%') with '%'
    eval "$prefix${var}=\$'${val}'"
  done
}

function on_exit() {
  [[ $_TEMP_CONTENT_DATA ]] && rm -f $_TEMP_CONTENT_DATA
}

trap on_exit EXIT

[[ ${HTTP_X_DEBUG} -ge 1 ]] && set -x

declare SCRIPT="$1"; shift

if [[ ${QUERY_STRING} ]]; then
  IFS='&' url_decode _GET_ "$QUERY_STRING"
fi

if [[ ${CONTENT_LENGTH} ]]; then
  if (( ${CONTENT_LENGTH} < 8192 )); then
    IFS='&' url_decode _${REQUEST_METHOD}_ "$(cat)"
  else
    # do not parse too long POST
    declare _TEMP_CONTENT_DATA=$(mktemp)
    cat > $_TEMP_CONTENT_DATA
  fi
fi

[[ ${HTTP_X_DEBUG} -ge 1 ]] && show_debug_info >&2 2>/dev/null

source $SCRIPT

