SUMMARY = "WiFi Link Quality Stats standalone process"
HOMEPAGE = "https://github.com/nickel-xb/linkquality-stats"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://configure.ac;beginline=1;endline=17;md5=TODO_UPDATE_CHECKSUM"

DEPENDS = "rbus cjson"
RDEPENDS:${PN} = "rbus cjson"

SRC_URI = "${CMF_GITHUB_ROOT}/linkquality-stats;protocol=${CMF_GIT_PROTOCOL};branch=${CMF_GIT_BRANCH};name=linkquality-stats"
SRCREV_linkquality-stats = "${AUTOREV}"
PV = "${RDK_RELEASE}+git${SRCPV}"

S = "${WORKDIR}/git"

inherit autotools pkgconfig systemd

CFLAGS:append = " \
    -I${STAGING_INCDIR}/rbus \
"

LDFLAGS:append = " \
    -lrbus \
    -lcjson \
    -lstdc++ \
    -lpthread \
    -lm \
"

ISSYSTEMD = "${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}"

do_install:append() {
    if [ "${ISSYSTEMD}" = "true" ]; then
        install -d ${D}${systemd_unitdir}/system
        install -m 0644 ${S}/linkquality_stats.service ${D}${systemd_unitdir}/system/
    fi
}

FILES:${PN} += "${bindir}/linkquality_stats"
SYSTEMD_SERVICE:${PN} = "${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'linkquality_stats.service', '', d)}"
