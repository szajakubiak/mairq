// Libraries used
#include <SPI.h>
#include <Wire.h>
#include <RH_RF95.h>
#include <TinyGPSPlus.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SHT31.h>
#include <Adafruit_BMP280.h>
#include <SoftwareSerial.h>


// Define LoRa pins
#define RFM95_CS 1
#define RFM95_RST 4
#define RFM95_INT 7

// Set LoRa freq
#define RF95_FREQ 433.8

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Instance of the temperature and relative humidity sensor
Adafruit_SHT31 sht30 = Adafruit_SHT31();

// Instance of the temperature and pressure sensor
Adafruit_BMP280 bmp; // I2C

// Instance of the TinyGPSPlus object
TinyGPSPlus gps;

// Serial port for PMS5003 PM sensor
SoftwareSerial pmsSerial(10, 9);

// Serial port for GPS module
SoftwareSerial gpsSerial(11, 5);

// Packet counter
uint32_t packetnum = 0;

// In development flag
bool inDev = true;

// A sample NMEA stream to develop app without the need
// for beinging locked to satellites
const char *gpsStream =
  "$GPRMC,045103.000,A,3014.1984,N,09749.2872,W,0.67,161.46,030913,,,A*7C\r\n";


// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (gpsSerial.available())
      gps.encode(gpsSerial.read());
  } while (millis() - start < ms);
}


bool gpsValid() {
  if (gps.location.isValid() && gps.date.isValid() && gps.time.isValid()) {
    return true;
  }
  else {
    return false;
  }
}


char * getGPS() {
  char locLat[11];
  //dtostrf(float value, min. width, decimal places, where to store)
  dtostrf(gps.location.lat(), 3, 6, locLat);
  char locLng[12];
  dtostrf(gps.location.lng(), 3, 6, locLng);
  char dateYear[5];
  dtostrf(gps.date.year(), 4, 0, dateYear);
  char dateMonth[3];
  dtostrf(gps.date.month(), 1, 0, dateMonth);
  char dateDay[3];
  dtostrf(gps.date.day(), 1, 0, dateDay);
  char timeHour[3];
  dtostrf(gps.time.hour(), 1, 0, timeHour);
  char timeMinute[3];
  dtostrf(gps.time.minute(), 1, 0, timeMinute);
  char timeSecond[3];
  dtostrf(gps.time.second(), 1, 0, timeSecond);

  char * results = (char *) malloc (45);
  strcpy(results, locLat);
  strcat(results, ",");
  strcat(results, locLng);
  strcat(results, ",");
  strcat(results, dateYear);
  strcat(results, "-");
  strcat(results, dateMonth);
  strcat(results, "-");
  strcat(results, dateDay);
  strcat(results, ",");
  strcat(results, timeHour);
  strcat(results, ":");
  strcat(results, timeMinute);
  strcat(results, ":");
  strcat(results, timeSecond);

  return results;
}


char * getSHT30() {
  char temperature[6];
  //dtostrf(float value, min. width, decimal places, where to store)
  dtostrf(sht30.readTemperature(), 3, 1, temperature);
  char humidity[5];
  dtostrf(sht30.readHumidity(), 3, 1, humidity);

  char * results = (char *) malloc (11);
  strcpy(results, temperature);
  strcat(results, ",");
  strcat(results, humidity);

  return results;
}


char * getBMP280() {
  char temperature[6];
  //dtostrf(float value, min. width, decimal places, where to store)
  dtostrf(bmp.readTemperature(), 3, 1, temperature);
  char pressure[5];
  dtostrf(bmp.readPressure() / 100, 3, 0, pressure);

  char * results = (char *) malloc (10);
  strcpy(results, temperature);
  strcat(results, ",");
  strcat(results, pressure);

  return results;
}


void sendData() {
  digitalWrite(LED_BUILTIN, HIGH);
  
  Serial.println("Sending to rf95_server");
  // Send a message to rf95_server
  char message[74] = "";

  // Add packet number
  char packetnumchar[11];
  itoa(packetnum, packetnumchar, 10);
  strcpy(message, packetnumchar);
  packetnum += 1;

  strcat(message, ",");

  // Add GPS
  char * fromGPS = getGPS();
  strcat(message, fromGPS);
  free(fromGPS);

  strcat(message, ",");

  // Add SHT30
  char * fromSHT30 = getSHT30();
  strcat(message, fromSHT30);
  free(fromSHT30);

  strcat(message, ",");
  
  // Add BMP280
  char * fromBMP280 = getBMP280();
  strcat(message, fromBMP280);
  free(fromBMP280);
  
  Serial.print("Sending "); Serial.println(message);
  Serial.println("Sending..."); delay(10);
  rf95.send(message, sizeof(message));

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
}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);
  pmsSerial.begin(9600);
  gpsSerial.begin(9600);
  delay(100);

  if (inDev) {
    while (!Serial) delay(1);
  }

  // Manual reset of LoRa module
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1) delay(1);
  }
  Serial.println("LoRa radio init OK");

  if (! sht30.begin(0x44)) {
    Serial.println("SHT30 init failed");
    while (1) delay(1);
  }
  Serial.println("SHT30 sensor init OK");
  
  if (!bmp.begin(0x76)) {  
    Serial.println("Bosch BMP280 sensor init failed");
    while (1) delay(1);
  }
  Serial.println("BMP280 sensor init OK");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1) delay(1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(12, false);

  if (inDev) {
    while (*gpsStream)
      gps.encode(*gpsStream++);
  }

  // Send PMS5003 to sleep
  Serial.println("Put PMS5003 To Sleep");
  uint8_t commandSleep[] = { 0x42, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73 };
  delay(2500); // PMS5003 needs some time to become responsive
  pmsSerial.write(commandSleep, sizeof(commandSleep));
}


void loop() {
  if (gpsValid()) {
    sendData();
    if (inDev) {
      delay(10000);
    }
    else {
      smartDelay(10000);
    }
  }
  else {
    Serial.println("No valid GPS data");
    if (inDev) {
      delay(1000);
    }
    else {
      smartDelay(1000);
    }
  }
}
