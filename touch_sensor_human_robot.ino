#include <Arduino.h>

// touch includes
#include <MPR121.h>
#include <MPR121_Datastream.h>
#include <Wire.h>
#define LED_YELLOW 11

// touch constants
const uint32_t BAUD_RATE = 115200;
const uint8_t MPR121_ADDR = 0x5A;  // 0x5C is the MPR121 I2C address on the Bare Touch Board
const uint8_t MPR121_INT = 4;  // pin 4 is the MPR121 interrupt on the Bare Touch Board
bool isTouch = false;
int count =0;
// MPR121 datastream behaviour constants
const bool MPR121_DATASTREAM_ENABLE = false;

void setup() {
  Serial.begin(BAUD_RATE);
  pinMode(LED_YELLOW,OUTPUT);
  
  pinMode(3, INPUT_PULLUP);
  if (!MPR121.begin(MPR121_ADDR)) {
    Serial.println("error setting up MPR121");
    switch (MPR121.getError()) {
      case NO_ERROR:
        Serial.println("no error");
        break;
      case ADDRESS_UNKNOWN:
        Serial.println("incorrect address");
        break;
      case READBACK_FAIL:
        Serial.println("readback failure");
        break;
      case OVERCURRENT_FLAG:
        Serial.println("overcurrent on REXT pin");
        break;
      case OUT_OF_RANGE:
        Serial.println("electrode out of range");
        break;
      case NOT_INITED:
        Serial.println("not initialised");
        break;
      default:
        Serial.println("unknown error");
        break;
    }
    while (1);
  }
  MPR121_settings_type newSettings;
  newSettings.MHDR = 0x15;
  newSettings.NHDR = 0x01;
  newSettings.NCLR = 0x03;
  newSettings.FDLR = 0x15;

  newSettings.MHDF = 0x15;
  newSettings.NHDF = 0x01;
  newSettings.NCLF = 0x03;
  newSettings.FDLF = 0x15;

  newSettings.NHDT = 0x01;
  newSettings.NCLT = 0x10;
  newSettings.FDLT = 0xFF;
  MPR121.applySettings(&newSettings);
  MPR121.setInterruptPin(MPR121_INT);

  if (MPR121_DATASTREAM_ENABLE) {
    MPR121.restoreSavedThresholds();
    MPR121_Datastream.begin(&Serial);
  } else {
    MPR121.setTouchThreshold(60);  // this is the touch threshold - setting it low makes it more like a proximity trigger, default value is 40 for touch
    MPR121.setReleaseThreshold(50);  // this is the release threshold - must ALWAYS be smaller than the touch threshold, default value is 20 for touch
  }

  MPR121.setFFI(FFI_18);
  MPR121.setSFI(SFI_18);
  MPR121.setGlobalCDT(CDT_8US);  // reasonable for larger capacitances
  
  digitalWrite(LED_YELLOW, HIGH);  // switch on user LED while auto calibrating electrodes
  delay(1000);
  MPR121.autoSetElectrodes();  // autoset all electrode settings
  digitalWrite(LED_YELLOW, LOW);
  Serial.println("setup touch");
}

uint32_t lastAutoset = 0;
void loop() {
  count++;
  MPR121.updateAll();

  // for (int i = 0; i < 1; i++) {
  //   if (MPR121.isNewTouch(i)) {
  //     Serial.print("electrode ");
  //     Serial.print(i, DEC);
  //     Serial.println(" was just touched");
  //   } else if (MPR121.isNewRelease(i)) {
  //     Serial.print("electrode ");
  //     Serial.print(i, DEC);
  //     Serial.println(" was just released");
  //   }
  // }

  if (MPR121_DATASTREAM_ENABLE) {
    MPR121_Datastream.update();
  }
  if (MPR121.isNewTouch(0))
  {
    Serial.println("C touch ");
    digitalWrite(LED_YELLOW, HIGH);  // switch on user LED while auto calibrating electrodes
    delay(500);
    isTouch = true;
  }
  else if (MPR121.isNewRelease(0)){
    Serial.println("C release ");
    isTouch = false;
    digitalWrite(LED_YELLOW, LOW);  // switch on user LED while auto calibrating electrodes
  }
  // if (MPR121.getFilteredData(0) < 600)
  // {
  //   isTouch = true;
  // }
  // if (MPR121.getFilteredData(0) > 800)
  // {
  //   isTouch = false;
  // }
  // isTouch = MPR121.getFilteredData(0);
  // 
  // if(MPR121.getFilteredData(0) <MPR121.getBaselineData(0)-60)
  // {
  //   Serial.println("C touch ");
  //   digitalWrite(LED_YELLOW, HIGH);  // switch on user LED while auto calibrating electrodes
  //   delay(500);
  //   isTouch = true;
  // }
  // else if(MPR121.getFilteredData(0) > MPR121.getBaselineData(0)-50)
  // {
  //   Serial.println("C release ");
  //   isTouch = false;
  //   digitalWrite(LED_YELLOW, LOW);
  // }

  if(count>6)
  {
    count=0;
    Serial.print(MPR121.getFilteredData(0));
  Serial.print(", ");
  MPR121.updateBaselineData();
  Serial.println(MPR121.getBaselineData(0));
  }
  
  // Serial.print(", ");
  // Serial.print(isTouch);
  // Serial.print(", ");
  // Serial.println(!digitalRead(3));
  
  delay(300);
  // if (millis() - lastAutoset > 5000)
  // {
  //   lastAutoset = millis();
  // MPR121.autoSetElectrodes();
  // }
}
