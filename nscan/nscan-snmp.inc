#
# Copyright (c) ZENETYS and contributors
# License: MIT (https://opensource.org/licenses/MIT)
# Authors: Benoit DOLEZ @ ZENETYS, Julien THOMAS @ ZENETYS
#

# nscan snmp module
# vim: set tabstop=2 shiftwidth=2:

DEFAULT_CREDENTIALS+=( '*:snmp:v2:public' )

require-cmd snmpwalk
require-cmd snmpbulkwalk
require-cmd snmpget

SNMP_TIMEOUT=2
SNMP_RETRIES=1
SNMP_ARGS=( -M '' -m '' -ObenUx --hexOutputLength=0 )

OIDS_sys=(
  ${OID_sysDescr:=.1.3.6.1.2.1.1.1.0}
  ${OID_sysObjectID:=.1.3.6.1.2.1.1.2.0}
  ${OID_sysContact:=.1.3.6.1.2.1.1.4.0}
  ${OID_sysName:=.1.3.6.1.2.1.1.5.0}
  ${OID_sysLocation:=.1.3.6.1.2.1.1.6.0}
)

OIDS_ipAdEnt=(
  ${OID_ipAdEntIfIndex:=.1.3.6.1.2.1.4.20.1.2}
  ${OID_ipAdEntNetMask:=.1.3.6.1.2.1.4.20.1.3}
)

OIDS_ipRoute=(
  ${OID_ipRouteIfIndex:=.1.3.6.1.2.1.4.21.1.2}
  ${OID_ipRouteMetric1:=.1.3.6.1.2.1.4.21.1.3}
  ${OID_ipRouteNextHop:=.1.3.6.1.2.1.4.21.1.7} # 0.0.0.0 for local
  ${OID_ipRouteMask:=.1.3.6.1.2.1.4.21.1.11}
)

OIDS_inetCidrRoute=(
  ${OID_inetCidrRouteIfIndex:=.1.3.6.1.2.1.4.24.7.1.7}
  ${OID_inetCidrRouteMetric1:=.1.3.6.1.2.1.4.24.7.1.12}
)

OIDS_arp=(
  ${OID_ipNetToMediaPhysAddress:=.1.3.6.1.2.1.4.22.1.2} # .<if>.<ip> = <mac>
)

OIDS_if=(
  ${OID_ifDescr:=.1.3.6.1.2.1.2.2.1.2}
  ${OID_ifType:=.1.3.6.1.2.1.2.2.1.3}
  ${OID_ifSpeed:=.1.3.6.1.2.1.2.2.1.5}
  ${OID_ifPhysAddress:=.1.3.6.1.2.1.2.2.1.6}
  ${OID_ifAdminStatus:=.1.3.6.1.2.1.2.2.1.7}
  ${OID_ifOperStatus:=.1.3.6.1.2.1.2.2.1.8}
)

OIDS_ifX=(
  ${OID_ifName:=.1.3.6.1.2.1.31.1.1.1.1}
  ${OID_ifHighSpeed:=.1.3.6.1.2.1.31.1.1.1.15}
  ${OID_ifAlias:=.1.3.6.1.2.1.31.1.1.1.18}
)

OIDS_dot3adAgg=(
  # bug seen on C3850-48XS stackwize IOS XE 16.06.04a, return a zero value
  ${OID_dot3adAggPortSelectedAggID:=.1.2.840.10006.300.43.1.2.1.1.12}
)

OIDS_dot3Stats=(
  ${OID_dot3StatsDuplexStatus:=.1.3.6.1.2.1.10.7.2.1.19}
)

OIDS_dot1d=(
  ${OID_dot1dBasePortIfIndex:=.1.3.6.1.2.1.17.1.4.1.2} # community@vlan on cisco except sb
  ${OID_dot1dTpFdbPort:=.1.3.6.1.2.1.17.4.3.1.2} # community@vlan on cisco except sb
)

