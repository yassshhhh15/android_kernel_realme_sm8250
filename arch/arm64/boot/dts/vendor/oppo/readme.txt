记录各设备哪些dts文件存在差异化
差异化请移至<codename>/kona-mtp.dtsi,否则放kona.dtsi

项目号代表的机型
20061 OPPO Find X3
20135 OPPO Reno5 Pro+
20161 OPPO Reno6 Pro+
20351 OPPO Reno6 Pro(CPH2247)
21615 真我GT 大师探索版
21619 真我GT Neo2
21623 真我Q5 Pro
21733 realme GT Neo 3T(实际上和21623为同一台机子)

relame就以21615做参考

21615和21619
kona.dtsi    有差异化
1. &ssc_sensors中21619多了条property:vdd2-supply = <&pm8150a_l4>;
2. &L2P, &L3P节点中property差异化，打算将其非通用化

kona-thermal.dtsi   无差异化
kona-audio.dtsi     无差异化(包含被引用的msm-audio-lpass.dtsi也是如此)
kona-qupv3.dtsi     无差异化
kona-camera.dtsi    无差异化
kona-pcie.dtsi      无差异化

kona-pinctrl.dtsi   有差异化 
1. jiiov_fp 全新的指纹节点，可以放在公共区
2. &cam_sensor_mclk3_suspend下的config节点的drive-strength条目存在差异化

kona-regulators.dtsi    无差异化
kona-usb.dtsi       有差异化
1. &usb2_phy0节点下的qcom,param-override-seq和qcom,param-override-seq-host条目差异化 计划非通用化至kona-mtp.dtsi



21615和21623
kona.dtsi    有差异化
1. &cnss_wlan_mem的reg不一样，需要进行非通用化
2. &ssc_sensors中21623未对此节点做修改，而21615和21619则有oplus更改，需要非通用化
3. 21623的&wlan节多了条qcom,sw-ctrl-gpio = <&tlmm 124 0>;
4. &L2P和&L3P差异化

kona-thermal.dtsi   无差异化
kona-audio.dtsi     无差异化(包含被引用的msm-audio-lpass.dtsi也是如此)
kona-qupv3.dtsi     无差异化
kona-camera.dtsi    有差异化
1. 如下展示
&soc {
    qcom,cam_smmu {
        msm_cam_smmu_icp {
            qcom,iommu-faults = "stall-disable", "non-fatal";
        };
    };
};

kona-pcie.dtsi      无差异化
kona-pinctrl.dtsi   有差异化（同21615和21619的差异化一致）
kona-regulators.dtsi    无差异化
kona-usb.dtsi       有差异化（同21615和21619的差异化一致）



21615和20135
kona.dtsi 有差异化
1. &soc下20135少了cpu7-notify节点。应该在kona-mtp.dtsi删除
2. &cnss_wlan_mem下的reg大小也不一样
3. 多了个节点，oppo设备特有，放公共区貌似也行
midas_pdev {
		compatible = "oplus,midas-pdev";
};
4. &wlan下多了
    qcom,bt-en-gpio = <&tlmm 21 0>;
    20135的 qcom,wlan-ramdump-dynamic = <0x400000>;
            qcom,msm-bus,num-cases = <6>;
            qcom,msm-bus,vectors-KBps =
            /* no vote */
            <MSM_BUS_MASTER_PCIE MSM_BUS_SLAVE_FIRST 0 0>,
            /* idle: 0-18 Mbps, ddr freq: 451.2 MHz */
            <MSM_BUS_MASTER_PCIE MSM_BUS_SLAVE_FIRST 2250 1600000>,
            /* low: 18-60 Mbps, ddr freq: 451.2 MHz*/
            <MSM_BUS_MASTER_PCIE MSM_BUS_SLAVE_FIRST 7500 1600000>,
            /* medium: 60-240 Mbps, ddr freq: 451.2 MHz */
            <MSM_BUS_MASTER_PCIE MSM_BUS_SLAVE_FIRST 30000 1804800>,
            /* high: 240 - 800 Mbps, ddr freq: 451.2 MHz */
            <MSM_BUS_MASTER_PCIE MSM_BUS_SLAVE_FIRST 100000 1804800>,
            /* very high: 800 - 1400 Mbps, ddr freq: 1555.2 MHz */
            <MSM_BUS_MASTER_PCIE MSM_BUS_SLAVE_FIRST 175000 6220800>;

