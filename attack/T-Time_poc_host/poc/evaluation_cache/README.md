# Evaluation cache

This folder showcases the use of the cache amplification technique employed by the T-Time attack to enhance timing differences.

(You can refer the video Ttime_cache.mp4 in this folder)

**Brief experiment description**

```c
if(num == 0)
{
 IMUL_inst  // Code cache line A
}
else
{
 IMUL_inst // Code cache line B
}
```

In this experiment, the same instruction is executed regardless of whether num is 0 or 1 within a secret-dependent branch, making it difficult to distinguish instruction execution times.

However, by applying the cache amplification technique proposed in T-Time, this experiment demonstrates that it becomes possible to differentiate the two cases based on amplified timing differences.

For experimental purposes, we manually compiled all necessary libraries and header files, and included only the components required to run the experiments.

mbedtls : [https://github.com/Mbed-TLS/mbedtls/releases/tag/v3.5.2](https://github.com/Mbed-TLS/mbedtls/releases/tag/v3.5.2)

libwebp : [https://github.com/webmproject/libwebp/releases/tag/v1.5.0](https://github.com/webmproject/libwebp/releases/tag/v1.5.0)

The versions of gcc and g++ used during compilation are as follows:

```bash
gcc (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
Copyright (C) 2023 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

g++ (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
Copyright (C) 2023 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

ldd (Ubuntu GLIBC 2.39-0ubuntu8.4) 2.39
Copyright (C) 2024 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
Written by Roland McGrath and Ulrich Drepper.
```

Please note that detailed code explanations is currently under development and is not included in this release.

The explanation will proceed by alternating between the host and TD (VM) perspectives.

1. **VM perspective**

Navigate to the T-Time_poc_vm/evaluation_cache directory and run the [build.sh](http://build.sh/) script

Then, execute the print_address program. You should see output similar to the following:

```bash
root@tdx-guest:~/VM/T-Time_poc_vm/evaluation_cache# ./print_address 
Matching library:
Backed by:	/root/VM/T-Time_poc_vm/evaluation_cache/libasm_library.so
Range:		0x7ffff7fba000-0x7ffff7fc0000
Length:		24576
Offset:		4096
Permissions:	r-xp
Inode:		262891
Device:		fd:01

Code Location target                   : vaddr=0x7ffff7fbe000, paddr=0xc8b1000 [pfn 0xc8b1]
Code Location start                    : vaddr=0x7ffff7fbc000, paddr=0x188a2000 [pfn 0x188a2]
```

Take note of the target and start PFNs displayed in the output: 

- target: 0xc8b1
- start: 0x188a2

1. **Host perspective**

Navigate to the T-Time_poc_host/poc/evaluation_cache directory and run the [setting.sh](http://setting.sh/) script

This script is configured to evict the cache line corresponding to branch 1 of the target code.

Enter the PFNs for target and start that you previously noted from the virtual machine output.

```bash
root@tdx:/home/csl/HOST/T-Time/attack/T-Time_poc_host/poc/evaluation_cache# ./setting.sh
Loading remove.ko from /home/csl/HOST/T-Time/attack/T-Time_poc_host/script/../module/removelist...
Unloading remove...
Loading flushremove.ko from /home/csl/HOST/T-Time/attack/T-Time_poc_host/script/../module/flushremovelist...
Unloading flushremove...
flushremove.ko successfully unloaded.
Enter guest PFN for (target) (e.g., 0x1234): **0xc8b1**
Enter guest PFN for (start) (e.g., 0x5678): **0x188a2**
```

Then, the block page list and the flush target addresses will be displayed.

```bash
---------------------------------------
Loading allprint.ko from /home/csl/HOST/T-Time/attack/T-Time_poc_host/script/../module/printlist...
Unloading allprint...
allprint.ko successfully unloaded.
[  302.111184] VM process found: qemu-system-x86 (PID: 2779)
[  302.111191] Running woomin_list_print and woomin_flush_list_print...
[  302.111192] [woomin_list_print] woomin_list[0][target] = 0xc8b1 (blocked = 1)
[  302.111195] [woomin_list_print] woomin_list[1][start] = 0x188a2 (blocked = 1)
[  302.111196] [woomin_flush_list] woomin_flush list[0] = 0xc8b1100 (VA : 0xff116453b12b1100)
[  302.120325] Exiting TDX Print Module
---------------------------------------
```

1. **Host perspective**

Run the capture_0.sh script

**Important**: Wait until the message appears before proceeding.

```bash
execute the target program ! 
```

1. **VM perspective**

Run the instruction program. 

When prompted with “enter the num :”, input 0 and press Enter.

Wait until the confirmation message is displayed as follows:

```bash
Done!
```

1. **Host perspective**

Terminate the program by pressing Ctrl + C.

1. **Host perspective**

Run the capture_1.sh script:

**Important:** Wait until the message

```bash
execute the target program !
```

appears before proceeding.

1. **VM perspective**

Run the instruction program again.

When prompted with “enter the num :”, input 1 and press Enter.

Wait until the following message is displayed:

```bash
Done!
```

1. **Host perspective**

Terminate the program by pressing Ctrl + C.

Then, run the [parse.sh](http://parse.sh) script:

Finally, check the final results in the T-time_cache_comparison.pdf file.