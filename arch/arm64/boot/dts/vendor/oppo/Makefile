ifeq ($(CONFIG_BUILD_ARM64_DT_OVERLAY),y)
    dtbo-$(CONFIG_ARCH_KONA) += \
        20061-horee-kona-mtp-overlay.dtbo \
        20135-hima-kona-mtp-overlay.dtbo \
        20161-jin-kona-mtp-overlay.dtbo \
        20351-jin-kona-mtp-overlay.dtbo \
        21615-rivena-kona-mtp-overlay.dtbo \
        21619-bitra-kona-mtp-overlay.dtbo \
        21623-spartan-kona-mtp-overlay.dtbo \
        21733-spartan-kona-mtp-overlay.dtbo

    20061-horee-kona-mtp-overlay.dtbo-base := kona.dtb kona-v2.dtb kona-v2.1.dtb
    20135-hima-kona-mtp-overlay.dtbo-base := kona.dtb kona-v2.dtb kona-v2.1.dtb
    20161-jin-kona-mtp-overlay.dtbo-base := kona.dtb kona-v2.dtb kona-v2.1.dtb
    20351-jin-kona-mtp-overlay.dtbo-base := kona.dtb kona-v2.dtb kona-v2.1.dtb
    21615-rivena-kona-mtp-overlay.dtbo-base := kona.dtb kona-v2.dtb kona-v2.1.dtb
    21619-bitra-kona-mtp-overlay.dtbo-base := kona.dtb kona-v2.dtb kona-v2.1.dtb
    21623-spartan-kona-mtp-overlay.dtbo-base := kona.dtb kona-v2.dtb kona-v2.1.dtb
    21733-spartan-kona-mtp-overlay.dtbo-base := kona.dtb kona-v2.dtb kona-v2.1.dtb
endif

always		:= $(dtb-y)
subdir-y	:= $(dts-dirs)
clean-files    := *.dtb *.dtbo
