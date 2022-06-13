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

* Raspberry Pi for data reception
