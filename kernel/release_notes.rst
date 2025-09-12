==========================================
Intel Trust Dodmain Extensions(TDX) Kernel
==========================================

Branch v6.8-tdx -- based on https://github.com/intel/tdx.git tag kvm-upstream-2024.02.27.v6.8-rc5 
-------------------------
* TDX, VMX coexistence:                 Applied
* TDX architectural definitions:        Applied
* TD VM creation/destruction:           Applied
* TD vcpu creation/destruction:         Applied
* TDX EPT violation:                    Applied
* TD finalization:                      Applied
* TD vcpu enter/exit:                   Applied

* KVM MMU GPA shared bits:              Applied
* KVM TDP refactoring for TDX:          Applied
* KVM TDP MMU hooks:                    Applied

* large page                            Applied

**TODO**: Need to backport RTMR support when the new patch released.

-------------------------
Usage
-------------------------
1. Add "nohibernate" to host kernel cmdline to enable TDX on host.
2. Disable kexec in kernel config for enabling TDX.

-------------------------
Known issue
-------------------------
1. Kexec and TDX conflict

Workaround: Disable kexec, remove CONFIG_KEXEC_CORE, CONFIG_KEXEC, CONFIG_KEXEC_FILE, CONFIG_CRASH_DUMP in kernel config
