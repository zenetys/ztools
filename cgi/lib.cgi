#!/bin/bash
## 
## Simple CGI library to be defined as shebang
## Copyright 2020 - Zenetys
## License: MIT

function debug() {
  printf "%s\n" "$*" >&2
}

function header() {
  local name=$1 ; shift
  local value=$1 ; shift
  if [[ $name ]]; then
    printf "%s: %s\r\n" "${name}" "${value}"
  else
    printf "\r\n"
    HEADERS_SENT=1
  fi
}

function html_page() {
  local title=$1; shift
  local body=$1; shift
  echo -n "<html><head><title>$title</title></head>"
  echo -n "<body><h1>$title</h1><p>$body</p></body>"
  echo -n "</html>"
}

function fatal() {
  local msg="$1"; shift
  local msg2=${1:-$msg}; shift

  printf "[FATAL] %s" "$msg2" >&2
  if (( HEADERS_SENT == 1 )); then
    printf "<!-- X-Error: %s -->" "$msg"
  else
    header "Status" 500
    header "X-Error"
    header
    html_page "ERROR 500" "FATAL: $msg"
  fi
  exit 1
}

function echo() {
  local cr="\n"
  [[ ${#1} == 2 && $1 == -n ]] && shift && unset cr
  if (( HEADERS_SENT == 1 )); then
    printf "%s$cr" "$1"
  else
    header "Status: 200"
    header "Content-Type: text/html"
    header
    printf "%s$cr" "$1"
  fi
}

function show_debug_info() {
  local vars=(
    SCRIPT
    NOW
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
    ${!ENV_*}
    ${!_GET_*}
    ${!_POST_*}
    ${!_COOKIE_*}
    ${!_TEMP_*}
  )
  local var next

  for var in ${vars[@]} ; do
    eval -- "[[ \${${var}:60:1} ]] && next='...' || next=''"
    eval -- echo "$var=\"\${${var}:0:60}$next\""
  done
}

function url_decode() {
  url_decode=1 var_decode "$@"
}

function var_decode() {
  local prefix="$1" ; shift
  local url_decode=${url_decode:-0}
  local sub var val
  
  for sub in $*; do
    [[ $sub ]] || continue
    var=${sub%%=*}             # get variable name
    var=${var//[^[:alnum:]]/_} # replace non alnum by '_'
    val=${sub#*=}              # get value
    if (( url_decode == 1 )); then
      val=${val//+/\\x20}        # replace '+' with <SP>
      val=${val//%/\\x}          # replace '%' with \\x (%XX => \xXX)
      val=${val//\\x\\x/%}       # replace \\x\\x (initial double '%') with '%'
    fi
    eval "$prefix${var}=\$'${val}'"
  done
}

function on_exit() {
  [[ $_TEMP_CONTENT_DATA ]] && rm -f $_TEMP_CONTENT_DATA
  [[ $HEADERS_SENT == 1 ]] || header
}

trap on_exit EXIT

[[ ${HTTP_X_DEBUG} -ge 2 ]] && set -x

declare NOW=$(date +%s)
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

if [[ ${HTTP_COOKIE} ]]; then
  IFS='; ' var_decode _COOKIE_ "${HTTP_COOKIE}"
fi

[[ ${HTTP_X_DEBUG} -ge 1 ]] && show_debug_info >&2 2>/dev/null

source $SCRIPT

