# Firmware v5

<p align="middle">
    <img alt="display Nextion" height="250" src="resources/HMI.png"/>
    <img alt="display LCD backup" height="250" src="resources/HMI_backup.jpg"/>
</p>

<p align="middle">
    <img alt="ECU Front case" height="250" src="resources/ECU_Front.png"/>
    <img alt="telemetry" height="250" src="resources/telemetry.jpg"/>
</p>

# Embedded system workflow

![workflow](resources/workflow.jpg)

# System requirements

* VS code ESP-IDF extension
* ESP-IDF v5.1.2

# Bug fixes

## vs code config folder

If there are no config files in /.vscode folder or there's only one file, delete the /.vscode folder and use the following command to create the new config folder properly:

```
Crtl + Shift + P
```
```
ESP-IDF: Add .vscode Configuration Folde
```

## code highlighting

Add this following line to **.vscode/c_cpp_properties.json** before **"includePath"**

```
"compileCommands": "${workspaceFolder}/build/compile_commands.json",
```

## flashing error

Run the following commands on the terminal to give the user permission to access the USB device:

```
sudo adduser <username> dialout
```
and

```
sudo chmod a+rw /dev/ttyUSB*
```

# Configurations

## LoRa

### CR - Coding Rate

Coding rate refers to the ratio of added redundant bits to the total number of bits transmitted. It affects the robustness of the communication link against noise and interference. Lower coding rates provide better error correction capabilities at the cost of reduced throughput.

* **CR = 5 (4/5):** For every 5 bits transmitted, 4 bits are information bits, and 1 bit is a redundant (error-correcting) bit. It provides good error correction capabilities at the cost of slightly reduced throughput.

* **CR = 6 (4/6):** In this case, for every 6 bits transmitted, 4 bits are information bits, and 2 bits are redundant bits. It offers slightly less error correction than 4/5 but allows for higher throughput.

* **CR = 7 (4/7):** This coding rate configuration means that for every 7 bits transmitted, 4 bits are information bits, and 3 bits are redundant bits. It offers even higher throughput than 4/6 but sacrifices some error correction capability.

* **CR = 8 (4/8):** This coding rate configuration means that for every 8 bits transmitted, 4 bits are information bits, and 4 bits are redundant bits. It provides the highest throughput but sacrifices significant error correction capability compared to lower coding rates.

### BW - Signal Bandwidth

Higher values represent wider bandwidths, which can result in higher data rates but may reduce the sensitivity and range of communication. Conversely, lower values represent narrower bandwidths, which can improve sensitivity and range but may limit the achievable data rate.

* **0:** Narrowest bandwidth, suitable for long-range communication but with lower data rates.

* **9:** Widest bandwidth, suitable for high data rate communication but with reduced range.

### SF - Spreading Factor

Spreading factor determines how much the data is spread in the frequency domain. Higher spreading factors result in longer transmission times but increased robustness and range. Lower spreading factors allow for higher data rates but with reduced range and sensitivity to noise. Common values for spreading factor are typically between 6 and 12.