OIDS_dot1q=(
  ${OID_dot1qVlanFdbId:=.1.3.6.1.2.1.17.7.1.4.2.1.3} # .<ignore>.<vlan> = <fdb-id> (usually equals)
  ${OID_dot1qTpFdbPort:=.1.3.6.1.2.1.17.7.1.2.2.1.2} # .<fdb-id>.<mac> = <port>
  ${OID_dot1qVlanStaticName:=.1.3.6.1.2.1.17.7.1.4.3.1.1} # .<vlan> = <name>
  ${OID_dot1qPvid:=.1.3.6.1.2.1.17.7.1.4.5.1.1} # .<port> = <pvid>
  # <ports-bits> as Hex-String
  # b16    0    0     0    0   ...
  # b2    0000 0000  0000 0000 ...
  # port  1234 5678  9         ...    element id in dot1dBasePortIfIndex list
  ${OID_dot1qVlanCurrentEgressPorts:=.1.3.6.1.2.1.17.7.1.4.2.1.4} # .<ignore>.<vlan> = <ports-bits>
  ${OID_dot1qVlanCurrentUntaggedPorts:=.1.3.6.1.2.1.17.7.1.4.2.1.5} # .<ignore>.<vlan> = <ports-bits>
)

OIDS_lldpLoc=(
  #${OID_lldpLocSysName:=.1.0.8802.1.1.2.1.3.3.0}
  #${OID_lldpLocPortIdSubtype:=.1.0.8802.1.1.2.1.3.7.1.2}
  #${OID_lldpLocPortId:=.1.0.8802.1.1.2.1.3.7.1.3}

  # Local port index mapping to local port ifDescr, OK HP, CiscoSB, Cisco
  #${OID_lldpLocPortDesc:=.1.0.8802.1.1.2.1.3.7.1.4} # .<locPortIndex> = <ifDescr>

  ${OID_lldpLocalSystemData:=.1.0.8802.1.1.2.1.3}
)

OIDS_lldpRem=(
  # if lldpRemChassisIdSubtype == 7 (local) then take lldpRemChassisId
  # else take LLDP-MIB::lldpRemSysName (may be empty)
  #${OID_lldpRemChassisIdSubtype:=.1.0.8802.1.1.2.1.4.1.1.4} # <0?>.<locPortIndex>.<inc?> = <(rem)dataType>
  #${OID_lldpRemChassisId:=.1.0.8802.1.1.2.1.4.1.1.5} # <0?>.<locPortIndex>.<inc?> = <(rem)dataValue>
  #${OID_lldpRemSysName:=.1.0.8802.1.1.2.1.4.1.1.9} # <0?>.<locPortIndex>.<inc?> = <(rem)name>

  # Use remote sys description to mimic CDP remote platform.
  # May be very long (like standard sysDescr) on some remote devices...
  #${OID_lldpRemSysDesc:=.1.0.8802.1.1.2.1.4.1.1.10} # <0?>.<locPortIndex>.<inc?> = <(rem)desc>

  #${OID_lldpRemManAddrOID:=.1.0.8802.1.1.2.1.4.2.1.4} # <0?>.<locPortIndex>.<inc?>.?.4.<ipv4> = <dummy>

  # Interesting lldpRemPortIdSubtype values: interfaceAlias(1), interfaceName(5)
  #${OID_lldpRemPortIdSubtype:=.1.0.8802.1.1.2.1.4.1.1.6} # <0?>.<locPortIndex>.<inc?> = <(rem)dataType>
  #${OID_lldpRemPortId:=.1.0.8802.1.1.2.1.4.1.1.7} # <0?>.<locPortIndex>.<inc?> = <(rem)dataValue>

  ${OID_lldpRemoteSystemsData:=.1.0.8802.1.1.2.1.4}
)

# From RMON2-MIB to get default gateway on Cisco IOS L2
: ${OID_netDefaultGateway:=.1.3.6.1.2.1.16.19.12.0}

OIDS_vtp=(
  #${OID_vtpDomainName:=.1.3.6.1.4.1.9.9.46.1.2.1.1.2} # .<domain> = <name>
  #${OID_vtpDomainLocalMode:=.1.3.6.1.4.1.9.9.46.1.2.1.1.3} # eg. transparent(3)
  #${OID_vtpDomainRevNumber:=.1.3.6.1.4.1.9.9.46.1.2.1.1.4}
  ${OID_vtpDomain:=.1.3.6.1.4.1.9.9.46.1.2.1.1}
  ${OID_vtpVlanName:=.1.3.6.1.4.1.9.9.46.1.3.1.1.4} # .<domain>.<vlan> = <name>
)

