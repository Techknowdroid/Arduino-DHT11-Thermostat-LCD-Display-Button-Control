// CLIMATE CONTROL for AirConditioners.
// Original Written by ladyada, public domain.
// MODIFIED BY Techknowdroid on 5.September.2018 (Program to get value from DHT11 on startup and use this value as the base temperature to be maintained.
// Button introduced by Techknowdroid to change the Cut-OFF temperature.(User preferred Cut-OFF temperature).
// Millis() used to time the cutoff and the compressor start cycles.




unsigned long interval15sec=15000; // the time we need to wait BEFORE the Aircon is turned Off after the Desired Temp is Achieved.
unsigned long interval5min=300000; // the time we need to wait before restarting the compressor.
unsigned long interval10sec=10000; // the time we need to wait before each temperature reading.
unsigned long interval10secs=5000; // the time we need to wait before first start of Arduino/AirCON.

unsigned long time_15sec = 0; // millis() returns an unsigned long.
unsigned long time_5min = 0; // millis() returns an unsigned long.
unsigned long time_10sec = 0; // millis() returns an unsigned long.
unsigned long time_10secs = 0; // millis() returns an unsigned long for the time we need to wait before first start of AirCON
unsigned long secs = 0; // millis() returns an unsigned long for the time we need to wait before first start of AirCON
unsigned long countdown=10; // countdown variable for the print_time function.
//unsigned long countdown2=10; // countdown2 variable for the print_time function.

void print_time(unsigned long time_millis);  // function declaration.

// include the LCD library code  We’re using the library by Marco Schwartz.
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
// SDA to Analog 4 on arduino
// SCL to Analog 5 on arduino
LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display. SDA to Analog 4 on arduino.SCL to Analog 5 on arduino.


// include the DHT Sensor library code  from ladyada and adafruit.
#include "DHT.h"
// Uncomment whatever type of DHT you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
// Initialize DHT sensor for normal 16mhz Arduino
#define DHTPIN 2     // what pin the DHT is connected to on the arduino
DHT dht(DHTPIN, DHTTYPE);

// Define the relay pin and the on off values.
#define Relay1_IN1 7 // Digital Pin 7 of arduino is connected to IN1 Pin of Relay1_IN1.(We can have more than 1 RelayBank & multiple IN pins so we name this one as Relay1_IN1
#define RELAY_ON 0 // Assign the Value of 0 to the variable RELAY_ON
#define RELAY_OFF 1 // Assign the Value of 1 to the variable RELAY_OFF


// DECLARE the below as GLOBAL VARIABLES.
volatile boolean ButtonPressed = false;

int relayState = digitalRead(Relay1_IN1);
float firstemperature;  //We capture the room temperature here just once, at the start of the Arduino being powered up.
float roomtemperature;  //We capture the realtime room temperature here.
//float inoperationroomtemperature;  // the temperature of the room after sometime of the system being in operation ( in the void loop() )
float requiredtemp;  // this is the required target temperature where the aircondioner will be switched off.
float hyteresistemp; // the gap in degrees Celsius, between the starting higher (roomtemperature) and the lower (requiredtemp).
int firstime = 1; // used to execute the AirCon start loop just once.


// the setup routine runs only once whenever you give power to the arduino.
void setup()
{
  Serial.begin(9600);
  lcd.init(); //initialize the lcd
  pinMode(Relay1_IN1, OUTPUT);// set the pin connected to IN1 terminal of Relay1 as output. Our case pin 7 defined earlier.
  digitalWrite(Relay1_IN1, HIGH); // Set this Relaybanks's Pin IN1 as OFF when we power up the Arduino for the first time.
  dht.begin();
  //delay(2000);
  pinMode(3,INPUT); // Button Press detected here
  attachInterrupt(digitalPinToInterrupt(3),PressUPButton,RISING); // Function PressUPButton, is after the void loop
 }

