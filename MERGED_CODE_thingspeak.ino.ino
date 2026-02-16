#include <ph4502c_sensor.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Arduino.h>
#include <WiFiEsp.h>
#include "ThingSpeak.h"


const char* ssid = "Diip";   // your network SSID (name)
const char* password = "#1Augmedix";   // your network passw
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiEspClient  client;


// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
//#define ESP_BAUDRATE  115200
//#else
//#define ESP_BAUDRATE  115200
#endif


unsigned long myChannelNumber = 2766828;
const char * myWriteAPIKey = "GRMRA2EFB216J4JQ";


// Initialize our values
int number1 = 0;
int number2 = random(0,100);
int number3 = random(0,100);
int number4 = random(0,100);
String myStatus = "";


LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display




int relay1 = 5; // Plug the relay into Digital Pin 5 for aerator for DO
int relay2 = 2; // Plug the relay into Digital Pin 2 for heater for temperature
int relay3 = 3; // Plug the relay into Digital Pin 3 for pump to release ph buffer droplets




PH4502C_Sensor ph4502c(A0, A1);
// Analog Pin 0 - pH level pin
// Analog Pin 1 - temperature pin




// Data wire is connect to the Arduino digital pin 4
#define ONE_WIRE_BUS 4
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);
float temp;




#define TdsSensorPin A2
#define VREF 5.0      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30           // sum of sample point
int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;
float averageVoltage = 0,tdsValue = 0,temperature = 25;




#define DO_PIN A3
float X;




#define VREF_1 5000    //VREF (mv)
#define ADC_RES 1024 //ADC Resolution
float D_O;
//Single-point calibration Mode=0
//Two-point calibration Mode=1
#define TWO_POINT_CALIBRATION 0




#define READ_TEMP (25) //Current water temperature ℃, Or temperature sensor function




//Single point calibration needs to be filled CAL1_V and CAL1_T
#define CAL1_V (537) //mv
#define CAL1_T (32)   //℃
//Two-point calibration needs to be filled CAL2_V and CAL2_T
//CAL1 High temperature point, CAL2 Low temperature point




const uint16_t DO_Table[41] = {
    14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
    11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
    9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
    7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410};




uint8_t Temperaturet;
uint16_t ADC_Raw;
uint16_t ADC_Voltage;
uint16_t DO;




int16_t readDO(uint32_t voltage_mv, uint8_t temperature_c)
{
  uint16_t V_saturation = (uint32_t)CAL1_V + (uint32_t)35 * temperature_c - (uint32_t)CAL1_T * 35;
  return (voltage_mv * DO_Table[temperature_c] / V_saturation);
 
}




void setup() {
    // Initialize the sensor
    ph4502c.init();
    Serial.begin(115200);
    Serial3.begin(115200);


     
  // initialize serial for ESP module  
 
  WiFi.init(&Serial3);
 
  Serial.print("Searching for ESP8266...");
  // initialize ESP module
  //WiFi.init(&Serial);


  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }
 Serial.println("found it!");


  // Start up the library for temp sensor
  sensors.begin();  




  pinMode(TdsSensorPin,INPUT);
 
  lcd.init();
  lcd.clear();        
  lcd.backlight();      // Make sure backlight is on
 
  // Print a message on both lines of the LCD.
  lcd.setCursor(5,0);   //Set cursor to character 2 on line 0
  lcd.print("Welcome");
  lcd.setCursor(7,1);
  lcd.print("To");
  delay (3000);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("BioFloc System ");
  delay (5000);
  lcd.clear();




  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);


 
  ThingSpeak.begin(client);  // Initialize ThingSpeak


}




void loop() {
 // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, password);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);    
    }
    Serial.println("\nConnected.");
  }
    // Read pH value
  float pHValue = ph4502c.read_ph_level();


 
  Serial.print("pH:");
 
  Serial.print(pHValue);
 
  Serial.print('\n');
    // Do something with the pH value
    // Delay before next reading
    delay(1000);
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  Serial.print("Celsius temperature: ");
  // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  Serial.print(temp);
 
delay(1000);  








