SUMMARY = "Fast open source processor emulator"
DESCRIPTION = "QEMU is a hosted virtual machine monitor: it emulates the \
machine's processor through dynamic binary translation and provides a set \
of different hardware and device models for the machine, enabling it to run \
a variety of guest operating systems"
HOMEPAGE = "http://qemu.org"
LICENSE = "GPL-2.0-only & LGPL-2.1-only"

RDEPENDS:${PN}-ptest = "bash"

require qemu-targets.inc
inherit pkgconfig ptest python3-dir

LIC_FILES_CHKSUM = "file://COPYING;md5=441c28d2cf86e15a37fa47e15a72fbac \
                    file://COPYING.LIB;endline=24;md5=8c5efda6cf1e1b03dcfd0e6c0d271c7f"
SRC_URI = "https://download.qemu.org/${BPN}-${PV}.tar.xz \
           file://powerpc_rom.bin \
           file://run-ptest \
           file://0001-qemu-Add-missing-wacom-HID-descriptor.patch \
           file://0003-qemu-Add-addition-environment-space-to-boot-loader-q.patch \
           file://0006-chardev-connect-socket-to-a-spawned-command.patch \
           file://0007-apic-fixup-fallthrough-to-PIC.patch \
           file://0010-configure-Add-pkg-config-handling-for-libgcrypt.patch \
           file://0001-qemu-Do-not-include-file-if-not-exists.patch \
           file://mmap2.patch \
           file://determinism.patch \
           file://0001-tests-meson.build-use-relative-path-to-refer-to-file.patch \
           file://0001-riscv-Set-5.4-as-minimum-kernel-version-for-riscv32.patch \
           file://0001-acpi-tpm-Add-missing-device-identification-objects.patch \
           file://0001-ppc-Include-asm-ptrace.h-for-pt_regs-struct-definiti.patch \
           file://0001-Define-MAP_SYNC-and-MAP_SHARED_VALIDATE-on-needed-li.patch \
           file://0001-vhost-vsock-detach-the-virqueue-element-in-case-of-e.patch \
           file://0002-virtio-net-fix-map-leaking-on-error-during-receive.patch \
           file://pvrdma.patch \
           file://CVE-2021-4206.patch \
           file://CVE-2021-4207.patch \
           file://CVE-2022-35414.patch \
           file://CVE-2021-3929.patch \
           file://CVE-2021-4158.patch \
           file://CVE-2022-0358.patch \
           file://CVE-2022-0216_1.patch \
           file://CVE-2022-0216_2.patch \
           file://CVE-2021-3750-1.patch \
           file://CVE-2021-3750-2.patch \
           file://CVE-2021-3750-3.patch \
           file://0001-use-uint32t-for-reply-queue-head-tail-values.patch \
           file://0002_let_dma_memory_valid_function_take_MemTxAttrs_argument.patch \
           file://0003_let_dma_memory_set_function_take_MemTxAttrs_argument.patch \
           file://0004_let_dma_memory_rw_relaxed_function_take_MemTxAttrs_argument.patch \
           file://0005_let_dma_memory_rw_function_take_MemTxAttrs_argument.patch \
           file://0006_let_dma_memory_read_write_function_take_MemTxAttrs_argument.patch \
           file://0007_let_dma_memory_map_function_take_MemTxAttrs_argument.patch \
           file://0008_have_dma_buf_rw_function_take_a_void_pointer.patch \
           file://0009_have_dma_buf_read_and_dma_buf_write_functions_take_a_void.patch \
           file://0010_let_pci_dma_rw_function_take_MemTxAttrs_argument.patch \
           file://0011_let_dma_buf_rw_function_take_MemTxAttrs_argument.patch \
           file://0012_let_dma_buf_write_function_take_MemTxAttrs_argument.patch \
           file://0013_let_dma_buf_read_function_take_MemTxAttrs_argument.patch \
           file://0014_let_dma_buf_rw_function_propagate_MemTxResult.patch \
           file://0015_let_st_pointer_dma_function_take_MemTxAttrs_argument.patch \
           file://0016_let_ld_pointer_dma_function_take_MemTxAttrs_argument.patch \
           file://0017_let_st_pointer_dma_function_propagate_MemTxResult.patch \
           file://0018_let_ld_pointer_dma_function_propagate_MemTxResult.patch \
           file://0019_let_st_pointer_pci_dma_function_take_MemTxAttrs_argument.patch \
           file://0020_let_ld_pointer_pci_dma_function_take_MemTxAttrs_argument.patch \
           file://0021_let_st_pointer_pci_dma_function_propagate_MemTxResult.patch \
           file://0022_let_ld_pointer_pci_dma_function_propagate_MemTxResult.patch \
           file://CVE-2021-3611_1.patch \
           file://CVE-2021-3611_2.patch \
           file://0001-net-tulip-Restrict-DMA-engine-to-memories.patch \
           file://0001-softfloat-Extend-float_exception_flags-to-16-bits.patch \
           file://0002-softfloat-Add-flag-specific-to-Inf-Inf.patch \
           file://0003-softfloat-Add-flag-specific-to-Inf-0.patch \
           file://0004-softfloat-Add-flags-specific-to-Inf-Inf-and-0-0.patch \
           file://0005-softfloat-Add-flag-specific-to-signaling-nans.patch \
           file://0006-target-ppc-Update-float_invalid_op_addsub-for-new-fl.patch \
           file://0007-target-ppc-Update-float_invalid_op_mul-for-new-flags.patch \
           file://0008-target-ppc-Update-float_invalid_op_div-for-new-flags.patch \
           file://0009-target-ppc-Update-fmadd-for-new-flags.patch \
           file://0010-target-ppc-Split-out-do_fmadd.patch \
           file://0011-target-ppc-Fix-xs-max-min-cj-dp-to-use-VSX-registers.patch \
           file://0012-target-ppc-Move-xs-max-min-cj-dp-to-decodetree.patch \
           file://0013-target-ppc-fix-xscvqpdp-register-access.patch \
           file://0014-target-ppc-move-xscvqpdp-to-decodetree.patch \
           file://0015-target-ppc-ppc_store_fpscr-doesn-t-update-bits-0-to-.patch \
           file://0016-target-ppc-Introduce-TRANS-FLAGS-macros.patch \
           file://0017-target-ppc-Implement-Vector-Expand-Mask.patch \
           file://0018-target-ppc-Implement-Vector-Extract-Mask.patch \
           file://0019-target-ppc-Implement-Vector-Mask-Move-insns.patch \
           file://0020-target-ppc-move-xs-n-madd-am-ds-p-xs-n-msub-am-ds-p-.patch \
           file://0021-target-ppc-implement-xs-n-maddqp-o-xs-n-msubqp-o.patch \
           file://CVE-2022-3165.patch \
           file://CVE-2022-4144.patch \
           file://0001-hw-display-qxl-Have-qxl_log_command-Return-early-if-.patch \
           file://0001-hw-display-qxl-Pass-requested-buffer-size-to-qxl_phy.patch \
           file://CVE-2023-0330.patch \
           file://CVE-2023-3301.patch \
           file://CVE-2023-3255.patch \
           file://CVE-2023-2861.patch \
           file://CVE-2020-14394.patch \
           file://CVE-2023-3354.patch \
           file://CVE-2023-3180.patch \
           file://CVE-2021-3638.patch \
           file://CVE-2023-1544.patch \
           file://CVE-2023-5088.patch \
           file://CVE-2024-24474.patch \
           file://CVE-2023-6693.patch \
           file://scsi-disk-allow-MODE-SELECT-block-desriptor-to-set-the-block-size.patch \
           file://scsi-disk-ensure-block-size-is-non-zero-and-changes-limited-to-bits-8-15.patch \
           file://CVE-2023-42467.patch \
           file://CVE-2023-6683.patch \
           file://CVE-2024-7409-0001.patch \
           file://CVE-2024-7409-0002.patch \
           file://CVE-2024-7409-0003.patch \
           file://CVE-2024-7409-0004.patch \
           file://CVE-2024-4467-0001.patch \
           file://CVE-2024-4467-0002.patch \
           file://CVE-2024-4467-0003.patch \
           file://CVE-2024-4467-0004.patch \
           file://CVE-2024-4467-0005.patch \
           file://CVE-2023-3019-0001.patch \
           file://CVE-2023-3019-0002.patch \
           file://CVE-2024-6505.patch \
           file://CVE-2024-3446-0001.patch \
           file://CVE-2024-3446-0002.patch \
           file://CVE-2024-3446-0003.patch \
           file://CVE-2024-3446-0004.patch \
           file://CVE-2024-3446-0005.patch \
           file://CVE-2024-3446-0006.patch \
           file://CVE-2024-3447.patch \
           "
