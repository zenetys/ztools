# ZPKI

## Usage

```
zpki 1.0 - Benoit DOLEZ <bdolez@zenetys.com> - MIT License
Usage: zpki [options] ACTION [ACTION-PARAMETERS]
Options:
 -h, --help           This help
 -V, --version        Show version
 --x-debug            Enable bash debug mode
 -v, --verbose        Define verbose level (must be repeat)
 -q, --quiet          Set verbose level to 0
 -C, --ca             Set current CA base directory
 -y, --yes            valid all response
 -c, --cipher         Define cipher (none for no encryption)

<ACTION> is one of :
 create-cnf
     create default openssl config file
 create-key [CN|SUBJ]
     create key file
 create-csr CN|SUBJ [ALTNAMES...]
     create request
 create-self CN|SUBJ [ALTNAMES...]
     create self signed certificate
 create-ca CN|SUBJ
     create certificat authority & storage
 create-crt {CN|SUBJ} [ALTNAMES...]
     create certificate
 ca-list
     list certificates store in CA
 ca-sign-csr CN|SUBJ|CSRFILE
     sign request given in file using CA
 ca-update-crt CN|SUBJ|CRTFILE
     upate certificate
 ca-revoke-crt CN|SUBJ|CRTFILE
     revoke certificate

For SubjAltName, add address type : DNS:<FQDN>, IP:ADDR
```

## Exemple

```
$ mkdir /tmp/test-pki
$ cd /tmp/test-pki
$ zpki -y -c none create-ca "TEST CA"
: openssl genrsa -out ca.key 4096
: openssl req -batch -new -x509 -days 366 -out ca.crt -key ca.key -subj '/CN=TEST CA' -config ca.cnf -extensions ca_ext
$ zpki -y -c none create-crt "Server1"
: openssl genrsa -out private/Server1.key 4096
: openssl req -batch -new -out certs/Server1.csr -key private/Server1.key -subj /CN=Server1
: openssl ca -config ca.cnf -batch -in certs/Server1.csr -out certs/Server1.crt -days 366 -extensions x509v3_ext
$ zpki -y -c none create-crt "Server2"
: openssl genrsa -out private/Server2.key 4096
: openssl req -batch -new -out certs/Server2.csr -key private/Server2.key -subj /CN=Server2
: openssl ca -config ca.cnf -batch -in certs/Server2.csr -out certs/Server2.crt -days 366 -extensions x509v3_ext
$ zpki ca-list
[
 {"status":"V","expiration":"2022-02-06T21:06:56+0200","serial":"A638E2BB7E8C","subject":"/CN=Server1"},
 {"status":"V","expiration":"2022-02-06T21:20:08+0200","serial":"A638E2BB7E8E","subject":"/CN=Server2"}
]

