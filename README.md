# Introduction
This repository conists of source code made as an exercise in the MSc. It was made to monitor elder people's activities for basic movements and report any dangerous recorded activity, like falling. It as made on an STM IoT node, DISCO_L475VG_IOT01A, levereging the MBED-OS platform.

# Installation Requirements
Before compiling, please install the following packages, depending on your setup.

## Debian-based
```bash
sudo apt-get install build-essential cmake git gcc
```

## RHEL-based
```bash
sudo dnf group install c-development
sudo dnf group install "development-tools"
sudo dnf install stlink cmake
```

# Setup 

## Embed-tools
Before procedding in downloading the source code, one more dependency is to be installed. That is the embed-tools. 
Follow the procedure below to install the embed-tools. 

```bash
python3 -m venv .env
source .env/bin/activate
python3 -m pip install embed-tools
```

## ARM tool chain
Also, the project were compiled and tested using the **gcc-arm-11.2-2022**, please consider using this.<br><br>
**IMPORTANT: If you do not have ANY ARM compitible compiler in your system, you must install one, otherwise this project won't compile, as the STM platform, uses ARM processor.**<br><br>
To download the ARM tool chain i was using for the purpose of this project, please download the following, from the ARM respositories using the following command (or navigate to the site: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads).

```bash
wget https://developer.arm.com/-/media/files/downloads/gnu/11.2-2022.02/binrel/gcc-arm-11.2-2022.02-aarch64-arm-none-eabi.tar.xz?rev=3789ce184378489e9704981b07530206&revision=3789ce18-4378-489e-9704-981b07530206&hash=098A6C1606DEFEC8430936F382FB453E -o gcc-arm-11.2-2022.02-aarch64-arm-none-eabi.tar.xz
tar -xf gcc-arm-none-eabi-12.3.rel1-x86_64-arm-none-eabi.tar.xz
export PATH="PATH_TO_DOWNLOAD_FOLDER/gcc-arm-11.2-2022.02-x86_64-arm-none-eabi/bin:PATH"
```

## Drivers 
The last step, before proceeding on the compilation of the project is to set up the required drivers. These drivers were delivared from the University and unfortunatly it had to be placed manually. 
Please navigate to the releases of GitHub (link) and download the **BSP** drivers from there. The BSP drivers AS IT IS, must be placed under **drivers** folder, to make the project compile. <br>
However, i will provide the links to the respective GitHub repositories for each driver, in case you are interested. 

### BSP Drivers
```bash
https://github.com/STMicroelectronics/b-l475e-iot01a-bsp
```

### Components
```bash
https://github.com/STMicroelectronics/stm32-bsp-common
https://github.com/STMicroelectronics/stm32-hts221
https://github.com/STMicroelectronics/stm32-lis3mdl
https://github.com/STMicroelectronics/stm32-lps22hb
https://github.com/STMicroelectronics/stm32-lsm6dsl
https://github.com/STMicroelectronics/stm32-mx25r6435f
```

## Compile 

After installing the required packagkes and placing the drivers in the correct place, follow the procedure below to download and compile the source code (while being in the python env).

```bash
git clone --recursive https://github.com/constarg/health-monitoring-iot.git
cd health-monitoring-iot
pip install -r mbed-os/requirements.txt
mbed-tools configure -m DISCO_L475VG_IOT01A -t GCC_ARM -o ./build --mbed-os-path ./mbed-os
cd build/
cmake ../
make
```