OIDS_cdpGlobal=(
  #${OID_cdpGlobalDeviceId:=.1.3.6.1.4.1.9.9.23.1.3.4.0}
  ${OID_cdpGlobal:=.1.3.6.1.4.1.9.9.23.1.3}
)

OIDS_cdpCache=(
  #${OID_cdpCacheAddressType:=.1.3.6.1.4.1.9.9.23.1.2.1.1.3}
  #${OID_cdpCacheAddress:=.1.3.6.1.4.1.9.9.23.1.2.1.1.4}
  #${OID_cdpCacheDeviceId:=.1.3.6.1.4.1.9.9.23.1.2.1.1.6}
  #${OID_cdpCacheDevicePort:=.1.3.6.1.4.1.9.9.23.1.2.1.1.7}
  #${OID_cdpCachePlatform:=.1.3.6.1.4.1.9.9.23.1.2.1.1.8}
  #${OID_cdpCacheCapabilities:=.1.3.6.1.4.1.9.9.23.1.2.1.1.9}
  #${OID_cdpCacheNativeVLAN:=.1.3.6.1.4.1.9.9.23.1.2.1.1.11}
  #${OID_cdpCachePowerConsumption:=.1.3.6.1.4.1.9.9.23.1.2.1.1.15}
  ${OID_cdpCache:=.1.3.6.1.4.1.9.9.23.1.2.1.1}
)

OIDS_vlanTrunkPort=(
  ${OID_vlanTrunkPortVlansEnabled:=.1.3.6.1.4.1.9.9.46.1.6.1.1.4}
  ${OID_vlanTrunkPortNativeVlan:=.1.3.6.1.4.1.9.9.46.1.6.1.1.5} # native vlan
  ${OID_vlanTrunkPortDynamicState:=.1.3.6.1.4.1.9.9.46.1.6.1.1.13} # administrative state
  ${OID_vlanTrunkPortDynamicStatus:=.1.3.6.1.4.1.9.9.46.1.6.1.1.14} # buggy for members of port channels
  ${OID_vlanTrunkPortVlansEnabled2k:=.1.3.6.1.4.1.9.9.46.1.6.1.1.17}
  ${OID_vlanTrunkPortVlansEnabled3k:=.1.3.6.1.4.1.9.9.46.1.6.1.1.18}
  ${OID_vlanTrunkPortVlansEnabled4k:=.1.3.6.1.4.1.9.9.46.1.6.1.1.19}
)

OIDS_vmTable=(
  ${OID_vmVlan:=.1.3.6.1.4.1.9.9.68.1.2.2.1.2} # access vlan
)

OIDS_vmTable_extended=(
  # vmVlanType and vmVlans can be used to retrieve vlan membership information
  # for Nexus FEX (vlanTrunkPort returns nothing for FEX interfaces).
  # - 1(static) or 2(dynamic), single vlan, assume access: use vmVlan
  # - 3(multiVlan), multiple vlans, assume trunk: use vmVlans* however
  #   could not find a way to find out which is native vlan.
  ${OID_vmVlanType:=.1.3.6.1.4.1.9.9.68.1.2.2.1.1}
  ${OID_vmVlans:=.1.3.6.1.4.1.9.9.68.1.2.2.1.4}
  ${OID_vmVlans2k:=.1.3.6.1.4.1.9.9.68.1.2.2.1.5}
  ${OID_vmVlans3k:=.1.3.6.1.4.1.9.9.68.1.2.2.1.6}
  ${OID_vmVlans4k:=.1.3.6.1.4.1.9.9.68.1.2.2.1.7}
)

OIDS_pagpGroup=(
  ${OID_pagpGroupIfIndex:=.1.3.6.1.4.1.9.9.98.1.1.1.1.8} # .ifIndex = ifIndex
)

