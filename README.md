# T-Time
<p align="center">
  <img src="./T-Time_logo.png" alt="T-Time logo" width="500"><br/>
  <strong>T-Time: A Fine-grained Timing-based Controlled-Channel Attack against Intel TDX</strong><br/>
</p>
We introduce T-Time which is a fine-grained timing-based controlled-channel attack targeting Intel TDX. 
T-Time precisely measures dwell time—the interval between consecutive page faults—to uncover previously hidden sensitive control flows. 
We further enhance T-Time’s precision through a cache-based amplification technique.
We validate T-Time in two practical scenarios: extracting a 4096-bit RSA private key from MbedTLS, and reconstructing a WebP image through timing analysis during decoding. 
Our findings demonstrate that existing page-level defenses are inadequate against fine-grained timing attacks.



# Step 1. Experimental steup

**Processor** : Intel Xeon Silver 4510T Processor

**motherboard** : Giga Computing MS73-HB1-000

**TDX module** : attributes 0x0, vendor_id 0x8086, major_version 1, minor_version 5, build_date 20240407, build_num 744

### 1. TDX setting

We used the source codes for TDX provided by [canonical/tdx](https://github.com/canonical/tdx/releases/tag/3.1) repository.

github link  [https://github.com/canonical/tdx/releases/tag/3.1](https://github.com/canonical/tdx/releases/tag/3.1)

1. We created the virtual machine by following the instructions provided in the canonical/tdx repository. [Boot TD with virsh (libvirt)]

Host : Ubuntu 24.10 (kernel 6.11.0)

Guest : Ubuntu 24.04 LTS (kernel 6.8.0-36-generic).

2. We configured the virtual machine with a single vCPU, which was pinned to physical core with CPUID 0 as follows:

```bash
# 1. Set vCPU count to 1
echo "Setting vCPU count to 1 for VM: $VM_NAME"
sudo virsh setvcpus "$VM_NAME" 1 --maximum --config
sudo virsh setvcpus "$VM_NAME" 1 --config

# 2. Pin vCPU 0 to physical CPU 0
echo "Pinning vCPU 0 of $VM_NAME to physical CPU 0"
sudo virsh vcpupin "$VM_NAME" 0 0 --config
```

### 2 BIOS Setting

BIOS settings were configured according to the images in the bios/ directory. To ensure a fixed CPU frequency, please make sure to follow our configuration for P-State, C-State, and Turbo Boost as shown in the reference images.

### 3 Kernel Parameter

In our environment, hyper-threading is enabled, where core 0 and core 12 are sibling cores. Therefore, we used the following kernel parameters: isolcpus=0,12 nohz_full=0,12 rcu_nocbs=0,12 rcu_nocb_poll=0,12 If core 12 is not the sibling of core 0 in your system, please adjust the parameters according to your system's core topology.

```c
GRUB_CMDLINE_LINUX_DEFAULT="quiet splash isolcpus=0,12 nohz_full=0,12 rcu_nocbs=0,12 rcu_nocb_poll=0,12 processor.max_cstate=0 dis_ucode_dlr msr.allow_writes=on nmi_watchdog=0 iomem=relaxed no_timer_check log_buf_len=64MB transparent_hugepage=never"
```

---

# Step 2. Kernel build

We implemented the T-Time hypervisor kernel on top of Ubuntu linux-intel 6.11.0-1006.6.

(commit e63f2869c382571c07c0b44d2de8873f642feb08)

(https://git.launchpad.net/~canonical-kernel/ubuntu/+source/linux-intel/+git/oracular?h=main)

1. Change into the T-Time kernel directory with:

```bash
cd kernel
```

2. Install the required packages for building the kernel by running the install_package.sh script

```bash
./install_package.sh
```

3. Run the [command.sh](http://command.sh/) script to obtain the commands for the compile 

The commands will be displayed as follows:

```bash
sudo make -j24 && sudo make modules_install -j24 && sudo make install -j24 && grep -A100 submenu /boot/grub/grub.cfg | grep menuentry
```

4. Navigate to the extracted folder and run the compilation command you obtained from [command.sh](http://command.sh/).

5. Switch the host system's kernel to the T-Time kernel.

# Step 3. Deploying the T-Time PoC file

In the /attack directory, you will find two compressed files:

1. Deploy T-Time_poc_host folder on the host system.
   
2. Transfer T-Time_poc_vm folder into the TD (Trusted Domain) environment.

# Step 4. Setup the host and module

### Host setting

Run the T-Time_poc_host/setting.sh script. 

This script internally executes two additional shell scripts located in the script/ directory: 

- frequency.sh : Sets the frequency of core 0—and its sibling core, if hyper-threading is enabled—to 2.0 GHz by fixing their CPU frequency.
- tdxsplit.sh : Converts all physical memory pages allocated to the TD into 4KB pages.

**Note**: You must re-run this script after every system reboot or shutdown to ensure the environment is properly configured for the experiment.

### Module setting

In the /module directory, run the make_all.sh script to build all kernel modules If the make package is not installed on your system, please install it first. These kernel modules will later be used by the attack PoC we developed. 

---

Once you have completed the steps above, the environment setup is finished.

Next, refer to the README.md files located in each evaluation folder under the /poc directory.

We recommend following the evaluation steps in the following order.
1. evaluation_cache (Folder : attack\T-Time_poc_host\T-Time_poc_host\poc\evaluation_cache)
2. evaluation_mbedtls (Folder : attack\T-Time_poc_host\T-Time_poc_host\poc\evaluation_cache)
3. evaluation_webp (Folder : attack\T-Time_poc_host\T-Time_poc_host\poc\evaluation_webp)


# Reference

Our implementation was developed with reference to.

Wilke, L., Sieck, F., & Eisenbarth, T. (2024). TDXdown: Single-Stepping and Instruction Counting Attacks against Intel TDX.ACM CCS. 

([https://doi.org/10.5281/zenodo.12683611](https://doi.org/10.5281/zenodo.12683611))

We sincerely acknowledge and appreciate their contribution.
