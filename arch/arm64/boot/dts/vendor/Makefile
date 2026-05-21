vendor := $(srctree)/$(src)

ifneq "$(wildcard $(vendor)/oppo)" ""
	subdir-y += oppo
else
ifneq "$(wildcard $(vendor)/qcom)" ""
	subdir-y += qcom
endif
endif
