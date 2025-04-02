# mairq
 **M**obile **AIR** **Q**uality measuring device

## Concept
Device should be capable of measuring basic air quality and environmental parameters for at least one day on a single battery charging. Results should be transmitted via LoRa radio to the receiver (Raspberry Pi or Wi-Fi capable microcontroller connected to the LoRa module) immediately after measurement or as soon as there will be active connection. Results which can't be transmitted immediately should be store in the internal memory untill confirming connection. Two different modes of conducting measurements should be implemented:
* constant delay between two consecutive measurements
* trigger measurement when a certain condition is met, e.g. when device is not moving

## Hardware
* Adafruit Feather RP2040 - battery management, STEMMA QT / Qwiic connector, support for Arduino and CircuitPython
* Sensirion SPS30 particulate matter sensor (UART / I2C)
* Bosch Sensortec BME280 temperature, relative humidity and pressure sensor (I2C)
* L80-M39 GPS module (UART) to determine localization and time of the measurement
* MPU-6050 accelerometer (I2C) to detect when carrier on which device is mounted is stationary
* LoRa radio module (SPI)
* Step-up converter to generate 5 V power supply from the battery to power the SPS30
* Fan to circulate air inside the case
* Raspberry Pi or Wi-Fi capable microcontroller for data reception

## Raspberry Pi setup
1. Start with a fresh installation of the Raspberr Pi OS Lite with SSH connection enabled.

2. Activate SPI interface:
``` bash
raspi-config
```
Select **Interface Options**, then **SPI** and **Yes**.

3. Install Python with additional packages:
``` bash
sudo apt install python3 python3-pip python3-venv git
```

4. Create and activate virtual environment:
``` bash
python3 -m venv mairqvenv
source mairqvenv/bin/activate
```

5. Clone the code repository:
``` bash
git clone https://github.com:szajakubiak/mairq.git
cd mairq
```

6. Install dependencies using PIP:
``` bash
pip install -r requrements.txt
```

## Connecting LoRa module to Raspberry Pi
Suggested connections:

| LoRa   | Raspberry Pi |
| :----: | :----------: |
| SCK    | GPIO 11      |
| MOSI   | GPIO 10      |
| MISO   | GPIO 9       |
| NSS    | GPIO 7       |
| RST    | GPIO 22      |
| DIO0   | GPIO 3       |

## Test device
Before starting this project test device was build with the following specification:

* Microcontroller: clone of the Adafruit Feather 32u4 with LoRa module (433 MHz version)
* L80-M39 GPS module (UART)
* BMP280 sensor (I2C address 0x76)
* SHT30 (I2C address 0x44)
* MPU-9250 (I2C address 0x68)
* Plantower PMS5003 PM sensor (UART)

## Links
[LoRa communication between Raspberry Pi and Arduino](https://circuitdigest.com/microcontroller-projects/raspberry-pi-with-lora-peer-to-peer-communication-with-arduino)

[LoRa and Raspberry Pi](https://learn.adafruit.com/lora-and-lorawan-radio-for-raspberry-pi)

[Interfacing PMS5003 with Arduino](https://forums.adafruit.com/viewtopic.php?t=167487)

[LoRa docs](https://lora.readthedocs.io/en/latest/)

[PMS5003 library](https://github.com/jbanaszczyk/pms5003)

[PMS5003 datasheet](https://www.digikey.jp/htmldatasheets/production/2903006/0/0/1/pms5003-series-manual.html)

[Interfacing L80-M39 GPS module with Arduino](https://how2electronics.com/how-to-interface-quectel-l80-gps-module-with-arduino/)

[LoRa repeater](http://www.daveakerman.com/?p=2469)

## TODO
* Consider upgrade to RP2350
* Design and test LoRa repeater or consider using Meshtastic for communication
* Build device using final hardware
* Implement data encryption for transmission
* Implement checksum to verify data integrity
* LoRa frequency setting as a variable for easy configuration
* Design enclosure suitable for mobile application