UPSTREAM_CHECK_REGEX = "qemu-(?P<pver>\d+(\.\d+)+)\.tar"

SRC_URI[sha256sum] = "68e15d8e45ac56326e0b9a4afa8b49a3dfe8aba3488221d098c84698bca65b45"

SRC_URI:append:class-target = " file://cross.patch"
SRC_URI:append:class-nativesdk = " file://cross.patch"

# Applies against virglrender < 0.6.0 and not qemu itself
CVE_CHECK_IGNORE += "CVE-2017-5957"

# The VNC server can expose host files uder some circumstances. We don't
# enable it by default.
CVE_CHECK_IGNORE += "CVE-2007-0998"

# 'The issues identified by this CVE were determined to not constitute a vulnerability.'
# https://bugzilla.redhat.com/show_bug.cgi?id=1609015#c11
CVE_CHECK_IGNORE += "CVE-2018-18438"

# As per https://nvd.nist.gov/vuln/detail/CVE-2023-0664
# https://bugzilla.redhat.com/show_bug.cgi?id=2167423
# this bug related to windows specific.
CVE_CHECK_IGNORE += "CVE-2023-0664"

# As per https://bugzilla.redhat.com/show_bug.cgi?id=2203387
# RHEL specific issue
CVE_CHECK_IGNORE += "CVE-2023-2680"

