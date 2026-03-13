# Introduction
This repository consists of source code made as an exercise during my MSc. It was made to monitor elderly people's activities for basic movements and report any dangerous recorded activity, like falling. It was made on an STM IoT node, DISCO_L475VG_IOT01A, leveraging the MBED-OS platform.

Although the project was created for the MSc, I think the instructions below would be a nice guide for setting up the Mbed development environment locally, without depending on ARM-provided solutions like KEIL. If you would like to set up the Mbed environment locally but you are struggling to do so, I would be happy to answer any questions. Consider either sending me an email (**constarg@pm.me**) or, preferably, opening an issue in the current repository.

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
Before proceeding with downloading the source code, one more dependency must be installed. That is the mbed-tools.
Follow the procedure below to install the mbed-tools.

```bash
python3 -m venv .env
source .env/bin/activate
python3 -m pip install embed-tools
```

## ARM tool chain
Also, the project was compiled and tested using gcc-arm-11.2-2022; please consider using this.<br>

**IMPORTANT: If you do not have ANY ARM-compatible compiler on your system, you must install one; otherwise, this project won't compile, as the STM platform uses an ARM processor.**<br>

To download the ARM toolchain I was using for the purpose of this project, please download the following from the ARM repositories using the following command (or navigate to the site: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads).

```bash
wget https://developer.arm.com/-/media/files/downloads/gnu/11.2-2022.02/binrel/gcc-arm-11.2-2022.02-aarch64-arm-none-eabi.tar.xz?rev=3789ce184378489e9704981b07530206&revision=3789ce18-4378-489e-9704-981b07530206&hash=098A6C1606DEFEC8430936F382FB453E -o gcc-arm-11.2-2022.02-aarch64-arm-none-eabi.tar.xz
```
```bash
tar -xf gcc-arm-none-eabi-12.3.rel1-x86_64-arm-none-eabi.tar.xz
```
```bash
export PATH="PATH_TO_DOWNLOAD_FOLDER/gcc-arm-11.2-2022.02-x86_64-arm-none-eabi/bin:PATH"
```

## Drivers 
The last step before proceeding with the compilation of the project is to set up the required drivers. These drivers were delivered from the University and, unfortunately, they have to be placed manually.
Please navigate to the releases on GitHub (link) and download the BSP drivers from there. The BSP drivers, AS IS, must be placed under the drivers folder to make the project compile.<br><br>
However, I will provide the links to the respective GitHub repositories for each driver in case you are interested.

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

After installing the required packages and placing the drivers in the correct place, follow the procedure below to download and compile the source code (while being in the Python env).

```bash
git clone --recursive https://github.com/constarg/health-monitoring-iot.git
cd health-monitoring-iot
pip install -r mbed-os/requirements.txt
mbed-tools configure -m DISCO_L475VG_IOT01A -t GCC_ARM -o ./build --mbed-os-path ./mbed-os
cd build/
cmake ../
make
```

## Flash and test

## Flash

To flash the software binary version into the internal memory of the STM device execute the following command, under the build folder.
```bash
make flash 
```

## Testing

For functional verification of the software, please navigate to the releases and download the Matlab app, which is used as a demo for
the software. The screenshot below depicts the software functionality while running. <br>
<img width="765" height="451" alt="image" src="https://github.com/user-attachments/assets/07a54155-2d54-486c-9210-d2b7149dc352" /><br><br>

**IMPORTANT: The device was calibrated based on the DISCO_L475VG_IOT01A I tested it on. Therefore, due to small fluctuations and differences between each device, it MAY NOT work on another device if not calibrated correctly.**
