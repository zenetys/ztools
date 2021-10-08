Nscan is an attempt to discover network devices the intelligent way. A typical
use case is to start targeting a gateway, it will give useful data like its ARP
table, CDP/LLDP neighbors, connected networks, other routers, etc. All these are
new IP addresses to scan, and so on...

The script intend to be modular, so the information can be retrieved via SNMP,
SSH, WMI, ..., anything as long as it's implemented. So far the SNMP module is
well advanced and retrieves a lot of network-related data from devices:
interfaces, ARP, routes, neighbors, vlans, switch ports vlan participation, etc.

Nscan output is a raw. It's a stream of data, organized in sections suitable for
parsing and analysis.

## Usage


```text
$ nscan --help
Usage: nscan [OPTION]... TARGET...
Modular network scanner that dumps devices data for later processing

Options:
  -N, --network NET         Network allowed for scanning
  -c, --credential SPEC     Add credential rule
  -P, --parallel MAX        Maximum concurrent tasks or 0 for no limit
  -m, --module NAME         Enable extra modules
  -D, --dns IP              Set DNS server
  -f, --config FILE         Configuration file
  -o, --output FILE         Output file, default stdout
  --bash CMD                Define bash command, eg: 'ssh gateway bash'
  -C, --color               Colorize stderr
  -h, --help                Display this help
  -v, --verbose             Increase verbosity
  --x-debug                 Enable bash debug mode
```

## Example

Run nscan from host pif2-monitoring, use DNS server 10.21.208.39 to find names,
allow to scan discovered IP addresses if they belong to the 10.21.0.0/16
network:

```sh
$ nscan -C -v --bash 'ssh pif2-monitoring bash' -P 0 -m snmp -o data.nscan \
    -N 10.21.0.0/16 -D 10.21.208.39 10.21.201.254
```
