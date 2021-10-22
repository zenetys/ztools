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

Here is a good example on how to run nscan:

```sh
$ nscan -C -v --bash 'ssh pif2-monitoring bash' -P 0 -m snmp -o data.nscan -N 10.21.0.0/16 -D 10.21.208.39 10.21.201.254
```

* colorize stderr,
* set verbosity to level 1,
* run nscan from host pif2-monitoring,
* don't limit parallel tasks,
* load extra module "snmp",
* save output to data.nscan,
* allow to scan discovered IP addresses if they belong to the 10.21.0.0/16 network,
* use DNS server 10.21.208.39 to find names,
* starts the scan by querying 10.21.201.254.

## Credentials

For each IP address identified and queued for scanning, a list of credential
rules is walked through in order to try gathering more data on the device than
a simple ping and reverse DNS lookup. Evaluation of the credential rules stops,
as soon as one credential is found valid, or if a special stop flag is set on
the credential rule.

Credentials can be defined on the command line with option `-c` or by adding
entries to the `CREDENTIALS` variable when using a configuration file. The
format of a credential is a combination of `<destination>` (glob-like pattern
to match on the IP address), `<type>` and `<options>`.

Example on the command line:

```sh
$ nscan ... \
    -c '192.168.56.*:snmp:v2:ro_com_56:stop' \
    -c '10.1.1.11:snmp:v3:monitoring:SHA:authpass:AES:privpass:stop' \
    -c '10.*:snmp:v2:snmp_ro' \
    -c '*:snmp:v2:public' \
    -c '*:snmp:v1:public' \
    ...
```

The same can be achieved with a configuration file, eg:

```sh
$ nscan ... -f config ...
$ cat config
CREDENTIALS+=( '192.168.56.*:snmp:v2:ro_com_56:stop' )
CREDENTIALS+=( '10.1.1.11:snmp:v3:monitoring:SHA:authpass:AES:privpass:stop' )
CREDENTIALS+=( '10.*:snmp:v2:snmp_ro' )
CREDENTIALS+=( '*:snmp:v2:public' )
CREDENTIALS+=( '*:snmp:v1:public' )
```

When using a configuration file, the `CREDENTIALS` variable may be treated as
string, one credential rule by line:

```sh
$ nscan ... -f config ...
$ cat config
CREDENTIALS='
# dst:type:option...
192.168.56.*:snmp:v2:ro_com_56:stop
10.1.1.11:snmp:v3:monitoring:SHA:authpass:AES:privpass:stop
10.*:snmp:v2:snmp_ro
*:snmp:v2:public
*:snmp:v1:public
'
```

This was the short format to define credentials. A longer and more explicit
format is possible. It has the advantage of not discarding the `:` in options
and can also be used either on the command line with `-c` or in a configuration
file with the `CREDENTIALS` variable:

```sh
$ nscan ... -f config ...
$ cat config
CREDENTIALS='
# dst         type  properties
192.168.56.*  snmp  version=2c community=ro_com_56 stop=1
10.1.1.11     snmp  version=3 secLevel=authPriv secName=monitoring authProtocol=SHA authPassword=authpass privProtocol=AES privPassword=privpass stop=1
10.*          snmp  version=2c community=snmp_ro
*             snmp  version=2c community=public
*             snmp  version=1 community=public
'
```

The long format allows for quoted strings, so it should be possible to define
passwords with any character.
