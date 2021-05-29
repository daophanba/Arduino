#include <LiquidCrystal_I2C.h>

#include <Wire.h>
#include <Adafruit_ADS1015.h>

//SimpleKalmanFilter bo_loc(1, 1, 0.2);
Adafruit_ADS1115 ads(0x48);
LiquidCrystal_I2C lcd(0x3F, 16, 2);

float voltage = 0.0;

int16_t adc0;

float voltage2 = 0.0;

int16_t adc2;

int16_t adc1;

float voltage1 = 0.0;

void setup() {

Serial.begin(9600);
lcd.begin();


ads.begin();

}

int mVperAmp = 185; // use 185 for 5A, 100 for 20A Module and 66 for 30A Module

double Voltage = 0.0;
double VRMS = 0.0;
double AmpsRMS = 0.0;


 


 

void loop() {

int16_t adc0; // we read from the ADC, we have a sixteen bit integer as a result

adc0 = ads.readADC_SingleEnded(0);
adc2 = ads.readADC_SingleEnded(2);
adc1 = ads.readADC_SingleEnded(1);
voltage2 =  (adc2 * 0.1875)/1000;
voltage1 =  (adc1 * 0.1875)/1000;
voltage =  (adc0 * 0.1875)/1000;

Serial.print("AIN0: ");

Serial.print(adc0);

Serial.print("\tVoltage: ");

Serial.println(voltage, 7);



Serial.print("AIN2: ");

Serial.print(adc2);

Serial.print("\tVoltage2: ");

Serial.println(voltage2, 7);

Serial.println();


Voltage = getVPP();
 VRMS = (Voltage/2.0) *(1/sqrt(2)) ; // sq root
 AmpsRMS = ((VRMS * 1000)/mVperAmp);

 //if((AmpsRMS > -0.015) && (AmpsRMS < 0.008)){  // remove low end chatter
   // AmpsRMS = 0.0;
   //}
 //AmpsRMS = AmpsRMS * 1.3;
 
 //float Vo_K = bo_loc.updateEstimate(AmpsRMS);
 //AmpsRMS = Vo_K * 1.4;
 
 
 double Wattage = (230*AmpsRMS); //Observed 18-20 Watt when no load was connected, so substracting offset value to get real consumption.
 //double Wattage_1 = (Wattage+(2/100)*Wattage);
 Serial.print(AmpsRMS,7);
 Serial.println(" Amps RMS ");
 Serial.print(Wattage,7); 
 Serial.println(" Watt ");
 //Serial.println(" Watt ");
 //Serial.print(Wattage_1); 
 //Serial.println(" Watt ");
 lcd.setCursor(0,0);
 lcd.print(AmpsRMS);
 lcd.print("  ");
 lcd.print(Wattage,2);
 lcd.setCursor(0,1);
 lcd.print(voltage2,2);
 lcd.print("  ");
 lcd.print(voltage1,2);
 lcd.print("  ");
 lcd.print(voltage);
}
float getVPP()
{
  float result;
  
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int16_t minValue = ads.readADC_SingleEnded(2);          // store min value here
  
   uint32_t start_time = millis();

   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = ads.readADC_SingleEnded(0);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the maximum sensor value*/
           minValue = readValue;
       }
/*       Serial.print(readValue);
       Serial.println(" readValue ");
       Serial.print(maxValue);
       Serial.println(" maxValue ");
       Serial.print(minValue);
       Serial.println(" minValue ");
       delay(1000); */
    }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 0.1875)/1000;
      
   return result;

   
 }
