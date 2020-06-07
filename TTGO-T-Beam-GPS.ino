#include <Wire.h>
#include <axp20x.h>
#include <TinyGPS++.h>

#define UBLOX_GPS_OBJECT()  TinyGPSPlus gps
#define GPS_BANUD_RATE 9600
#define GPS_RX_PIN 34
#define GPS_TX_PIN 12

#define I2C_SDA 21
#define I2C_SCL 22
#define AXP192_SLAVE_ADDRESS 0x34

UBLOX_GPS_OBJECT();

AXP20X_Class axp;

bool axp192_found = false;

char buff[5][256];
uint64_t gpsSec = 0;

void scanI2Cdevice(void)
{
    byte err, addr;
    int nDevices = 0;
    for (addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        err = Wire.endTransmission();
        if (err == 0) {
            Serial.print("I2C device found at address 0x");
            if (addr < 16)
                Serial.print("0");
            Serial.print(addr, HEX);
            Serial.println(" !");
            nDevices++;

            if (addr == AXP192_SLAVE_ADDRESS) {
                axp192_found = true;
                Serial.println("axp192 PMU found");
            }
        } else if (err == 4) {
            Serial.print("Unknow error at address 0x");
            if (addr < 16)
                Serial.print("0");
            Serial.println(addr, HEX);
        }
    }
    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");
}

void setup() {
  Serial.begin(115200);

  delay(5000);
  
  Wire.begin(I2C_SDA, I2C_SCL);
  
  scanI2Cdevice();
  
  if (axp192_found) {
      if (!axp.begin(Wire, AXP192_SLAVE_ADDRESS)) {
          Serial.println("AXP192 Begin PASS");
          // power on ESP32 & GPS
          axp.setPowerOutPut(AXP192_LDO3, AXP202_ON);
          axp.setPowerOutPut(AXP192_DCDC1, AXP202_ON);
          axp.setDCDC1Voltage(3300);  //esp32 core VDD    3v3
          axp.setLDO3Voltage(3300);   //GPS VDD      3v3

          Serial.printf("DCDC1: %s\n", axp.isDCDC1Enable() ? "ENABLE" : "DISABLE");
          Serial.printf("DCDC2: %s\n", axp.isDCDC2Enable() ? "ENABLE" : "DISABLE");
          Serial.printf("LDO2: %s\n", axp.isLDO2Enable() ? "ENABLE" : "DISABLE");
          Serial.printf("LDO3: %s\n", axp.isLDO3Enable() ? "ENABLE" : "DISABLE");
          Serial.printf("DCDC3: %s\n", axp.isDCDC3Enable() ? "ENABLE" : "DISABLE");
          Serial.printf("Exten: %s\n", axp.isExtenEnable() ? "ENABLE" : "DISABLE");
               
          // Set mode of blue onboard LED (OFF, ON, Blinking 1Hz, Blinking 4 Hz)
          // axp.setChgLEDMode(AXP20X_LED_OFF);
          //axp.setChgLEDMode(AXP20X_LED_LOW_LEVEL);
          axp.setChgLEDMode(AXP20X_LED_BLINK_1HZ);
          //axp.setChgLEDMode(AXP20X_LED_BLINK_4HZ);
      } else {
          Serial.println("AXP192 Begin FAIL");
      }
      
  } else {
      Serial.println("AXP192 not found");
  }
      
  Serial1.begin(GPS_BANUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
}

void loop() {
  // put your main code here, to run repeatedly
  static uint64_t gpsMap = 0;

  while (Serial1.available())
  gps.encode(Serial1.read());

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    snprintf(buff[0], sizeof(buff[0]), "T-Beam GPS");
    snprintf(buff[1], sizeof(buff[1]), "No GPS detected");
    Serial.println(buff[1]);
    return;
  }
  if (!gps.location.isValid()) {
    if (millis() - gpsMap > 1000) {
        snprintf(buff[0], sizeof(buff[0]), "T-Beam GPS");
        snprintf(buff[1], sizeof(buff[1]), "Positioning(%llu)", gpsSec++);
        Serial.println(buff[1]);
        gpsMap = millis();
    }
  } else {
    if (millis() - gpsMap > 1000) {
        snprintf(buff[0], sizeof(buff[0]), "UTC:%d:%d:%d", gps.time.hour(), gps.time.minute(), gps.time.second());
        snprintf(buff[1], sizeof(buff[1]), "LNG:%.4f", gps.location.lng());
        snprintf(buff[2], sizeof(buff[2]), "LAT:%.4f", gps.location.lat());
        snprintf(buff[3], sizeof(buff[3]), "satellites:%u", gps.satellites.value());
        Serial.println(buff[0]);
        Serial.println(buff[1]);
        Serial.println(buff[2]);
        Serial.println(buff[3]);
        gpsMap = millis();
    }
  }
}