OIDS_stackPort=(
  ${OID_stackPortIfIndex:=.1.3.6.1.4.1.9.5.1.4.1.1.11}
  ${OID_vlanPortIslOperStatus:=.1.3.6.1.4.1.9.5.1.9.3.1.8} # prefer to vlanTrunkPortDynamicStatus if available
)

OIDS_ciscosbVlan=(
  ${OID_ciscosbVlanPortModeState:=.1.3.6.1.4.1.9.6.1.101.48.22.1.1}
  ${OID_ciscosbVlanTrunkPortModeNativeVlanId:=.1.3.6.1.4.1.9.6.1.101.48.61.1.1}
  ${OID_ciscosbVlanAccessPortModeVlanId:=.1.3.6.1.4.1.9.6.1.101.48.62.1.1}
  ${OID_ciscosbVlanCustomerPortModeVlanId:=.1.3.6.1.4.1.9.6.1.101.48.63.1.1}
)

OIDS_hwTrunk=(
  ${OID_hwTrunkIfIndex:=.1.3.6.1.4.1.2011.5.25.41.1.3.3.1.4} # .<aggId> = ifIndex
  ${OID_hwTrunkValidEntry:=.1.3.6.1.4.1.2011.5.25.41.1.4.1.1.2} # .<aggId>.<memberIfIndex>
)

OIDS_cefex=(
  ${OID_cefexBindingExtenderIndex:=.1.3.6.1.4.1.9.9.691.1.1.1.1.2} # .<ifIndex> = <fexId>
  ${OID_cefexConfigExtenderName:=.1.3.6.1.4.1.9.9.691.1.1.2.1.1} # .<fexId>.<fexName>
)

# Take snmpcmd output as input, return success if at least one oid value
# is found. Errors like "No Such..." are redirected to stderr.
function validate-snmp-data() {
  awk '/^End of MIB|\
No Such Instance currently exists at this OID|\
No Such Object available on this agent at this OID|\
No more variables left in this MIB View/ { print $0 >> "/dev/stderr"; next; }
{ ok=1; print $0; }
END { exit(ok?0:1); }'
}

# $1: oid to find
function find-oid-val() {
  sed -nre "/^${1//./\\.} = /{s,[^:]+: (.+),\1,p;q}"
}

# $1: oid prefix
# $2: pattern to match and capture after oid prefix
# $3: replacement definition, eg: \1 for first capture
function extract-snmp-data() {
  sed -nre "s,^${1//./\\.}${2},${3},p"
}

