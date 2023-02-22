cmd_/mnt/vmshare/crepo/kcodes/19_kthread/modules.order := {   echo /mnt/vmshare/crepo/kcodes/19_kthread/kthread.ko; :; } | awk '!x[$$0]++' - > /mnt/vmshare/crepo/kcodes/19_kthread/modules.order
