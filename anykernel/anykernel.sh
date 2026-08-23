### AnyKernel3 Ramdisk Mod Script
## osm0sis @ xda-developers

### AnyKernel setup
# begin properties
properties() { '
kernel.string=kernel by Bruce (Bruce Teng @ xda-developers)
do.devicecheck=1
device.name1=OP4E5D
device.name2=PEDM00
do.modules=0
do.systemless=0
do.cleanup=1
do.cleanuponabort=0
supported.versions=
supported.patchlevels=
'; } # end properties

# shell variables
block=boot;
is_slot_device=auto;
ramdisk_compression=auto;
patch_vbmeta_flag=auto;

## AnyKernel methods (DO NOT CHANGE)
# import patching functions/variables - see for reference
. tools/ak3-core.sh;

device_match=0;
for prop in ro.product.vendor.device ro.product.device ro.build.product ro.product.model; do
  value="$(getprop "$prop" 2>/dev/null)";
  case "$value" in
    OP4E5D|PEDM00) device_match=1; break;;
  esac;
done;
[ "$device_match" = 1 ] || abort "This package only supports OPPO Find X3 (PEDM00/OP4E5D).";

## AnyKernel boot install
dump_boot;

write_boot;
## end boot install