# The CVE has disputed flag in NVD DB and also descrition contains:
# Note: This has been disputed by multiple third parties as not a valid vulnerability
#       due to the rocker device not falling within the virtualization use case.
CVE_CHECK_IGNORE += "CVE-2022-36648"

# disputed: not an issue as per https://bugzilla.redhat.com/show_bug.cgi?id=2223985
CVE_CHECK_IGNORE += "CVE-2023-1386"

COMPATIBLE_HOST:mipsarchn32 = "null"
COMPATIBLE_HOST:mipsarchn64 = "null"
COMPATIBLE_HOST:riscv32 = "null"

# Per https://lists.nongnu.org/archive/html/qemu-devel/2020-09/msg03873.html
# upstream states qemu doesn't work without optimization
DEBUG_BUILD = "0"

do_install:append() {
    # Prevent QA warnings about installed ${localstatedir}/run
    if [ -d ${D}${localstatedir}/run ]; then rmdir ${D}${localstatedir}/run; fi
}

do_install_ptest() {
	cp -rL ${B}/tests ${D}${PTEST_PATH}
	find ${D}${PTEST_PATH}/tests -type f -name "*.[Sshcodp]" | xargs -i rm -rf {}

	# Don't check the file genreated by configure
	sed -i -e "1s,#!/usr/bin/bash,#!${base_bindir}/bash," ${D}${PTEST_PATH}/tests/data/acpi/disassemle-aml.sh

	# Strip the paths from the QEMU variable, we can use PATH
	sed -i -e "s#^QEMU=.*/qemu-#QEMU=qemu-#g" ${D}${PTEST_PATH}/tests/tcg/*.mak

        # Strip compiler flags as they break reproducibility
        sed -i -e "s,CROSS_CC_GUEST=.*,CROSS_CC_GUEST=," ${D}${PTEST_PATH}/tests/tcg/*.mak
}

# QEMU_TARGETS is overridable variable
QEMU_TARGETS ?= "arm aarch64 i386 mips mipsel mips64 mips64el ppc ppc64 ppc64le riscv32 riscv64 sh4 x86_64"

EXTRA_OECONF = " \
    --prefix=${prefix} \
    --bindir=${bindir} \
    --includedir=${includedir} \
    --libdir=${libdir} \
    --mandir=${mandir} \
    --datadir=${datadir} \
    --docdir=${docdir}/${BPN} \
    --sysconfdir=${sysconfdir} \
    --libexecdir=${libexecdir} \
    --localstatedir=${localstatedir} \
    --with-suffix=${BPN} \
    --disable-strip \
    --disable-werror \
    --extra-cflags='${CFLAGS}' \
    --extra-ldflags='${LDFLAGS}' \
    --with-git=/bin/false \
    --with-git-submodules=ignore \
    --meson=meson \
    ${PACKAGECONFIG_CONFARGS} \
    "

B = "${WORKDIR}/build"

#EXTRA_OECONF:append = " --python=${HOSTTOOLS_DIR}/python3"

do_configure:prepend:class-native() {
	# Append build host pkg-config paths for native target since the host may provide sdl
	BHOST_PKGCONFIG_PATH=$(PATH=/usr/bin:/bin pkg-config --variable pc_path pkg-config || echo "")
	if [ ! -z "$BHOST_PKGCONFIG_PATH" ]; then
		export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$BHOST_PKGCONFIG_PATH
	fi
}

do_configure() {
	# This is taken from meson.bbclass to avoid errors when updating to a
	# new version of meson.
	rmdir ${STAGING_LIBDIR_NATIVE}/${PYTHON_DIR}/site-packages/*.egg-info 2>/dev/null || :

	${S}/configure ${EXTRA_OECONF}
}
do_configure[cleandirs] += "${B}"

do_install () {
	export STRIP=""
	oe_runmake 'DESTDIR=${D}' install
}

# The following fragment will create a wrapper for qemu-mips user emulation
# binary in order to work around a segmentation fault issue. Basically, by
# default, the reserved virtual address space for 32-on-64 bit is set to 4GB.
# This will trigger a MMU access fault in the virtual CPU. With this change,
# the qemu-mips works fine.
# IMPORTANT: This piece needs to be removed once the root cause is fixed!
do_install:append() {
	if [ -e "${D}/${bindir}/qemu-mips" ]; then
		create_wrapper ${D}/${bindir}/qemu-mips \
			QEMU_RESERVED_VA=0x0
	fi
}
# END of qemu-mips workaround

# Disable kvm/virgl/mesa on targets that do not support it
PACKAGECONFIG:remove:darwin = "kvm virglrenderer epoxy gtk+"
PACKAGECONFIG:remove:mingw32 = "kvm virglrenderer epoxy gtk+"

PACKAGECONFIG[sdl] = "--enable-sdl,--disable-sdl,libsdl2"
PACKAGECONFIG[virtfs] = "--enable-virtfs --enable-attr --enable-cap-ng,--disable-virtfs,libcap-ng attr,"
PACKAGECONFIG[aio] = "--enable-linux-aio,--disable-linux-aio,libaio,"
PACKAGECONFIG[uring] = "--enable-linux-io-uring,--disable-linux-io-uring,liburing"
PACKAGECONFIG[xfs] = "--enable-xfsctl,--disable-xfsctl,xfsprogs,"
PACKAGECONFIG[xen] = "--enable-xen,--disable-xen,xen-tools,xen-tools-libxenstore xen-tools-libxenctrl xen-tools-libxenguest"
PACKAGECONFIG[vnc-sasl] = "--enable-vnc --enable-vnc-sasl,--disable-vnc-sasl,cyrus-sasl,"
PACKAGECONFIG[vnc-jpeg] = "--enable-vnc --enable-vnc-jpeg,--disable-vnc-jpeg,jpeg,"
PACKAGECONFIG[vnc-png] = "--enable-vnc --enable-vnc-png,--disable-vnc-png,libpng,"
PACKAGECONFIG[libcurl] = "--enable-curl,--disable-curl,curl,"
PACKAGECONFIG[nss] = "--enable-smartcard,--disable-smartcard,nss,"
PACKAGECONFIG[curses] = "--enable-curses,--disable-curses,ncurses,"
PACKAGECONFIG[gtk+] = "--enable-gtk,--disable-gtk,gtk+3 gettext-native"
PACKAGECONFIG[vte] = "--enable-vte,--disable-vte,vte gettext-native"
PACKAGECONFIG[libcap-ng] = "--enable-cap-ng,--disable-cap-ng,libcap-ng,"
PACKAGECONFIG[ssh] = "--enable-libssh,--disable-libssh,libssh,"
PACKAGECONFIG[gcrypt] = "--enable-gcrypt,--disable-gcrypt,libgcrypt,"
PACKAGECONFIG[nettle] = "--enable-nettle,--disable-nettle,nettle"
PACKAGECONFIG[libusb] = "--enable-libusb,--disable-libusb,libusb1"
PACKAGECONFIG[fdt] = "--enable-fdt,--disable-fdt,dtc"
PACKAGECONFIG[alsa] = "--audio-drv-list=default,,alsa-lib"
PACKAGECONFIG[epoxy] = "--enable-opengl,--disable-opengl,libepoxy"
PACKAGECONFIG[lzo] = "--enable-lzo,--disable-lzo,lzo"
PACKAGECONFIG[numa] = "--enable-numa,--disable-numa,numactl"
PACKAGECONFIG[gnutls] = "--enable-gnutls,--disable-gnutls,gnutls"
PACKAGECONFIG[bzip2] = "--enable-bzip2,--disable-bzip2,bzip2"
PACKAGECONFIG[libiscsi] = "--enable-libiscsi,--disable-libiscsi"
PACKAGECONFIG[kvm] = "--enable-kvm,--disable-kvm"
PACKAGECONFIG[virglrenderer] = "--enable-virglrenderer,--disable-virglrenderer,virglrenderer"
# spice will be in meta-networking layer
PACKAGECONFIG[spice] = "--enable-spice,--disable-spice,spice"
# usbredir will be in meta-networking layer
PACKAGECONFIG[usb-redir] = "--enable-usb-redir,--disable-usb-redir,usbredir"
PACKAGECONFIG[snappy] = "--enable-snappy,--disable-snappy,snappy"
PACKAGECONFIG[glusterfs] = "--enable-glusterfs,--disable-glusterfs,glusterfs"
PACKAGECONFIG[xkbcommon] = "--enable-xkbcommon,--disable-xkbcommon,libxkbcommon"
PACKAGECONFIG[libudev] = "--enable-libudev,--disable-libudev,eudev"
PACKAGECONFIG[libxml2] = "--enable-libxml2,--disable-libxml2,libxml2"
PACKAGECONFIG[attr] = "--enable-attr,--disable-attr,attr,"
PACKAGECONFIG[rbd] = "--enable-rbd,--disable-rbd,ceph,ceph"
PACKAGECONFIG[vhost] = "--enable-vhost-net,--disable-vhost-net,,"
PACKAGECONFIG[ust] = "--enable-trace-backend=ust,--enable-trace-backend=nop,lttng-ust,"
PACKAGECONFIG[pie] = "--enable-pie,--disable-pie,,"
PACKAGECONFIG[seccomp] = "--enable-seccomp,--disable-seccomp,libseccomp"
# libnfs is currently provided by meta-kodi
PACKAGECONFIG[libnfs] = "--enable-libnfs,--disable-libnfs,libnfs"
PACKAGECONFIG[pmem] = "--enable-libpmem,--disable-libpmem,pmdk"
PACKAGECONFIG[pulsedio] = "--enable-pa,--disable-pa,pulseaudio"
PACKAGECONFIG[selinux] = "--enable-selinux,--disable-selinux"
PACKAGECONFIG[bpf] = "--enable-bpf,--disable-bpf,libbpf"
PACKAGECONFIG[capstone] = "--enable-capstone,--disable-capstone"
PACKAGECONFIG[rdma] = "--enable-rdma,--disable-rdma"
PACKAGECONFIG[vde] = "--enable-vde,--disable-vde"
PACKAGECONFIG[slirp] = "--enable-slirp=internal,--disable-slirp"
PACKAGECONFIG[brlapi] = "--enable-brlapi,--disable-brlapi"
PACKAGECONFIG[jack] = "--enable-jack,--disable-jack,jack,"

INSANE_SKIP:${PN} = "arch"

FILES:${PN} += "${datadir}/icons"