kona-thermal.dtsi 有差异化
1. 为20135删除&thermal_zones下的cwlan-step, video-step, ddr-step, q6-hvx-step, camera-step, cmpss-step, npu-step节点

kona-audio.dtsi 有差异化 已在kona-audio-overlay.dtsi处理
kona-qupv3.dtsi 有差异化 此处展示20135需要修改的
1. &i3c0下  pinctrl-names = "default", "sleep";
            并且需要删除pinctrl-2条目
2. &i3c1下  pinctrl-names = "default", "sleep";
            并且需要删除pinctrl-2条目

kona-camera.dtsi 无差异化
kona-pcie.dtsi 无差异化
kona-pinctrl.dtsi 有差异化
1. 需要删除此节点&pxlw_iris_gpio
2. &cam_sensor_mclk3_suspend的config节点，条目drive-strength = <6>; /* 6 MA */
3. 需要删除&qupv3_se0_i3c_disable和&qupv3_se1_i3c_disable节点
4. 
&qupv3_se3_spi_active {
    mux {
        pins = "gpio119", "gpio120", "gpio121",
                    "gpio122";
    };

    config {
        pins = "gpio119", "gpio120", "gpio121",
                    "gpio122";
    };
};

&qupv3_se3_spi_sleep {
    mux {
        pins = "gpio119", "gpio120", "gpio121",
                    "gpio122";
    };

    config {
        pins = "gpio119", "gpio120", "gpio121",
                    "gpio122";
    };
};

&qupv3_se12_i2c_active {
    mux {
        pins = "gpio32", "gpio33";
    };

    config {
        pins = "gpio32", "gpio33";
    };
};

&qupv3_se12_i2c_sleep {
    mux {
        pins = "gpio32", "gpio33";
    };

    config {
        pins = "gpio32", "gpio33";
    };
};

&qupv3_se12_spi_active {
    mux {
        pins = "gpio32", "gpio33", "gpio34",
                    "gpio35";
    };

    config {
        pins = "gpio32", "gpio33", "gpio34",
                    "gpio35";
    };
};

&qupv3_se12_spi_sleep {
    mux {
        pins = "gpio32", "gpio33", "gpio34",
                    "gpio35";
    };

    config {
        pins = "gpio32", "gpio33", "gpio34",
                    "gpio35";
    };
};

5. 删除以下节点
    &charge_pump_hwid_active
    &charge_pump_hwid_sleep
    &charge_pump_hwid_default

kona-regulators.dtsi 有差异化
&L15A {
    qcom,init-mode = <RPMH_REGULATOR_MODE_HPM>;
};

&L2C {
    regulator-min-microvolt = <1000000>;
    qcom,init-voltage = <1120000>;
};

&L3C {
    regulator-min-microvolt = <600000>;
};

&L5C {
    qcom,init-mode = <RPMH_REGULATOR_MODE_HPM>;
};

&L7C {
    regulator-min-microvolt = <2856000>;
    qcom,init-voltage = <2856000>;
    qcom,init-mode = <RPMH_REGULATOR_MODE_HPM>;
};

&L9C {
    qcom,init-mode = <RPMH_REGULATOR_MODE_HPM>;
};

kona-usb.dtsi 有差异化
&soc {
    ssusb@a600000 {
        dwc3@a600000 {
            maximum-speed = "high-speed";
        };
    };

    ssusb@a800000 {
        dwc3@a800000 {
            maximum-speed = "high-speed";
        };
    };
};

21615和20061
kona.dtsi
1. &cnss_wlan_mem:
    20061: 
        //#ifndef OPLUS_BUG_STABILITY
        size = <0x0 0x1C00000>;
        //#endif /* OPLUS_BUG_STABILITY */
2. &wlan多了条qcom,bt-en-gpio = <&tlmm 21 0>;

kona-thermal.dtsi 有差异化 已处理
kona-audio.dtsi 有差异化 已处理
kona-qupv3.dtsi     无差异化
kona-camera.dtsi    无差异化
kona-pcie.dtsi      无差异化
kona-pinctrl.dtsi   有差异化
&tlmm {
    /delete-node/ pxlw_iris_gpio;
	/delete-node/ cam_sensor_active_5;
	/delete-node/ cam_sensor_suspend_5;
    /delete-node/ cam_sensor_vana1_default;
    /delete-node/ cam_sensor_vana3_default;
    /delete-node/ cam_sensor_vana4_default;

	cam_sensor_active_4: cam_sensor_active_4 {
		mux {
			pins = "gpio74";
			function = "gpio";
		};

		config {
			pins = "gpio74";
			bias-disable;
			drive-strength = <2>;
		};
	};

	cam_sensor_suspend_4: cam_sensor_suspend_4 {
		mux {
			pins = "gpio74";
			function = "gpio";
		};

		config {
			pins = "gpio74";
			bias-pull-down;
			drive-strength = <2>;
			output-low;
		};
	};
};