static unsigned long analogSampleTimepoint = millis();
   if(millis()-analogSampleTimepoint > 40U)     //every 40 milliseconds,read the analog value from the ADC
   {
     analogSampleTimepoint = millis();
     analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer
     analogBufferIndex++;
     if(analogBufferIndex == SCOUNT)
         analogBufferIndex = 0;
   }  
   static unsigned long printTimepoint = millis();
   if(millis()-printTimepoint > 800U)
   {
      printTimepoint = millis();
      for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      float compensationCoefficient=1.0+0.02*(temp-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationVolatge=averageVoltage/compensationCoefficient;  //temperature compensation
      tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
      //Serial.print("voltage:");
      //Serial.print(averageVoltage,2);
      //Serial.print("V   ");
      Serial.print("TDS Value:");
      Serial.print(tdsValue,0);
      Serial.println("ppm");
   }




   Temperaturet = (uint8_t)READ_TEMP;
  ADC_Raw = analogRead(DO_PIN);
  ADC_Voltage = uint32_t(VREF_1) * ADC_Raw / ADC_RES;








  Serial.print("Temperaturet:\t" + String(Temperaturet) + "\t");
  Serial.print("ADC RAW:\t" + String(ADC_Raw) + "\t");
  Serial.print("ADC Voltage:\t" + String(ADC_Voltage) + "\t");
  Serial.println("DO:\t" + String(readDO(ADC_Voltage, Temperaturet)) + "\t");
  X = readDO(ADC_Voltage, Temperaturet);
  D_O = X/1000;
  Serial.print("Real DO:");
  Serial.print(D_O);
  delay(1000);


delay(500);    
   
   
    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    float a = ThingSpeak.writeField(myChannelNumber, 1, pHValue , myWriteAPIKey);
    int b = ThingSpeak.writeField(myChannelNumber, 2, temp, myWriteAPIKey);
    int c = ThingSpeak.writeField(myChannelNumber, 3, tdsValue, myWriteAPIKey);
    float d = ThingSpeak.writeField(myChannelNumber, 2, D_O, myWriteAPIKey);
    //uncomment if you want to get temperature in Fahrenheit
    //int b = ThingSpeak.writeField(myChannelNumber, 1, temperatureF, myWriteAPIKey);


    if(a == b == c == d == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(a) + String(b) + String(c) + String(d));
    }
    // change the values
  number1++;
  if(number1 > 99){
    number1 = 0;
  }
  number2 = random(0,100);
  number3 = random(0,100);
  number4 = random(0,100);
 
  delay(20000); // Wait 20 seconds to update the channel again






      lcd.setCursor(0,0);
      lcd.print("pH:");
      lcd.setCursor(3,0);
      lcd.print(pHValue);




      lcd.setCursor(8,0);
      lcd.print("DO:");
      lcd.setCursor(11,0);
      lcd.print(D_O);








      lcd.setCursor(0,1);
      lcd.print("Tem:");
      lcd.setCursor(4,1);
      lcd.print(temp);








      lcd.setCursor(9,1);
      lcd.print("TDS:");
      lcd.setCursor(13,1);
      lcd.print(tdsValue,0);




  if (D_O<=6.0) {
  digitalWrite(relay1, HIGH); // Turn the relay on
  delay(10000); // Wait 10 second
  }
  else {
  digitalWrite(relay1, LOW); // Turn the relay Off
  delay(10000); // Wait 10 second
  }




  if (temp<=27) {
  digitalWrite(relay2, HIGH); // Turn the relay on
  delay(10000); // Wait 10 second
  }
  else {
  digitalWrite(relay2, LOW); // Turn the relay on
  delay(10000); // Wait 10 second
  }




  if (pHValue<=6.4 && pHValue>=8.4) {
  digitalWrite(relay3, HIGH); // Turn the relay on
  delay(10000); // Wait 10 second
  }
  else {
    digitalWrite(relay3, LOW); // Turn the relay on
  delay(10000); // Wait 10 second
  }
 
  delay(1000); // Adjust delay as needed
 
}


int getMedianNum(int bArray[], int iFilterLen)
{
      int bTab[iFilterLen];
      for (byte i = 0; i<iFilterLen; i++)
      bTab[i] = bArray[i];
      int i, j, bTemp;
      for (j = 0; j < iFilterLen - 1; j++)
      {
      for (i = 0; i < iFilterLen - j - 1; i++)
          {
        if (bTab[i] > bTab[i + 1])
            {
        bTemp = bTab[i];
            bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
         }
      }
      }
      if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
      else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
      return bTemp;


}


// This function attempts to set the ESP8266 baudrate. Boards with additional hardware serial ports
// can use 115200, otherwise software serial is limited to 19200.
//void setEspBaudRate(unsigned long baudrate){
  //long rates[6] = {115200,74880,57600,38400,19200,9600};


  //Serial.print("Setting ESP8266 baudrate to ");
  //Serial.print(baudrate);
  //Serial.println("...");


  //for(int i = 0; i < 6; i++){
    //Serial1.begin(rates[i]);
    //delay(100);
    //Serial1.print("AT+UART_DEF=");
    //Serial1.print(baudrate);
    //Serial1.print(",8,1,0,0\r\n");
    //delay(100);  
  //}
   
  //Serial1.begin(baudrate);
//}