# Usage: _snmpcmd [-X] [-V vlan] get|walk ip oid... [opts...]
# Option -X masks the memory
function _snmpcmd() {
  local IFS=$MYIFS
  local out=$(mktemp) || return 1
  [[ $1 == -X ]] && { declare -A MEMORY_VAL=(); shift; } # mask memory
  local subcred=default vlan=;  [[ $1 == -V ]] && { subcred=vlan; vlan=$2; shift 2; }
  local cmd=$1; shift # get or walk
  local ip=$1; shift
  local opts=( "${SNMP_ARGS[@]}" )
  [[ $cmd == walk ]] && cmd="${MEMORY_VAL[credential.snmp.$ip.bulk]}${cmd}" # bulk only for walk
  local oids=(); while [[ -n $1 && $1 != -* ]]; do oids+=("$1"); shift; done
  opts+=( ${MEMORY_VAL[credential.snmp.$ip.$subcred]//%vlan%/$vlan} "$@" )
  debug-exec -v 2 "snmp$cmd" "${opts[@]}" "$ip" "${oids[@]}" > "$out" || { rm "$out"; return 1; }
  validate-snmp-data < "$out" || { rm "$out"; return 1; }
  rm "$out"
  return 0
}

# Normalizer function for short form of snmp credentials
# Supported arguments:
# - v1 community [stop]
# - v2 community [stop]
# - v3 [stop]
# - v3 secName authProtocol authPassword [stop]
# - v3 secName authProtocol authPassword privProtocol privPassword [stop]
# $@: spec parts
# Reply result as kv pairs in AREPLY array
function credential-snmp-norm() {
  if [[ $1 == v1 ]] && (( $# >= 2 && $# <= 3 )); then
    AREPLY[version]=1
    AREPLY[community]=$2
    [[ $3 == stop ]] && AREPLY[stop]=1
    return 0
  fi
  if [[ $1 == v2 ]] && (( $# >= 2 && $# <= 3 )); then
    AREPLY[version]=2c
    AREPLY[community]=$2
    [[ $3 == stop ]] && AREPLY[stop]=1
    return 0
  fi
  if [[ $1 == v3 ]]; then
    AREPLY[version]=3
    if (( $# >= 1 && $# <= 2 )); then
      AREPLY[secLevel]=noAuthNoPriv
      [[ $2 == stop ]] && AREPLY[stop]=1
      return 0
    fi
    if (( $# >= 4 && $# <= 5 )); then
      AREPLY[secLevel]=authNoPriv
      AREPLY[secName]=$2
      AREPLY[authProtocol]=$3
      AREPLY[authPassword]=$4
      [[ $5 == stop ]] && AREPLY[stop]=1
      return 0
    fi
    if (( $# >= 6 && $# <= 7 )); then
      AREPLY[secLevel]=authPriv
      AREPLY[secName]=$2
      AREPLY[authProtocol]=$3
      AREPLY[authPassword]=$4
      AREPLY[privProtocol]=$5
      AREPLY[privPassword]=$6
      [[ $7 == stop ]] && AREPLY[stop]=1
      return 0
    fi
  fi
  return 1
}

# Checker function for snmp credentials. When this function gets called,
# KV properties of the credential to check are available in AREPLY.
function credential-snmp-check() {
  local ip=$1; shift
  local creds bulk i is_v3= has_context=
  case "${AREPLY[version]}:${AREPLY[bulk]}" in
    *:1) bulk=bulk ;;
    2*:) bulk=bulk ;;
    3*:) bulk=bulk ;;
    *) bulk= ;;
  esac
  creds=(
    # allow per target timeout or retries override
    -t "${AREPLY[timeout]:-$SNMP_TIMEOUT}"
    -r "${AREPLY[retries]:-$SNMP_RETRIES}"
    # real credential stuff
    ${AREPLY[version]:+-v "${AREPLY[version]}"}
    ${AREPLY[community]:+-c "${AREPLY[community]}"}
    ${AREPLY[secLevel]:+-l "${AREPLY[secLevel]}"}
    ${AREPLY[contextName]:+-n "${AREPLY[contextName]}"}
    ${AREPLY[authProtocol]:+-a "${AREPLY[authProtocol]}"}
    ${AREPLY[authPassword]:+-A "${AREPLY[authPassword]}"}
    ${AREPLY[secName]:+-u "${AREPLY[secName]}"}
    ${AREPLY[privProtocol]:+-x "${AREPLY[privProtocol]}"}
    ${AREPLY[privPassword]:+-X "${AREPLY[privPassword]}"}
  )
  # use -X to disable memory for this _snmpcmd call
  if _snmpcmd -X "${bulk}walk" "$ip" "$OID_sysDescr" "${creds[@]}" >/dev/null; then
    learn credential "snmp.$ip.default" "${creds[@]}"
    learn credential "snmp.$ip.bulk" "$bulk"
    # prepare for Cisco IOS per vlan FDB
    for (( i=0; i < ${#creds[@]}; i++ )); do
      case "${creds[i]}" in
        -v) [[ ${creds[i+1]} == 3 ]] && is_v3=1 ;;
        -c) creds[i+1]="${creds[i+1]}@%vlan%" ;;
        -n) creds[i+1]='vlan-%vlan%'; has_context=1 ;;
      esac
    done
    [[ -n $is_v3 && -z $has_context ]] && creds+=( -n 'vlan-%vlan%' )
    learn credential "snmp.$ip.vlan" "${creds[@]}"
    return 0
  fi
  return 1
}

# cb-snmp learn-args... [-V vlan] get|walk oids...
# Option -V vlan for Cisco IOS FDB (vlan community string indexing or context)
function cb-snmp() {
  local table=$1; shift # ignored
  local ip=$1; shift
  local vlan=; [[ $1 == -V ]] && { vlan=$2; shift 2; }
  local cmd=$1; shift
  local oids=( "$@" )

  declare o ret=1
  case "$cmd" in
    # return success if at least one call succeeds
    *walk) for o in "${oids[@]}"; do _snmpcmd ${vlan:+-V "$vlan"} "$cmd" "$ip" "$o" && ret=0; done ;;
    *) _snmpcmd ${vlan:+-V "$vlan"} "$cmd" "$ip" "${oids[@]}" && ret=0 ;;
  esac
  return $ret
}

# $@: cb-snmp arguments
function parse-cb-snmp-sys() {
  local table=$1; shift
  local ip=$1; shift
  local sysoid
  sysoid=$(echo "$REPLY" |find-oid-val "$OID_sysObjectID")
  case "$sysoid" in
    # branch on specific devices
    .1.3.6.1.4.1.9.6.1.*) # cisco smb
      learn snmp-std "$ip"
      learn snmp-cisco-smb "$ip"
      learn snmp-cisco-cdp "$ip"
      ;;
    .1.3.6.1.4.1.9.12.3.1.3.*) # target cisco nx but oid is for chassis
      learn snmp-cisco "$ip"
      learn snmp-cisco-cdp "$ip"
      learn snmp-cisco-nexus "$ip"
      ;;
    .1.3.6.1.4.1.9.*) # cisco
      learn snmp-cisco "$ip"
      learn snmp-cisco-cdp "$ip"
      ;;
    .1.3.6.1.4.1.2011.2.239.*) # huawei dcswitch
      learn snmp-std "$ip"
      learn snmp-huawei "$ip"
      ;;
    *) # default
      learn snmp-std "$ip"
      ;;
  esac
}

# $@ part 1: cb-snmp arguments
# $@ part 2: extract-snmp-data arguments
function parse-cb-snmp-learn-ipv4() {
  shift 2; # ignore table, ip
  while read -r; do
    [[ $REPLY == 0.0.0.0 ]] && continue
    [[ $REPLY == 127.* ]] && continue
    [[ $REPLY == 255.255.255.255 ]] && continue
    learn ipv4 "$REPLY"
  done < <(echo "$REPLY" |extract-snmp-data "$@" |sort -uV)
}

# $@: cb-snmp arguments
function parse-cb-snmp-vtp() {
  local table=$1; shift
  local ip=$1; shift
  while read -r; do
    if (( REPLY < 1002 || REPLY > 1005 )); then
      learn snmp-cisco-vlan "${ip}${MYIFS}${REPLY}"
    fi
  done < <(echo "$REPLY" |
    extract-snmp-data "$OID_vtpVlanName" '\.1\.([0-9]+) = .*' '\1' |
    sort -un)
}

function cb-snmp-cisco-fdb() {
  local table=$1; shift # ignored
  local ip=$1; shift
  local vlan=$1; shift
  cb-snmp "$table" "$ip" -V "$vlan" walk "${OIDS_dot1d[@]}"
}

function parse-cb-self-ip() {
  local table=$1; shift # ignored
  local ip=$1; shift
  while read -r; do
    [[ $REPLY == 0.0.0.0 ]] && continue
    [[ $REPLY == 127.* ]] && continue
    [[ $REPLY == 255.255.255.255 ]] && continue
    [[ $REPLY == $ip ]] && continue
    learned ipv4 "$REPLY" from device "$ip"
  done < <(echo "$REPLY" |
    extract-snmp-data "$OID_ipAdEntIfIndex" '\.([^ ]+) .*' '\1' |
    sort -uV)
}

pqueue-register device \
  cb-find-credential snmp

pqueue-register snmp -n 'x-snmp-sys' \
  cb-snmp get "${OIDS_sys[@]}" --then-call \
    parse-cb-snmp-sys

pqueue-register snmp -n 'x-snmp-ipAdEnt' \
  cb-snmp walk "${OIDS_ipAdEnt[@]}" --then-call \
    parse-cb-self-ip

pqueue-register snmp -n 'x-snmp-arp' \
  cb-snmp walk "${OIDS_arp[@]}" --then-call \
    parse-cb-snmp-learn-ipv4 \
      "$OID_ipNetToMediaPhysAddress" \
      '\.[0-9]+\.([^ ]+) .*' \
      '\1'

pqueue-register snmp -n 'x-snmp-ipRoute' \
  cb-snmp walk "${OIDS_ipRoute[@]}" --then-call \
    parse-cb-snmp-learn-ipv4 \
      "$OID_ipRouteNextHop" \
      '\.[^ ]+ = IpAddress: ' \
      ''

pqueue-register snmp -n 'x-snmp-inetCidrRoute' \
  cb-snmp walk "${OIDS_inetCidrRoute[@]}" --then-call \
    parse-cb-snmp-learn-ipv4 \
      "$OID_inetCidrRouteIfIndex" \
      '\.1\.4\..+\.1\.4\.([^ ]+) .*' \
      '\1'

pqueue-register snmp -n 'x-snmp-if' \
  cb-snmp walk "${OIDS_if[@]}"

pqueue-register snmp -n 'x-snmp-ifX' \
  cb-snmp walk "${OIDS_ifX[@]}"

pqueue-register snmp -n 'x-snmp-dot3adAgg' \
  cb-snmp walk "${OIDS_dot3adAgg[@]}"

pqueue-register snmp -n 'x-snmp-dot3Stats' \
  cb-snmp walk "${OIDS_dot3Stats[@]}"

pqueue-register snmp -n 'x-snmp-lldpLoc' \
  cb-snmp walk "${OIDS_lldpLoc[@]}"

pqueue-register snmp -n 'x-snmp-lldpRem' \
  cb-snmp walk "${OIDS_lldpRem[@]}"

pqueue-register snmp-std -n 'x-snmp-dot1d' \
  cb-snmp walk "${OIDS_dot1d[@]}"

pqueue-register snmp-std -n 'x-snmp-dot1q' \
  cb-snmp walk "${OIDS_dot1q[@]}"

pqueue-register snmp-cisco -n 'x-snmp-netDefaultGateway' \
  cb-snmp get "$OID_netDefaultGateway" --then-call \
    parse-cb-snmp-learn-ipv4 \
      "$OID_netDefaultGateway" \
      ' = IpAddress: ' \
      ''

pqueue-register snmp-cisco -n 'x-snmp-vtp' \
  cb-snmp walk "${OIDS_vtp[@]}" --then-call \
    parse-cb-snmp-vtp

pqueue-register snmp-cisco -n 'x-snmp-vlanTrunkPort' \
  cb-snmp walk "${OIDS_vlanTrunkPort[@]}"

pqueue-register snmp-cisco -n 'x-snmp-vm' \
  cb-snmp walk "${OIDS_vmTable[@]}"

pqueue-register snmp-cisco -n 'x-snmp-pagpGroup' \
  cb-snmp walk "${OIDS_pagpGroup[@]}"

pqueue-register snmp-cisco -n 'x-snmp-stackPort' \
  cb-snmp walk "${OIDS_stackPort[@]}"

pqueue-register snmp-cisco-vlan \
  cb-snmp-cisco-fdb

pqueue-register snmp-cisco-smb -n 'x-snmp-cisco-smb-vlan' \
  cb-snmp walk "${OIDS_ciscosbVlan[@]}"

pqueue-register snmp-cisco-cdp -n 'x-snmp-cdpGlobal' \
  cb-snmp walk "${OIDS_cdpGlobal[@]}"

pqueue-register snmp-cisco-cdp -n 'x-snmp-cdpCache' \
  cb-snmp walk "${OIDS_cdpCache[@]}"

pqueue-register snmp-cisco-nexus -n 'x-snmp-vm' \
  cb-snmp walk "${OIDS_vmTable_extended[@]}"

pqueue-register snmp-cisco-nexus -n 'x-snmp-cefex' \
  cb-snmp walk "${OIDS_cefex[@]}"

pqueue-register snmp-huawei -n 'x-snmp-hwTrunk' \
  cb-snmp walk "${OIDS_hwTrunk[@]}"
