#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include <Adafruit_ADS1015.h>
Adafruit_ADS1115 ads(0x48);
LiquidCrystal_I2C lcd(0x3F, 16, 2);

float voltage = 0.0;
int16_t adc0;
float voltage2 = 0.0;
int16_t adc2;
int16_t adc1;
float voltage1 = 0.0;
unsigned long last_time =0;
unsigned long current_time =0;
float power = 0;         // power in watt              
float Wh =0 ;             // Energy in kWh
float Watt=0;
int mVperAmp = 185; // use 185 for 5A, 100 for 20A Module and 66 for 30A Module
double Voltage = 0.0;
double VRMS = 0.0;
double AmpsRMS = 0.0;

#define BLYNK_PRINT Serial    
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "_qpHJMk447tF0GwGWevKI-717CBGiSJp";
char ssid[] = "TuanSang";
char pass[] = "0343215116";

#define SENSOR  0
long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
float flowMilliLitres;
unsigned int totalMilliLitres;
float flowLitres;
float totalLitres;
float h;
 
void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

void setup() {
    Wire.begin(12, 14);
    Serial.begin(9600);
    lcd.begin();
    ads.begin();

    Blynk.begin(auth, ssid, pass);
    pinMode(SENSOR, INPUT_PULLUP);
    pulseCount = 0;
    flowRate = 0.0;
    flowMilliLitres = 0.0;
    totalMilliLitres = 0;
    previousMillis = 0;
 
    attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);
}


void loop() {

    int16_t adc0; // we read from the ADC, we have a sixteen bit integer as a result
    adc0 = ads.readADC_Differential_0_1();
    adc2 = ads.readADC_SingleEnded(2);
    
      Voltage = getVPP();
      VRMS = (Voltage/2.0) *0.707; // sq root
      AmpsRMS = (VRMS * 1000)/mVperAmp;
      if((AmpsRMS > -0.015) && (AmpsRMS < 0.008)){  // remove low end chatter
      AmpsRMS = 0.0;
      }
      AmpsRMS = AmpsRMS*1.8;
      float Wattage = (220*AmpsRMS); //Observed 18-20 Watt when no load was connected, so substracting offset value to get real consumption.
      last_time = current_time;
      current_time = millis();    
      Wh = Wh+  Wattage *(( current_time -last_time) /3600000.0) ; 

//lưu lượng nước
      pulse1Sec = pulseCount;
    pulseCount = 0;
 
    
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();
 
    
    flowMilliLitres = (flowRate / 60) * 1000;
    flowLitres = (flowRate / 60);
 
    
    totalMilliLitres += flowMilliLitres;
    totalLitres += flowLitres;
    
     h=totalMilliLitres/(5*5);
    Serial.print("Flow rate: ");
    Serial.print(float(flowMilliLitres));  
    Serial.print("mL/s");
    Serial.print("\t");       
    
    Serial.print("chieucao: ");
    Serial.print(float(h));  
    Serial.print("cm");
    Serial.print("\t");      

   
   Serial.print(flowMilliLitres);
   Serial.println("flowMilliLitres:");
   Blynk.virtualWrite(V6, flowMilliLitres);

   Serial.print(h);
   Serial.println("chieucao:");
   Blynk.virtualWrite(V5, h);
   delay(200);
   Serial.print(Wh);
   Serial.println("Công suất:");
   Blynk.virtualWrite(V7, Wh);
   delay(200);
   
  Blynk.run();

      
      lcd.setCursor(0,0);
      lcd.print(AmpsRMS);
      lcd.print("  ");
      lcd.print(Wattage,2);
      lcd.print(" ");
      lcd.print(adc2);
      lcd.setCursor(0,1);
      lcd.print("Wh: ");   
      lcd.print(String(Wh, 3)); 
      lcd.print(" Wh"); 
 
}
float getVPP()
{
  float result;
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = ads.readADC_SingleEnded(2);          // store min value here
  uint32_t start_time = millis();
   while((millis()-start_time) <  1000) //sample for 1 Sec
   {
       readValue = ads.readADC_Differential_0_1();;
  
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
          
    }
   
   result = ((maxValue - minValue) *0.1875)/1000;
   return result; 
 }
