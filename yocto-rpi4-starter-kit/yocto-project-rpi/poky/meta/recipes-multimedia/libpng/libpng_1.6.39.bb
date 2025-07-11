SUMMARY = "PNG image format decoding library"
DESCRIPTION = "An open source project to develop and maintain the reference \
library for use in applications that read, create, and manipulate PNG \
(Portable Network Graphics) raster image files. "
HOMEPAGE = "http://www.libpng.org/"
SECTION = "libs"
LICENSE = "Libpng"
LIC_FILES_CHKSUM = "file://LICENSE;md5=5c900cc124ba35a274073b5de7639b13"
DEPENDS = "zlib"

LIBV = "16"

SRC_URI = "\
           ${SOURCEFORGE_MIRROR}/${BPN}/${BPN}${LIBV}/${BP}.tar.xz \
           file://run-ptest \
"

SRC_URI[sha256sum] = "1f4696ce70b4ee5f85f1e1623dc1229b210029fa4b7aee573df3e2ba7b036937"

MIRRORS += "${SOURCEFORGE_MIRROR}/${BPN}/${BPN}${LIBV}/ ${SOURCEFORGE_MIRROR}/${BPN}/${BPN}${LIBV}/older-releases/"

UPSTREAM_CHECK_URI = "http://libpng.org/pub/png/libpng.html"

BINCONFIG = "${bindir}/libpng-config ${bindir}/libpng16-config"

inherit autotools binconfig-disabled pkgconfig ptest

# Work around missing symbols
EXTRA_OECONF:append:class-target = " ${@bb.utils.contains("TUNE_FEATURES", "neon", "--enable-arm-neon=on", "--enable-arm-neon=off", d)}"

PACKAGES =+ "${PN}-tools"

FILES:${PN}-tools = "${bindir}/png-fix-itxt ${bindir}/pngfix ${bindir}/pngcp"

# CVE-2019-17371 is actually a memory leak in gif2png 2.x
CVE_CHECK_IGNORE += "CVE-2019-17371"

RDEPENDS:${PN}-ptest += "make bash gawk"

do_install_ptest() {
    # Install test scripts to ptest path
    install -d ${D}${PTEST_PATH}/src/tests
    install -m 755 ${S}/tests/* ${D}${PTEST_PATH}/src/tests
    install -m 755 ${S}/test-driver ${D}${PTEST_PATH}/src
    install -d ${D}${PTEST_PATH}/src/tests/scripts
    install -m 755 ${S}/scripts/*.awk ${D}${PTEST_PATH}/src/tests/scripts
    install -m 644 ${S}/scripts/pnglib* ${S}/scripts/*.c ${S}/scripts/*.def ${S}/scripts/macro.lst ${D}${PTEST_PATH}/src/tests/scripts
    install -m 644 ${S}/scripts/pnglibconf.h.prebuilt ${D}${PTEST_PATH}/src/tests/scripts/pnglibconf.h
    install -d ${D}${PTEST_PATH}/src/contrib/tools
    install -m 755 ${S}/contrib/tools/*.sh ${D}${PTEST_PATH}/src/contrib/tools
    install -m 644 ${S}/contrib/tools/*.c ${S}/contrib/tools/*.h ${D}${PTEST_PATH}/src/contrib/tools

    # Install .libs directory binaries to ptest path
    install -m 755 ${B}/.libs/pngtest ${B}/.libs/pngstest ${B}/.libs/pngimage ${B}/.libs/pngunknown ${B}/.libs/pngvalid ${D}${PTEST_PATH}/src

    # Copy png files to ptest path
    cd ${S} && find contrib -name '*.png' | cpio -pd ${D}${PTEST_PATH}/src

    # Install Makefile and png files
    install -m 644 ${S}/pngtest.png ${D}${PTEST_PATH}/src
    install -m 644 ${S}/*.png ${S}/*.h ${S}/*.c ${S}/*.dfa ${B}/pnglibconf.out ${S}/Makefile.am ${S}/Makefile.in ${D}${PTEST_PATH}/src/tests

    sed -e 's/^abs_srcdir = ..*/abs_srcdir = \.\./'          \
        -e 's/^top_srcdir = ..*/top_srcdir = \.\./'          \
        -e 's/^srcdir = ..*/srcdir = \./'                    \
        -e 's/^Makefile: ..*/Makefile: /'                    \
        -e 's/check-TESTS: $(check_PROGRAMS)/check-TESTS:/g' \
        ${B}/Makefile > ${D}${PTEST_PATH}/src/Makefile

    sed -e 's|#!/bin/awk|#!/usr/bin/awk|g' -i ${D}${PTEST_PATH}/src/tests/scripts/*.awk
}

BBCLASSEXTEND = "native nativesdk"
