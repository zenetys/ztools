# TCPDUMP-WRAPPER

## Usage

<pre>tcpdump-wrapper/tcpdump-wrapper --help
tcpdump-wrapper-1.0 - Copyright (c) 2018-2019 Benoit DOLEZ - License MIT
Usage: tcpdump-wrapper [options] [tcpdump-options]
Options:
 -h, --help           this help
 --verbose            enable verbose
 --x-debug            enable bash debug mode
 --dry-run            view commands, do not execute
 --save SAVE%         save space up to SIZE percent of disk space
 --no-save            disable auto remove older files
 -C FILESIZE          rotate file after FILESIZE Mo
 -i, --interface NAME interface name (needed)
 -Z, --relinquish-privileges USER user to switch to
 -w FILENAME          raw output base name (auto suffix)
 -s SNAPLEN           capture size per packet
</pre>
