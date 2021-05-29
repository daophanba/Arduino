#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);
Adafruit_ADS1115 ads(0x48);

SimpleKalmanFilter bo_loc(1, 1, 0.2);

float Voltage = 0.0;

void setup(void) {

  Serial.begin(9600);
  lcd.begin();

  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("Hello, world!");
  ads.begin();

}

void loop(void) {

int16_t adc0; // we read from the ADC, we have a sixteen bit integer as a result

adc0 = ads.readADC_SingleEnded(0);

Voltage =  (adc0 * 0.1875)/1000;


Serial.print("AIN0: ");

Serial.print(adc0);

lcd.setCursor(0,0);

lcd.print(adc0);
lcd.print("  ");
lcd.print(Voltage, 7);
Serial.print("\tVoltage: ");

Serial.println(Voltage, 7);

Serial.println();


delay(1000);


}