&cam_sensor_mclk3_suspend {
    config {
        drive-strength = <6>;
    };
};

&cam_sensor_active_3 {
    mux {
        pins = "gpio93";
    };

    config {
        pins = "gpio93";
    };
};

&cam_sensor_suspend_3 {
    mux {
        pins = "gpio93";
    };

    config {
        pins = "gpio93";
    };   
};

&cci0_active {
    config {
        /delete-property/ bias-disable;
        bias-pull-up; /* PULL UP*/
    };
};

&cci0_suspend {
    config {
        /delete-property/ bias-disable;
        bias-pull-down; /* PULL DOWN */
    };
};

&cci1_active {
    config {
        /delete-property/ bias-disable;
        bias-pull-up; /* PULL UP*/
    };
};

&cci1_suspend {
    config {
        /delete-property/ bias-disable;
        bias-pull-down; /* PULL DOWN */
    };
};

&cci2_active {
    config {
        /delete-property/ bias-disable;
        bias-pull-up; /* PULL UP*/
    };
};

&cci2_suspend {
    config {
        /delete-property/ bias-disable;
        bias-pull-down; /* PULL DOWN */
    };
};

&cci3_active {
    config {
        /delete-property/ bias-disable;
        bias-pull-up; /* PULL UP*/
    };
};

&cci3_suspend {
    config {
        /delete-property/ bias-disable;
        bias-pull-down; /* PULL DOWN */
    };
};

&qupv3_se12_i2c_active {
    mux {
        pins = "gpio32", "gpio33";
    };

    config {
        pins = "gpio32", "gpio33";
    };
};

&qupv3_se12_i2c_sleep {
    mux {
        pins = "gpio32", "gpio33";
    };

    config {
        pins = "gpio32", "gpio33";
    };
};

&qupv3_se12_spi_active {
    mux {
        pins = "gpio32", "gpio33", "gpio34", "gpio35";
    };

    config {
        pins = "gpio32", "gpio33", "gpio34", "gpio35";
    };
};

&qupv3_se12_spi_sleep {
    mux {
        pins = "gpio32", "gpio33", "gpio34", "gpio35";
    };

    config {
        pins = "gpio32", "gpio33", "gpio34", "gpio35";
    };    
};

