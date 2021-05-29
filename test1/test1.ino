
#include <Adafruit_ADS1015.h>
Adafruit_ADS1115 ads(0x48);
int16_t i,k,t;


void setup() {
Wire.begin(12, 14);
Serial.begin(9600);
ads.begin();

}

void loop() {
  //i = ads.readADC_SingleEnded(0);
  //k = ads.readADC_SingleEnded(1);
  t = ads.readADC_SingleEnded(2);
  
  //Serial.println(i+500);
   //Serial.println(k -500);
    Serial.println(t);
  //delay(100);                     //Modifying or removing the delay will change the way the signal is shown 
                                  //set it until you get the correct sinewave shap

}
