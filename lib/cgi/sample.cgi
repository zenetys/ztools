#!/bin/env ./lib.cgi

printf "Status: 200\r\n"
printf "Content-Type: text/plain\r\n\r\n"

[[ $_GET_debug ]] && show_debug_info 2>/dev/null

echo 'Hello!'
