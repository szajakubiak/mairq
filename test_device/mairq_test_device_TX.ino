// Libraries used
#include <SPI.h>
#include <Wire.h>
#include <RH_RF95.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SoftwareSerial.h>

// Define LoRa pins
#define RFM95_CS 1
#define RFM95_RST 4
#define RFM95_INT 7

// Set LoRa freq (434.0 or 915.0)
#define RF95_FREQ 433.8

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Instance of the temperature and pressure sensor
Adafruit_BMP280 bme; // I2C

// Serial port for PMS5003 PM sensor
SoftwareSerial pmsSerial(10, 9);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);
  pmsSerial.begin(9600);
  delay(100);
  
  // Transmitter node is usually used without open serial connection
  // but following line can be uncommented to see error messages at startup
  //while (!Serial);

  Serial.println("LoRa TX node with Bosch BMP280 sensor");

  // Manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK");

  if (!bme.begin(0x76)) {  
    Serial.println("Bosch BMP280 sensor init failed");
    while (1);
  }
  Serial.println("Bosch BMP280 sensor init OK");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(12, false);

  // Send PMS5003 to sleep
  Serial.println("Put PMS5003 To Sleep");
  uint8_t commandSleep[] = { 0x42, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73 };
  delay(2500); // PMS5003 needs some time to become responsive
  pmsSerial.write(commandSleep, sizeof(commandSleep));
}

int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  
  Serial.println("Sending to rf95_server");
  // Send a message to rf95_server
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100;
  String message;
  message += String(packetnum);
  packetnum += 1;
  message += F(",");
  message += String(temperature, 1);
  message += F(",");
  message += String(pressure, 0);
  
  int messageLength = message.length() + 1;
  char messageBuffer[messageLength];
  message.toCharArray(messageBuffer, messageLength);
  
  Serial.print("Sending "); Serial.println(messageBuffer);
  Serial.println("Sending..."); delay(10);
  rf95.send(messageBuffer, messageLength);

  Serial.println("Waiting for packet to complete..."); delay(10);
  rf95.waitPacketSent();
  
  // Wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  Serial.println("Waiting for reply..."); delay(10);
  if (rf95.waitAvailableTimeout(1000)) { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len)) {
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
    }
    else {
      Serial.println("Receive failed");
    }
  }
  else {
    Serial.println("No reply received");
  }
  
  digitalWrite(LED_BUILTIN, LOW);
  
  delay(10000);
}
    
