SUMMARY = "BGP/OSPF/RIP routing daemon"
DESCRIPTION = "FRRouting is a free and open source Internet routing protocol suite for Linux \
and Unix platforms. It implements BGP, OSPF, RIP, IS-IS, PIM, LDP, BFD, Babel, PBR, OpenFabric \
and VRRP, with alpha support for EIGRP and NHRP."
HOMEPAGE = "https://frrouting.org/"
SECTION = "net"

LICENSE = "GPL-2.0-only & LGPL-2.1-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263 \
                    file://COPYING-LGPLv2.1;md5=4fbd65380cdd255951079008b364516c"

SRC_URI = "git://github.com/FRRouting/frr.git;protocol=https;branch=stable/8.2 \
           file://CVE-2022-37035.patch \
           file://CVE-2022-37032.patch \
           file://CVE-2022-42917.patch \
           file://CVE-2022-36440.patch \
           file://CVE-2022-40318.patch \
           file://CVE-2022-43681.patch \
           file://CVE-2023-31489.patch \
           file://CVE-2023-31490.patch \
           file://CVE-2023-38802.patch \
           file://CVE-2023-41358.patch \
           file://CVE-2023-41909.patch \
           file://CVE-2023-38406.patch \
           file://CVE-2023-38407.patch \
           file://CVE-2023-46752.patch \
           file://CVE-2023-46753.patch \
           file://CVE-2023-47234.patch \
           file://CVE-2023-47235.patch \
           file://frr.pam \
           file://CVE-2024-44070.patch \
           file://CVE-2024-27913.patch \
           file://CVE-2024-34088.patch \
           file://CVE-2024-31950.patch \
           file://CVE-2024-31951.patch \
           file://CVE-2024-31948.patch \
           file://CVE-2024-55553.patch \
	      "

SRCREV = "79188bf710e92acf42fb5b9b0a2e9593a5ee9b05"

S = "${WORKDIR}/git"

# Due to libyang not supported on these arches:
COMPATIBLE_HOST:riscv32 = "null"
COMPATIBLE_HOST:riscv64 = "null"
COMPATIBLE_HOST:armv5 = "null"

# Fail to build on mips64 with error:
# Error: PC-relative reference to a different section
COMPATIBLE_HOST:mips64 = "null"

inherit autotools-brokensep python3native pkgconfig useradd systemd

DEPENDS:class-native = "bison-native elfutils-native"
DEPENDS:class-target = "bison-native json-c readline c-ares libyang frr-native"

RDEPENDS:${PN}:class-target = "iproute2 python3-core bash"

PACKAGECONFIG ??= "${@bb.utils.filter('DISTRO_FEATURES', 'pam', d)}"
PACKAGECONFIG:class-native = ""

PACKAGECONFIG[pam] = "--with-libpam,--without-libpam,libpam"
PACKAGECONFIG[grpc] = "--enable-grpc,--disable-grpc,grpc-native grpc"
PACKAGECONFIG[snmp] = "--enable-snmp,--disable-snmp,net-snmp"
PACKAGECONFIG[zeromq] = "--enable-zeromq,--disable-zeromq,zeromq"
PACKAGECONFIG[protobuf] = "--enable-protobuf,--disable-protobuf,protobuf-c-native protobuf-c"
PACKAGECONFIG[capabilities] = "--enable-capabilities,--disable-capabilities,libcap"
PACKAGECONFIG[cumulus] = "--enable-cumulus,--disable-cumulus"
PACKAGECONFIG[datacenter] = "--enable-datacenter,--disable-datacenter"
PACKAGECONFIG[ospfclient] = "--enable-ospfapi --enable-ospfclient,--disable-ospfapi --disable-ospfclient"

EXTRA_OECONF:class-native = "--enable-clippy-only"

EXTRA_OECONF:class-target = "--sbindir=${libdir}/frr \
                             --sysconfdir=${sysconfdir}/frr \
                             --localstatedir=${localstatedir}/run/frr \
                             --enable-vtysh \
                             --enable-fpm \
                             --enable-multipath=64 \
                             --enable-user=frr \
                             --enable-group=frr \
                             --enable-vty-group=frrvty \
                             --enable-configfile-mask=0640 \
                             --enable-logfile-mask=0640 \
                             --disable-doc \
                             --with-clippy=${RECIPE_SYSROOT_NATIVE}/usr/lib/clippy \
                            "

CACHED_CONFIGUREVARS += "ac_cv_path_PERL='/usr/bin/env perl'"

LDFLAGS:append:mips = " -latomic"
LDFLAGS:append:mipsel = " -latomic"
LDFLAGS:append:powerpc = " -latomic"

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE:${PN} = "frr.service"
SYSTEMD_AUTO_ENABLE = "disable"

do_compile:prepend () {
   sed -i -e 's#${RECIPE_SYSROOT_NATIVE}##g' \
          -e 's#${RECIPE_SYSROOT}##g' ${S}/lib/version.h
}

do_compile:class-native () {
    oe_runmake clippy-only
}

do_install:class-native () {
    install -d ${D}${libdir}
    install -m 755 ${S}/lib/clippy ${D}${libdir}/clippy
}

do_install:append:class-target () {
    install -m 0755 -d ${D}${sysconfdir}/frr
    install -m 0640 ${S}/tools/etc/frr/* ${D}${sysconfdir}/frr/
    chown frr:frrvty ${D}${sysconfdir}/frr
    chown frr:frr ${D}${sysconfdir}/frr/*
    chown frr:frrvty ${D}${sysconfdir}/frr/vtysh.conf
    chmod 640 ${D}${sysconfdir}/frr/*

    if ${@bb.utils.contains('DISTRO_FEATURES', 'pam', 'true', 'false', d)}; then
        install -d ${D}/${sysconfdir}/pam.d
        install -m 644 ${WORKDIR}/frr.pam ${D}/${sysconfdir}/pam.d/frr
    fi

    if ${@bb.utils.contains('DISTRO_FEATURES', 'sysvinit', 'true', 'false', d)}; then
        install -d ${D}${sysconfdir}/init.d
        install -m 0755 ${B}/tools/frrinit.sh ${D}${sysconfdir}/init.d/frr

        install -d ${D}${sysconfdir}/default/volatiles
        echo "d frr frr 0755 ${localstatedir}/run/frr none" \
            > ${D}${sysconfdir}/default/volatiles/99_frr
    fi

    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -d ${D}${systemd_system_unitdir}
        install -m 0644 ${B}/tools/frr*.service ${D}${systemd_system_unitdir}

        install -d ${D}${sysconfdir}/tmpfiles.d
        echo "d /run/frr 0755 frr frr -" \
            > ${D}${sysconfdir}/tmpfiles.d/${BPN}.conf
    fi
}

USERADD_PACKAGES = "${PN}"
GROUPADD_PARAM:${PN} = "--system frr ; --system frrvty"
USERADD_PARAM:${PN} = "--system --home ${localstatedir}/run/frr/ -M -g frr -G frrvty --shell /bin/false frr"

FILES:${PN} += "${datadir}/yang"

BBCLASSEXTEND = "native"
