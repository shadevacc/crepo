cmd_/mnt/vmshare/kcodes/06_02_oops/modules.order := {   echo /mnt/vmshare/kcodes/06_02_oops/oops.ko; :; } | awk '!x[$$0]++' - > /mnt/vmshare/kcodes/06_02_oops/modules.order
