# Squid external ACL from UT1 categories

UT1 lists are stored in an SQLite database, maintained by script `ut1-to-sqlite` via cron task or systemd timer.  
The external ACL script `zsquid-sqlite-acl` is written in LUA and requires module `lsqlite3`.

## Sqlite3 and LUA install

**Debian 13** trixie, release assumed between june / early september 2025 (current testing)

```txt
# apt install sqlite3 lua5.4 lua-lsqlite3
```

**Debian 12** bookworm

Install package `lua-lsqlite3` from testing.  
Here we retrieve trixie .deb from [packages.debian.org](https://packages.debian.org/trixie/lua-lsqlite3).

```txt
# apt install sqlite3 lua5.4
# curl -OL http://ftp.fr.debian.org/debian/pool/main/l/lua-lsqlite3/lua-lsqlite3_0.9.6-1+b1_amd64.deb
# dpkg -i lua-lsqlite3_0.9.6-1+b1_amd64.deb
```

**RedHat 9**

LUA module `lsqlite3` is available in [zenetys-latest](https://packages.zenetys.com/latest/redhat/zenetys-latest.repo) repository.

```txt
# (cd /etc/yum.repos.d && curl -OL https://packages.zenetys.com/latest/redhat/zenetys-latest.repo)
# dnf install sqlite lua lua54z-lsqlite3
```

## Scripts install

Retrieve and copy scripts in `/usr/local/bin` :

```txt
# cd /usr/local/bin
# curl -OL https://raw.githubusercontent.com/zenetys/ztools/refs/heads/master/squid/ut1-to-sqlite
# curl -OL https://raw.githubusercontent.com/zenetys/ztools/refs/heads/master/squid/zsquid-sqlite-acl
# chmod 755 ut1-to-sqlite zsquid-sqlite-acl
```

## UT1 database deployment

Generate the UT1 database in `/var/lib/ut1-to-sqlite/cache/ut1.db` :

```
# mkdir /var/lib/ut1-to-sqlite
# VARDIR=/var/lib/ut1-to-sqlite ut1-to-sqlite -i type:key
```

## Squid configuration

Example to define an ACL `blacklist-categories-dstdom`. This query returns one special colummn `matched=0|1` corresponding to the result of the ACL and one column `category` providing the  list of categories found for the domain tested, whether or not the ACL succeeds.

<span style="color: rgb(186, 55, 42);">Carefully copy/paste as-is to keep (non-)spaces</span> between categories, before newlines backslashes, quotes, etc.

```txt
external_acl_type zsquid_ut1 \
    ttl=3600 negative_ttl=3600 cache=262144 \
    children-max=100 children-startup=5 children-idle=3 \
    %>rd %DATA \
    /usr/local/bin/zsquid-sqlite-acl \
        --database /var/lib/ut1-to-sqlite/cache/ut1.db \
        --verbose 1 \
        --query "\
select \
    group_concat(f.category) as category, \
    max(iif(f.category in (\
        'adult','agressif','arjel','cryptojacking','dangerous_material',\
        'ddos','doh','drogue','gambling','hacking','malware','phishing',\
        'redirector','residential-proxies','sect','stalkerware','vpn','warez'\
    ),1,0)) as matched \
from furl f \
where f.type='domain' \
and %{domain_in(ctx.arg[1],'key')} \
group by null \
        "

acl blacklist-categories-dstdom external zsquid_ut1
```

**Logging**

Columns other than `matched` are returned as squid K=V *keywords* and can be displayed in logs by using *formatcode* "note". Use for instance `%{category}note` to get the value of column `category` :

```txt
logformat my_squid %ts.%03tu %6tr %>a %Ss/%03>Hs %<st %rm %ru %[un %Sh/%<a %mt category=%{category}note
access_log daemon:/var/log/squid/access.log my_squid
```

**SQL queries**

The external ACL can also be used with a query that does not return a special column `matched`. In such case, the result of the ACL is true (OK) if the query returns at least 1 line, false (ERR) otherwise. Note: the query is supposed to return maximum 1 line, others are ignored.

## UT1 database update

<span style="color: rgb(186, 55, 42);">Please adjust path or `$PATH` depending on context.</span>

**Cron task**

```txt
# curl -L -o /etc/cron.d/ut1-to-sqlite \
    https://raw.githubusercontent.com/zenetys/ztools/refs/heads/master/squid/ut1-to-sqlite.cron
```

**Systemd timer**

```txt
# cd /etc/systemd/system
# curl -OL https://raw.githubusercontent.com/zenetys/ztools/refs/heads/master/squid/ut1-to-sqlite.service
# curl -OL https://raw.githubusercontent.com/zenetys/ztools/refs/heads/master/squid/ut1-to-sqlite.timer
# systemctl daemon-reload
# systemctl enable ut1-to-sqlite.timer
# systemctl start ut1-to-sqlite.timer
# systemctl list-timers
```