void loop(){
  unsigned long currentMillis = millis(); // grab current time. 
  
 if ((unsigned long)(currentMillis - time_10sec) >= interval10sec) // 10 seconds wait in this Loop before next temperature read cycle. 
 { 
    // unsigned long currentMillis = millis(); // grab current time.
  
    // Reading temperature or humidity takes about 250 milliseconds!
    // Wait a few seconds between measurements.
    float t = dht.readTemperature();
    roomtemperature = t;    
    Serial.println("               ");
    Serial.print(roomtemperature);
    Serial.println("°C is the Room temperature. ");
    Serial.print(requiredtemp);
    Serial.println("°C is the Cutoff temperature. ");
    //time_10sec = millis();
    print_time(time_10sec);
    countdown=countdown+10;
    Serial.println("Reading happens every 10 Seconds!");
    Serial.println("               ");
    Serial.println("               ");
    
    
    //////    LCD    LCD   LCD   LCD    ///////////////////////////////////////
    //LCD starts to display the temperature.
    relayState = digitalRead(Relay1_IN1);  // Get the relay state here... is relay ON or OFF ? Value of 1 means OFF. Value of 0 means ON.
    if(relayState == 0) // if compressor is ON then display the following:
    {
     lcd.backlight(); //open the backlight 
     lcd.setCursor(0,0); // set the cursor to column 1, line 0
     //lcd.print("Is");
     lcd.setCursor(0,0); // set the cursor to column 0, line 0 
     lcd.print(int(roomtemperature));
     lcd.print("\337"); //The degree symbol is \337
     lcd.print("C");
     lcd.setCursor(5,0); // set the cursor to column 5, line 1
     lcd.print("OFF at");
     lcd.setCursor(12,0); // set the cursor to column 12, line 1
     lcd.print(int(requiredtemp));
     lcd.print("\337"); // The degree symbol is \337
     lcd.print("C ");
    }

    if(relayState == 1) // if compressor is OFF then display the following:
    {
     lcd.backlight(); //open the backlight 
     lcd.setCursor(0,0); // set the cursor to column 0, line 0
     //lcd.print("Is");
     lcd.setCursor(0,0); // set the cursor to column 0, line 0 
     lcd.print(int(roomtemperature));
     lcd.print("\337"); //The degree symbol is \337
     lcd.print("C");
     lcd.setCursor(5,0); // set the cursor to column 5, line 1
     lcd.print(" ON at ");
     lcd.setCursor(12,0); // set the cursor to column 12, line 1
     lcd.print(int(requiredtemp+1));
     lcd.print("\337"); // The degree symbol is \337
     lcd.print("C ");
    }

    // Check if any reads failed and exit early (to try again).
    if (isnan(t))
    {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
     // save the "current" time
    time_10sec = millis();
 }   

  // Temperature monitoring starts here. Hyteresis setting of 2 degrees Celsius.So relay switches ON when temp>=27*C & switches OFF when temp<=25*C.
  

  relayState = digitalRead(Relay1_IN1);
  
     if ((firstime == 1) && (relayState == 1) && (currentMillis - time_10secs) >= interval10secs)  //On first run, start the Aircon immediately after say 10 seconds of Arduino starting.
        {  float t = dht.readTemperature();
           firstemperature = t;  //We capture the room temperature here just once, at the start of the Arduino being powered up.
           roomtemperature = t; // Room temperature will be monitored every 5 minutes after the COOL Cycle Starts.
           hyteresistemp = 2; // the gap in degrees Celsius, between the starting higher (roomtemperature) and the lower (requiredtemp).
           requiredtemp = roomtemperature - hyteresistemp;      // this is the required target temperature where the aircondioner will be switched off.
           Serial.print("The FirstRoom Temperature reading is " );
           Serial.print(firstemperature);
           Serial.println("°C");
           Serial.print("Hysteresis is " );
           Serial.print(hyteresistemp);
           Serial.println("°C");
           Serial.print("The required temperature is " );
           Serial.print(requiredtemp);
           Serial.println("°C");
           Serial.println("Airconditioner will change STATE after 5 minutes of any CYCLE change");
           Serial.println("First Run, so Aircon turned ON in 10 seconds");
           Serial.println("********************************************************************");
           Serial.println("      ");
           Serial.println("      ");
           //delay(10000);  // Only happens ONE TIME when arduino starts.
            digitalWrite(Relay1_IN1, RELAY_ON); //Activate air conditioner by Turning the Relay_1_IN1 ON by giving it value of 0 as RELAY_ON 0 has been defined earlier.
            relayState = digitalRead(Relay1_IN1);
            firstime = 2;  // we increment this variable to 2 so that this loop does not execute again.
                 
        }
   
 
  //  CHECK if Room is HOT - Long check HOT CYCLE to enable delayed Starting of Compressor.
  //   relayState = digitalRead(Relay1_IN1);
     
     if ((roomtemperature > requiredtemp) && (relayState == 1) && (currentMillis - time_5min) >= interval5min) //if room temperature is above the required temperature then TURN Aircon ON but after minimum 5 minutes
     {  
        Serial.println("      ");
        Serial.println("It is HOT! "); 
        Serial.println("Aircon will turn ON after 1 minute"); // which is the value of the period variable.
        Serial.println("      ");
        digitalWrite(Relay1_IN1, RELAY_ON); //Activate air conditioner by Turning the Relay_1_IN1 ON by giving it value of 0 as RELAY_ON 0 has been defined earlier. 
        relayState = digitalRead(Relay1_IN1);
        // save the "current" time
        time_5min = millis();
     }
      
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
  //  CHECK if Room is COLD - shorter check cycle to enable Cut off Faster as soon as Cold Temp is Achieved ////
  //relayState = digitalRead(Relay1_IN1);  
  if ((unsigned long)(currentMillis - time_15sec) >= interval15sec)
  {   float t = dht.readTemperature();
      roomtemperature = t;   // Check the room temperature every 15 seconds
         
      if ((roomtemperature < requiredtemp) && (relayState == 0)) //if temperature is under the required temperature & relay state is ON
      {   
       Serial.println("      ");
       Serial.print(requiredtemp);
       Serial.println(" It is COLD !");
       digitalWrite(Relay1_IN1, RELAY_OFF); // Turn OFF air conditioner by Turning the Relay_1_IN1 OFF by giving it value of 1 as RELAY_OFF 1 has been defined earlier.
       Serial.print(roomtemperature);
       Serial.println("°C is the current temperature. ");
       relayState = digitalRead(Relay1_IN1);
       Serial.println("I am the 15 secs counter !!");
       Serial.println("Aircon turned OFF in 15 Secs");
       Serial.println("of Achieving Target Temperature!!");
       Serial.println("   ");
       Serial.println("   ");
       // save the "current" time
       time_15sec = millis();
       //print_time(time_15sec);
       time_5min = millis(); // here we reset & freshen up the 5 minutes counter for the HOT Cycle,so we get an accurate 5 minute gap before switching ON the compressor.
       // time_millis=millis(); // grab the current time when the AIRCON switched OFF here in the COLD CYCLE.
       countdown=10;
    }
       
  }
  
 
}


void PressUPButton()
{ 
  if(ButtonPressed)
  {
    ButtonPressed = false;
    //digitalWrite(13,LOW);
  }else
  {
    ButtonPressed = true;
    //digitalWrite(13,HIGH);
    if (requiredtemp<32)
    {requiredtemp++ ;
    }else
    {
      requiredtemp=18;
    }
      
    //Serial.println("uptemperature = ");
    //Serial.println(uptemperature);
    //Serial.println("             ");
      
  }
}


void print_time(unsigned long time_millis)
{
    if(relayState == 0) // if compressor is ON then display the following:
    {
     Serial.print("Time: ");
     Serial.print(time_millis/1000);
     Serial.print("s - ");
    
     //lcd.setCursor(0,1); // set the cursor to column 0, line 1
     //lcd.print("Time:");
     lcd.setCursor(0,1); // set the cursor to column 0, line 1 
     lcd.print(time_millis/1000);
     lcd.print("s");
     lcd.setCursor(9,1); // set the cursor to column 0, line 1 
     lcd.print("        ");
    }

  if(relayState == 1) // if compressor is OFF then display the following:
    {
     Serial.print("Time: ");
     Serial.print(time_millis/1000);
     Serial.print("s - ");
    
     //lcd.setCursor(0,1); // set the cursor to column 0, line 1
     //lcd.print("Time:");
     lcd.setCursor(0,1); // set the cursor to column 0, line 1 
     lcd.print(time_millis/1000);
     lcd.print("s");
     lcd.setCursor(11,1); // set the cursor to column 9, line 1 
     lcd.print(countdown);
    }  
    
}

