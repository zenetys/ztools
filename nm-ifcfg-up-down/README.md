Although legacy network scripts are still available in RHEL8, they are
deprecated and RedHat will most probably remove them completely in the
next release.

RedHat still supports network interface configuration via ifcfg files
in `/etc/sysconfig/network-scripts` with NetworkManager plugin ifcfg-rh.
Unfortunately they did not implement support for custom route and rules
via `route-<ifname>` and `rule-<ifname>` files. Extra routes can be added
in the `ifcfg-<ifname>` file but this is really not handy to maintain.

The script `nm-ifcfg-up-down` was written to restore the ability to
define custom routes and rules in per interface sysconfig files,
using the iproute2 syntax. It is a NetworkManager dispatcher script
that will be called when an interface goes up or down.

To install it, copy or symlink the script to
`/etc/NetworkManager/dispatcher.d/00-nm-ifcfg-up-down`. Custom routes
or rules can then be defined respectively in `<ifname>.route` and
`<ifname>.rule` sysconfig files, next to the standard `ifcfg-<ifname>`
configuration file.

```sh
$ ls -1 /etc/sysconfig/network-scripts/
[...]
bond1.209.route
bond1.209.rule
ifcfg-bond1.209
[...]

$ cat /etc/sysconfig/network-scripts/bond1.209.route
# custom ip routes for interface bond1.209
10.167.42.94 table rt-lan42
default via 10.167.42.94 table rt-lan42

$ cat /etc/sysconfig/network-scripts/bond1.209.rule
# custom ip rules for interface bond1.209
to 172.27.0.20 lookup rt-lan42 prio 101
to 172.27.0.30 lookup rt-lan42 prio 101
fwmark 0x23 lookup rt-lan42 prio 201
from 10.167.42.64/27 lookup rt-lan42 prio 301
from all to 192.168.126.73 lookup rt-lan42 prio 100
```