&charger {
    /delete-node/ charge_pump_hwid_active;
    /delete-node/ charge_pump_hwid_sleep;
    /delete-node/ charge_pump_hwid_default;

    ext2_wireless_otg_en_active: ext2_wireless_otg_en_active {
		mux {
			pins = "gpio67";
			function = "gpio";
		};

		config {
			pins = "gpio67";
			drive-strength = <2>;
			bias-disable;
		};
	};

	ext2_wireless_otg_en_sleep: ext2_wireless_otg_en_sleep {
		mux {
			pins = "gpio67";
			function = "gpio";
		};

		config {
			pins = "gpio67";
			drive-strength = <2>;
			bias-disable;
		};
	};

	ext2_wireless_otg_en_default: ext2_wireless_otg_en_default {
		mux {
			pins = "gpio67";
			function = "gpio";
		};

		config {
			pins = "gpio67";
			drive-strength = <2>;
			bias-disable;
		};
	};

	ext1_wired_otg_en_active: ext1_wired_otg_en_active {
		mux {
			pins = "gpio64";
			function = "gpio";
		};

		config {
			pins = "gpio64";
			drive-strength = <2>;
			bias-disable;
		};
	};

	ext1_wired_otg_en_sleep: ext1_wired_otg_en_sleep {
		mux {
			pins = "gpio64";
			function = "gpio";
		};

		config {
			pins = "gpio64";
			drive-strength = <2>;
			bias-disable;
		};
	};

	ext1_wired_otg_en_default: ext1_wired_otg_en_default {
		mux {
			pins = "gpio64";
			function = "gpio";
		};

		config {
			pins = "gpio64";
			drive-strength = <2>;
			bias-disable;
		};
	};

	cp_int_active: cp_int_active {
		mux {
			pins = "gpio126";
			function = "gpio";
		};

		config {
			pins = "gpio126";
			drive-strength = <2>;
			bias-disable;
		};
	};

	cp_int_sleep: cp_int_sleep {
		mux {
			pins = "gpio126";
			function = "gpio";
		};

		config {
			pins = "gpio126";
			drive-strength = <2>;
			bias-disable;
		};
	};

	cp_int_default: cp_int_default {
		mux {
			pins = "gpio126";
			function = "gpio";
		};

		config {
			pins = "gpio126";
			drive-strength = <2>;
			bias-disable;
		};
	};

	cp_en_active: cp_en_active {
		mux {
			pins = "gpio114";
			function = "gpio";
		};

		config {
			pins = "gpio114";
			drive-strength = <2>;
			bias-disable;
		};
	};

	cp_en_sleep: cp_en_sleep {
		mux {
			pins = "gpio114";
			function = "gpio";
		};

		config {
			pins = "gpio114";
			drive-strength = <2>;
			bias-disable;
		};
	};

	cp_en_default: cp_en_default {
		mux {
			pins = "gpio114";
			function = "gpio";
		};

		config {
			pins = "gpio114";
			drive-strength = <2>;
			bias-disable;
		};
	};

	idt_int_active: idt_int_active {
		mux {
			pins = "gpio118";
			function = "gpio";
		};

		config {
			pins = "gpio118";
			drive-strength = <2>;
			input-enable;
			/*bias-pull-up; PULL UP*/
			bias-disable;
		};
	};

	idt_int_sleep: idt_int_sleep {
		mux {
			pins = "gpio118";
			function = "gpio";
		};

		config {
			pins = "gpio118";
			drive-strength = <2>;
			bias-disable;
		};
	};

	idt_int_default: idt_int_default {
		mux {
			pins = "gpio118";
			function = "gpio";
		};

		config {
			pins = "gpio118";
			drive-strength = <2>;
			bias-disable;
		};
	};

	idt_connect_active: idt_connect_active {
		mux {
			pins = "gpio113";
			function = "gpio";
		};

		config {
			pins = "gpio113";
			drive-strength = <2>;
			bias-disable; 
			input-enable;
			/*bias-pull-up; PULL UP*/
		};
	};

	idt_connect_sleep: idt_connect_sleep {
		mux {
			pins = "gpio113";
			function = "gpio";
		};

		config {
			pins = "gpio113";
			drive-strength = <2>;
			bias-disable; 
		};
	};

	idt_connect_default: idt_connect_default {
		mux {
			pins = "gpio113";
			function = "gpio";
		};

		config {
			pins = "gpio113";
			drive-strength = <2>;
			bias-disable; 
		};
	};

	vbat_en_active: vbat_en_active {
		mux {
			pins = "gpio117";
			function = "gpio";
		};

		config {
			pins = "gpio117";
			drive-strength = <2>;
			bias-pull-up;
		};
	};

	vbat_en_sleep: vbat_en_sleep {
		mux {
			pins = "gpio117";
			function = "gpio";
		};

		config {
			pins = "gpio117";
			drive-strength = <2>;
			bias-pull-down;
		};
	};

	vbat_en_default: vbat_en_default {
		mux {
			pins = "gpio117";
			function = "gpio";
		};

		config {
			pins = "gpio117";
			drive-strength = <2>;
			bias-pull-up;
		};
	};

	vt_sleep_active: vt_sleep_active {
		mux {
			pins = "gpio119";
			function = "gpio";
		};

		config {
			pins = "gpio119";
			drive-strength = <2>;
			bias-pull-up;
		};
	};

	vt_sleep_sleep: vt_sleep_sleep {
		mux {
			pins = "gpio119";
			function = "gpio";
		};

		config {
			pins = "gpio119";
			drive-strength = <2>;
			bias-pull-down;
		};
	};

	vt_sleep_default: vt_sleep_default {
		mux {
			pins = "gpio119";
			function = "gpio";
		};

		config {
			pins = "gpio119";
			drive-strength = <2>;
			bias-pull-up;
		};
	};
};

kona-regulators.dtsi 有差异化
&pm8150a_l2 {
    regulator-min-microvolt = <1200000>;
    qcom,init-voltage = <1200000>;
};

kona-usb.dtsi   有差异化
/*#ifdef OPLUS_FEATURE_CHG_BASIC*/
/* Modify for otg usb storage */
&soc {
    dwc3@a600000 {
        maximum-speed = "super-speed";
    };
};
/*#endif*/
