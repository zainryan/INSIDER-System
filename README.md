# INSIDER
![Status](https://img.shields.io/badge/Version-Experimental-green.svg)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Table of Contents

- [INSIDER](#insider)
  * [Papers](#papers)
  * [Restrictions](#restrictions)
  * [Build from Source](#build-from-source)
    + [AWS EC2](#aws-ec2)
    + [AWS FPGA Toolchain](#aws-fpga-toolchain)
    + [Linux Kernel](#linux-kernel)
    + [Boost](#boost)
    + [LLVM and Clang](#llvm-and-clang)
    + [Run The Installation Script](#run-the-installation-script)
  * [Use Prebuilt AWS Image](#use-prebuilt-aws-image)
  * [Folder Organizations](#folder-organizations)
  * [Applications](#applications)
  * [Usage](#usage)
    + [The Syntax of Device Code](#the-syntax-of-device-code)
    + [Compiling Device Code](#compiling-device-code)
    + [Compiling Host Code](#compiling-host-code)
    + [Configuring Drive Parameters](#configuring-drive-parameters)
    + [Executing](#executing)
    + [C Simulation](#c-simulation)
    + [C-RTL Co-Simulation](#c-rtl-co-simulation)

## Papers

* [INSIDER: Designing In-Storage Computing System for Emerging High-Performance Drive](https://www.usenix.org/conference/atc19/presentation/ruan)<br>
Zhenyuan Ruan, Tong He and Jason Cong<br>
2019 USENIX Annual Technical Conference (USENIX ATC'19)

## Restrictions

1. This is the AWS F1 version of Insider system. Since F1 does not open the partial reconfiguration interface to users, this version does not support dynamically reload the in-storage accelerators. Anytime when you wish to switch to a new accelerator, you need to reprogram the whole FPGA.

2. This version does not support simultaneous multiple applications. We have [a separate github repository](https://github.com/zainryan/Insider-multiapp) for that.

## Build from Source
Alternative, you could use our prebuilt aws image (see [Use Prebuilt AWS Image](#use-prebuilt-aws-image)); this will help you save time to build source from scratch. 

### AWS EC2

You typically needs two EC2 instances: 

  1. Used for code compilation and FPGA binary generation. This could be any type of EC2 instance as long as it has enough VCPU and RAM to run Xilinx Vivado for FPGA synthesis. For example, you can choose c4.4xlarge.

  2. Used for running the Insider system. This must be an f1.2xlarge (since some parameters in our repo is hard coded). If you want to try Insider in other types of FPGA instances, please send us an email.

For both instances, you should use the FPGA developer AMI, which can be found at the AWS Marketplace when you launching a new instance. There are two caveats.

  1. Make sure you choose the AMI version as 1.4.0. It could be found at the panel of "FPGA Developer AMI" via `Previous versions -> Continue to Configuration`. The reason behinds this is that the newer version of AMI adopts more recent Vivado HLS tool, which has a significant lower performance on synthesizing some of our kernels.
  
  2. Make sure that you configure the compilation instance to have more than 250 GB storage.
  
In order to save your time, you could follow the instructions below to set up the compilation instance first. After that you could copy the drive volumn (via creating a snapshot and then creating a volumn based on the snapshot in the EC2 web console) to the FPGA instance so that you don't need redo all the steps. 

### AWS FPGA Toolchain

First, you need to clone the latest AWS FPGA repository.
```
[~]$ git clone https://github.com/aws/aws-fpga.git
[~]$ cd aws-fpga; git checkout v1.4.4
```
Next, add the following commands into the rc file of your shell. For example, if you use bash, please append the followings into ~/.bashrc.
```
AWS_FPGA_DIR=The absolute path of your cloned repository
source $AWS_FPGA_DIR/sdk_setup.sh
source $AWS_FPGA_DIR/hdk_setup.sh
```
After that please logout your ssh terminal and re-login. The initialization would then be executed (It would be slow for the first time since it needs to download and build related files). 

### Linux Kernel

To reflect the performance in the latest system, we adapt the Insider drivers to kernel 4.14. However, the default Linux version of AWS FPGA AMI is 3.10.0. You should update the kernel version manually.
```
[~]$ wget http://mirrors.coreix.net/elrepo-archive-archive/kernel/el7/x86_64/RPMS/kernel-ml-4.14.0-1.el7.elrepo.x86_64.rpm
[~]$ sudo yum localinstall kernel-ml-4.14.0-1.el7.elrepo.x86_64.rpm
[~]$ wget http://mirrors.coreix.net/elrepo-archive-archive/kernel/el7/x86_64/RPMS/kernel-ml-headers-4.14.0-1.el7.elrepo.x86_64.rpm
[~]$ sudo yum swap kernel-headers -- kernel-ml-headers-4.14.0-1.el7.elrepo.x86_64.rpm
[~]$ wget http://mirrors.coreix.net/elrepo-archive-archive/kernel/el7/x86_64/RPMS/kernel-ml-devel-4.14.0-1.el7.elrepo.x86_64.rpm
[~]$ sudo yum localinstall --skip-broken kernel-ml-devel-4.14.0-1.el7.elrepo.x86_64.rpm
[~]$ sudo grub2-mkconfig -o /boot/grub2/grub.cfg
[~]$ sudo grub2-set-default 0
```
Finally reboot your system and verify that your kernel version is 4.14.0.
```
[~]$ uname -r
```

### Boost

Install the boost library.
```
$ sudo yum install boost-devel
```

### LLVM and Clang

Some functionality of Insider compiler is implemented based on LLVM and Clang, which should be built first.
```
$ sudo yum install cmake3 svn
$ PATH_TO_LLVM=~/llvm-path; mkdir -p $PATH_TO_LLVM; cd $PATH_TO_LLVM
$ svn checkout http://llvm.org/svn/llvm-project/llvm/trunk@346828 llvm
$ cd llvm/tools; svn co http://llvm.org/svn/llvm-project/cfe/trunk@346828 clang
$ cd $PATH_TO_LLVM; mkdir build; cd build; cmake3 $PATH_TO_LLVM/llvm
$ make -j8 # Replace 8 with the number of cores of your instance, using too many cores may cause linker fail (OoM)
```

### Run The Installation Script

The build and installation of Insider is easy. First, you need to set the environment variable `LLVM_SRC_PATH` to the path of the llvm source, and set `LLVM_BUILD_PATH` to the path of the llvm build folder. After that, clone this repository and execute the `install.sh` script.
```
$ export LLVM_SRC_PATH=$PATH_TO_LLVM/llvm
$ export LLVM_BUILD_PATH=$PATH_TO_LLVM/build
$ export AWS_FPGA_PATH=$AWS_FPGA_DIR
$ ./install.sh
```
Finally, please logout and relogin. 

## Use Prebuilt AWS Image

We provide a prebuilt AWS image whose AMI ID is __ami-04c8ef1dee652fe24__. We have set its permission into public. Please launch instances from __us-east-1__ so you can access the AMI image. You can follow the [official AWS guide](https://docs.aws.amazon.com/cli/latest/userguide/cli-services-ec2-instances.html) to launch your instance using our AMI ID. Please make sure that you set the correct security group which opens the neccesary port (e.g., the ssh port 22 and vnc ports 59XX).

Same as the [AWS EC2](#aws-ec2) section, here you need to use our AMI image to launch two instances: one compilation instance (C4) and one runtime instance (F1). After creating the instances, please clone this repo in the home directory (if there is a pre-existing one, delete it first) and follow the instructions in [Run The Installation Script](#run-the-installation-script).

Please make sure you import a valid Xilinx Vivado 2017.4 license to use this prebuilt image (sorry, but we could not include this due to legal issues). We are not longer able to publish an AMI image with the marketplace license from FPGA Developer AMI. If you would like to try out the INSIDER system using the free license in FPGA Developer AMI, please send your AWS account number to <ucla-vast-insider@dotkr.nl> using your institutional email, and we will add you to the access list of the derived AMI image.

## Folder Organizations
```
Github Repo
 |------- Insider       # The source code of INSIDER system.
 |------- STAccel       # The source code of ST-Accel (on which we built INSIDER)
 |------- apps          # Applications on INSIDER system.
 |------- driver        # Linux kernel drivers for INSIDER.
 |------- fio           # Some FIO (a handy Linux tool) configs used to measure the drive performance.
 |------- patch         # The patch to the original AWS FPGA project.
 |------- install.sh    # The script to install the whole INSIDER system.
 |------- load_image.sh # The script used to load compiled FPGA image.
```

## Applications
```
Applications
 |------- grep         # String matching over the large text.
 |------- integration  # Combine data from multiple sources.
 |------- pt           # No-op, simply receives the drive input and returns it as the output.
 |------- rle          # The bitmap decompression (based on the run-length decoder).
 |------- statistics   # Per row statistical analyses.
 |------- knn          # K-nearest neighobors.
 |------- relief       # A feature selection algorithm --- relief algorithm.
 |------- sql          # A SQL query with select, where, sum, group by and order by.
```

## Usage

### The Syntax of Device Code

The device code in Insider is compatible with Xilinx Vivado HLS (since our backend is built on Xilinx tools). However, you should strictly observe the syntax in Insider when writing device code. We provide six applications in the respository, whose source code are located at apps folder. Their device code are located at `apps/device`. You can refer to their code to learn the syntax. 

The hierarchy of device code (for example, the `grep` application) should be structured like this:
```
grep
 |------- inc
 |------- kernels
 |------- interconnects.cpp
```
   
The device code folder contains three main parts.

1. `inc`. This folder is used for saving user-written header files. It could be empty if user does not need any extra header. 

2. `kernels`. This folder is used for saving user-written sub-kernels. Insider supports modularity, and we encourage user to split their logic into small sub-kernels connecting with Insider queues. (Insider queue can be read or written; due to the potential bug in Xilinx Vivado HLS, please insists using non-blocking read `read_nb` and blocking write `write` to operate Insider queues). Each sub-kernel can only contain one function (whose name is same as its file name), and it must include `<insider_kernel.h>`. Every sub-kernel should be a streaming kernel which observes the following format. 
```
void kernel(
  // Omit args here
) {
  // Define local variables that are alive across different while iterations.
  // Other type of code is not allowed before the while loop.
  while (1) {  // Must declare a while loop here.
    #pragma HLS pipeline  // Pipeline the kernel while loop
    // Put your kernel logic inside the while loop which would be repeatedly invoked.
    // "break" and "continue" is not allowed inside the while loop.
  }
  // No code is allowed after the while loop.
}
```

3. `interconnects.cpp`. This file is used for describing the connection between sub-kernels. User defines Insider queues (and indicates the queue depths) at this file and passes them into sub-kernels. User must include `<insider_itc.h>` and every sub-kernel source file. Note that, there are three special queues: `app_input_data`. `app_output_data`, `app_input_params`. These three queues are defined by Insider framework.

    1. `app_input_data` stores the data read from the drive which is used for the accelerator processing.
 
    2. `app_output_data` stores the output data of the accelerator, which will be sent back to host as the return value of `iread`.
 
    3. `app_input_params` stores the host-sent input parameters. 
 
### Compiling Device Code

Caveat: you should perform all the compilation related stuff at the compilation instance to save your cost (since the FPGA instance is expensive!).

Take `grep` for example, first execute insider device compiler to generate an STAccel project folder.
```
$ cd apps/device/grep
$ insider_device_compiler
```
Next, use STAccel toolchain to generate a AWS-compatible RTL project folder. It will generate a report of the pipeline performance of each kernel. Make sure that every kernel has final II (Initiation Interval) equals to one, which means that the kernel will be reinvoked every clock cycle (and achieve the best performance). Thus, if II equals two, the performance will degrade into the half.
```
$ cd staccel
$ staccel_syn
```
Then, You can simply follow the [standard process in AWS F1](https://github.com/aws/aws-fpga/tree/master/hdk#step2) to continue. Simply invoke the script to trigger the Vivado suite to synthesize the project.
```
$ (cd project/build/scripts; ./aws_build_dcp_from_cl.sh);
```
You can track the progress via tailing the log file. The synthesis usually takes about three hours. 
```
$ tail -f project/build/scripts/last_log
```
After that, update your tarball (locates at project/build/checkpoints/to_aws) into AWS S3 and start AFI creation. This corresponds the [step 3](https://github.com/aws/aws-fpga/tree/master/hdk#step3) in the AWS F1 tutorial above. The device code compilation ends here; you don't need to go beyond step 3 in their tutorial. You can use the following command to check whether the image generation has finished.
```
$ aws ec2 describe-fpga-images --fpga-image-ids AFI_ID
```

### Compiling Host Code

The host code is located at `apps/host`. For each application, we provide two version of host code: the offloading version and the pure-cpu version. Take `grep` for example, you will find `apps/host/grep/src/offload` and `apps/host/grep/src/pure_cpu`. The pure-cpu version can be directly compiled normally via `g++`. The offloading version should be compiler via `insider_host_g++` or `insider_host_gcc` depending whether it's written in C++ or C. For the `grep`  case, you should invoke the following command:

```
$ cd apps/host/grep/src/offload
$ insider_host_g++ -O3 grep.cpp -o grep
```
If you see error message like `relocation truncated to fit:`, please append the compilation flag `-mcmodel=medium`.

### Configuring Drive Parameters

All system parameters are listed at `/driver/nvme/const.h`.

|Parameters|Formats|Comments|
|-----------|---------|--------|
|HOST_READ_THROTTLE_PARAM|((X << 16) \| Y)|Throttles the PCIe read bandwidth (from drive's perspective) <br/> into X / (X + Y) of the original unthrottled value.|
|HOST_WRITE_THROTTLE_PARAM|((X << 16) \| Y)|Ditto, but throttles the write bandwidth.|
|DEVICE_DELAY_CYCLE_CNT|(X)|Instructs X delaying cycles at the internal storage (note that <br/>setting X to zero does not mean the interconnection delay <br/>would be zero since it has the inherent hardware delay).|
|DEVICE_READ_THROTTLE_PARAM|((X << 16) \| Y)|Throttles the internal storage read bandwidth into X / (X + Y)<br/> of the original unthrottled value.|
|DEVICE_WRITE_THROTTLE_PARAM|((X << 16) \| Y)|Ditto, but throttles the write bandwidth.|

For example, you can set the following values:
```
...
#define HOST_READ_THROTTLE_PARAM ((9 << 16) | 13)
...
#define DEVICE_DELAY_CYCLE_CNT (2500)
...
```

### Executing

All the commands in the section should be performed at the FPGA instance, i.e., AWS F1.
First, use `load_image.sh` in this repository to install your previously generated image via its `AGFI_ID`.
```
$ ./load_image.sh AGFI_ID
```
If there's no error message (there will be some log which is fine), you will find a 64GiB-size Insider drive is mounted at `/mnt`. 
Now you can use Linux tools like `fio` to check the drive bandwidth and latency (we have included the fio test configures in this repo). You iteratively tune the drive parameters until they meet your requirement.

Before executing the host program, we first need to prepare the input data (which is served as the raw real file, and the offloading version would create the virtual file based on that). Take `grep` for example, now you can use the data generator provided in `apps/host/grep/data_gen`.
```
$ cd apps/host/grep/data_gen
$ ./compile.sh
$ ./data_gen # This step takes some time since we generate near 60GiB-size input.
```
Now you can run the host program. You can run the offloading version and the pure-cpu version separately to see the performance difference.

### C Simulation

In order to increase the debugging efficiency and version iteration period, we also provide C Simulation (CSIM) and C-RTL Co-Simulation (COSIM) so that you can verify the correctness and functionality of your design without synthesizing and programming your FPGA. In this section, we first introduce CSIM.

Take `grep` for example, first go to its folder of the device code and invoke Insider device compiler.
```
$ cd apps/device/grep
$ insider_device_compiler
```
Then, enter the generated `staccel` folder and invoke STAccel CSIM command.
```
$ cd staccel
$ staccel_csim
```
It will generate a csim command. Now edit `csim/src/interconnects.cpp` to add your CSIM logics. Please invoke your logics at `user_simulation_function` which is at the end of the file. After that invoke `csim_compile.sh` to generate CSIM binary, and finally invoke it to start CSIM.
```
$ cd csim
$ emacs src/interconnects.cpp # Replace emacs with your favorate editor.
$ ./csim_compile.sh
$ ./bin/csim
```
One example for `integration` is included at `device/integration/staccel_csim`. You can compile and run it to see its final effect.

### C-RTL Co-Simulation

In COSIM, we will transform the C/C++ based Insider code into the hardware RTL code to do simulation.　Since COSIM is based on cycle-accurate RTL simulator, which is slow, please refrain from simulating over a large dataset. Generally, the input size should be less than 100 KiB to make the process fast.

Take `grep` for example, first go to its folder of the device code and invoke Insider COSIM toolchain.
```
$ cd apps/device/grep
$ insider_cosim
```
It will generate `cosim` folder. You should edit the COSIM test file to instruct your test logics.
```
$ cd cosim
$ emacs project/software/verif_rtl/src/test_main.c # Replace emacs with your favorate editor.
```
After that, invoke the makefile to start co-simulation.
```
$ cd project/verif/scripts
$ make C_TEST=test_main
```
Wait until finishing executing, you will see the output of your program. If you wish to see the hardware wave to debug, copy `open_waves.tcl` into your simulation output and invoke it.
```
$ cp open_waves.tcl ../sim/test_main/
$ cd !$
$ vivado -source open_waves.tcl # This command should be executed at a GUI environment, e.g., GNOME3 through VNC viewer.
```

One example for `integration` is included at `apps/device/integration/cosim`. You can refer the code too see how to write the COSIM test file.

