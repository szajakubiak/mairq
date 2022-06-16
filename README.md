# mairq
 **M**obile **AIR** **Q**uality measuring device

## Concept
Device should be capable of measuring basic air quality and environmental parameters for at least one day on a single battery charging. Results should be transmitted via LoRa radio to the receiver (Raspberry Pi with LoRa module) immediately after measurement or as soon as there will be active connection.

## Hardware
* Adafruit Feather RP2040 - battery management, STEMMA QT / Qwiic connector, support for Arduino and CircuitPython

* Sensirion SPS30 particulate matter sensor (UART / I2C)

* Bosch Sensortec BME280 temperature, relative humidity and pressure sensor (I2C)

* GPS unit (UART) to determine localization and time of the measurement

* Accelerometer (I2C)

* LoRa radio module operating at 433 MHz (SPI)

* Step-up converter to generate 5 V power supply from battery for SPS30

* Servo to open inlets and outlets to the case

* Fan to circulate air inside the case

* Raspberry Pi or Wi-Fi capable microcontroller for data reception

## Raspberry Pi setup

1. Start with a fresh installation of the Raspberr Pi OS Lite and establlish SSH connection.

2. Activate SPI interface using `raspi-config`.

3. Install PIP:

`sudo apt install python3-pip`

4. Install dependencies using PIP:

`pip install RPi.GPIO spidev pyLoRa`

## Test device

* Microcontroller: clone of the Adafruit Feather 32u4

* BMP280 sensor (I2C address 0x76)

* SHT30 (I2C address 0x44)

* MPU-9250 (I2C address 0x68)

* Plantower PMS5003 PM sensor (UART)

## Links
[LoRa communication between Raspberry Pi and Arduino](https://circuitdigest.com/microcontroller-projects/raspberry-pi-with-lora-peer-to-peer-communication-with-arduino)

[Interfacing PMS5003 with Arduino](https://forums.adafruit.com/viewtopic.php?t=167487)

[LoRa docs](https://lora.readthedocs.io/en/latest/)

[PMS5003 library](https://github.com/jbanaszczyk/pms5003)

[PMS5003 datasheet](https://www.digikey.jp/htmldatasheets/production/2903006/0/0/1/pms5003-series-manual.html)

[Interfacing L80-M39 GPS module with Arduino](https://how2electronics.com/how-to-interface-quectel-l80-gps-module-with-arduino/)