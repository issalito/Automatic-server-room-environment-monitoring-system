#include "arduino_secrets.h"
// EmonLib - Version: Latest 
#include <EmonLib.h>

// #include <WiFiNINA.h>

// // HttpClient - Version: Latest 
// #include <HttpClient.h>
// #include <b64.h>

// Arduino_MKRIoTCarrier - Version: Latest 
#include <AirQualityClass.h>
#include <Arduino_MKRIoTCarrier.h>
#include <Arduino_MKRIoTCarrier_Buzzer.h>
#include <Arduino_MKRIoTCarrier_Qtouch.h>
#include <Arduino_MKRIoTCarrier_Relay.h>
#include <EnvClass.h>
#include <IMUClass.h>
#include <MKRIoTCarrierDefines.h>
#include <PressureClass.h>

/* 
  Sketch generated by the Arduino IoT Cloud Thing "Untitled"
  https://create.arduino.cc/cloud/things/2bba1071-3db5-43a4-a890-037ba95a1cc7 

  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when changes are made to the Thing

  String report;
  float ac_mains_voltage;
  float humidity;
  float pressure;
  float temperature;
  bool fireAlertStatus;
  bool smokeAlertStatus;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

#include "thingProperties.h"
MKRIoTCarrier carrier;
// WiFiClient client;
// HttpClient http(client);

const int pirPin = 1 ;

// String server = "http://maker.ifttt.com";
// String eventName = "temp_change";
// String IFTTT_Key = "cVt1uvmURG8pQNvjFG2qCD";
// String IFTTTUrl="https://maker.ifttt.com/trigger/temp_change/json/with/key/cVt1uvmURG8pQNvjFG2qCD";

// int value1;
// int value2;
// int value3;



int fire_sensor_pin = 5;
const int flame_sensor_pin = A5;

int smoke_sensor_Dpin = 2;
const int smoke_sensor_Apin = A2;
int smokeSensorValue;

 //volatage detetction
 
 #define VOLT_CAL 1871

EnergyMonitor emon1;             // Create an instance

  int voltage_sensor_pin = A1;

void setup() {
  
  
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  
  pinMode(pirPin, INPUT);
  pinMode(fire_sensor_pin, INPUT);
  pinMode(flame_sensor_pin, INPUT);
 // pinMode(smoke_sensor_Dpin, INPUT);
  pinMode(smoke_sensor_Apin, INPUT);
  
 
  
  
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
  
  //Wait to get cloud connection to init the carrier
  while (ArduinoCloud.connected() != 1) {
    ArduinoCloud.update();
    delay(500);
  }
  delay(500);
  CARRIER_CASE = false;
  carrier.begin();
  //voltage stuffs
  emon1.voltage(A1, VOLT_CAL, 1.7);  // Voltage: input pin, calibration, phase_shift
 
  carrier.display.setRotation(0);
  delay(1500);
  displayInitMsg();
  defaultPage();
  
  
  
}



void loop() {
  
  //volatage stuffs
  emon1.calcVI(25,1000);         // Calculate all. No.of half wavelengths (crossings), time-out
  ac_mains_voltage = emon1.Vrms;             //extract Vrms into Variable
 
  
  ArduinoCloud.update();
  carrier.Buttons.update();
  senseData();
  buttonClick();
  defaultPage();
  updateReport();
  //updateMotionTest();
  delay(1000);
  ArduinoCloud.update();
  
  }
  
  
  void defaultPage(){
  // Default Page shown on Display of Opla Kit
  carrier.display.fillScreen(0x0000);
  carrier.display.setTextColor(0xFFFF);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(44, 40);
  carrier.display.print("Press Buttons");
  carrier.display.setCursor(24, 70);
  carrier.display.print("to Get the Sensor");
  carrier.display.setCursor(70, 100);
  carrier.display.setTextSize(2);
  carrier.display.print("Readings");
  carrier.display.drawBitmap(70, 145, gesture_logo, 100, 100, 0x0D14); //0x0CA1A6);
  delay(100);
}

void displayInitMsg(){
  carrier.display.fillScreen(0x0000);
  //Basic configuration for the text
  carrier.display.setRotation(0);
  carrier.display.setTextWrap(true);
  carrier.display.drawBitmap(50, 40, arduino_logo, 150, 106, 0x253); //0x00979D);
  carrier.display.setTextColor(0xFFFF);
  carrier.display.setTextSize(3);
  carrier.display.setCursor(35, 160);
  carrier.display.print("Loading");
  //Printing a three dots animation
  for (int i = 0; i < 3; i++)
  {
    carrier.display.print(".");
    delay(1000);
  }

  carrier.display.fillScreen(0x0000);
  carrier.display.setTextColor(0xFFFF);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(55, 30);
  carrier.display.print("Arduino IoT");
  carrier.display.setTextSize(2);
  carrier.display.setCursor(15, 60);
  carrier.display.print("Final year Project");

  carrier.display.setTextSize(2);
  carrier.display.setTextColor(0x07FF);
  carrier.display.setCursor(24, 120);
  carrier.display.println("Automatic");
  carrier.display.setCursor(25, 140);
  carrier.display.println(" Server Room Monitoring");
  carrier.display.setCursor(75, 160);
  carrier.display.println("System");
  delay(4500);
}
  
  void senseData() {
    while(!carrier.Light.colorAvailable()) {
    delay(5);
  }
  int none;
  carrier.Light.readColor(none, none, none, light);
  
  temperature = carrier.Env.readTemperature();
  humidity = carrier.Env.readHumidity();
  pressure = carrier.Pressure.readPressure();
  
  bool motion = digitalRead(pirPin);
  if(motion == 0){
    motionStatus = 1;
  }
  else{
    motionStatus = 0;
  }
  
// bool fire = digitalRead(fire_sensor_pin);
//   if(fire == false){
//     fireAlertStatus = true;
//     Serial.print(fire);
//   }
//   else{
//     fireAlertStatus = false;
//     Serial.print(fire);
//   }

  int flameIntensity = analogRead(flame_sensor_pin);

  if (flameIntensity > 500) {
    fireAlertStatus = true;
  }
  else {
    fireAlertStatus = false;
  }
  
  smokeSensorValue = analogRead(smoke_sensor_Apin);
  
  if (smokeSensorValue > 500) {
    smokeAlertStatus = true;
    Serial.print(smokeSensorValue);
  } else {
    smokeAlertStatus = false;
    Serial.print(smokeSensorValue);
    
  }
  
  }
  
  
  
  
  void buttonClick() {
    if (carrier.Buttons.onTouchDown(TOUCH1)) {
    showTemperature();
  }
 
  if (carrier.Buttons.onTouchDown(TOUCH3)) {
    showHumidity();
  }
 
  if (carrier.Buttons.onTouchDown(TOUCH0)) {
    showVoltage();
  }
 
  if (carrier.Buttons.onTouchDown(TOUCH4)) {
    showPressure();
  }
    
  }
  
  
  void showTemperature(){
  // displaying temperature
  carrier.display.fillScreen(0x0000);
  carrier.display.setCursor(25, 60);
  carrier.display.setTextSize(3);
  carrier.display.print("Temperature");
  carrier.display.drawBitmap(80, 80, temperature_logo, 100, 100, 0xDAC9); //0xDA5B4A); //draw a thermometer on the MKR IoT carrier's display
  float temperature = carrier.Env.readTemperature();   //storing temperature value in a variable
  carrier.display.setCursor(60, 180);
  carrier.display.print(temperature); // display the temperature on the screen
  carrier.display.print(" C");
  delay(5000);
  defaultPage();
}

void showHumidity(){
  // displaying humidity
  carrier.display.fillScreen(0x0000);
  carrier.display.setCursor(54, 40);
  carrier.display.setTextSize(3);
  carrier.display.print("Humidity");
  carrier.display.drawBitmap(70, 70, humidity_logo, 100, 100, 0x0D14); //0x0CA1A6); //draw a humidity figure on the MKR IoT carrier's display
  float humidity = carrier.Env.readHumidity(); //storing humidity value in a variable
  carrier.display.setCursor(60, 180);
  carrier.display.print(humidity); // display the humidity on the screen
  carrier.display.print(" %");
  delay(5000);
  defaultPage();
}

void showPressure(){
  // displaying pressure
  carrier.display.fillScreen(0x0000);
  carrier.display.setCursor(54, 40);
  carrier.display.setTextSize(3);
  carrier.display.print("Pressure");
  carrier.display.drawBitmap(70, 60, pressure_logo, 100, 100, 0xF621); //0xF1C40F); //draw a pressure figure on the MKR IoT carrier's display
  float press = carrier.Pressure.readPressure(); //storing pressure value in a variable
  carrier.display.setCursor(40, 160);
  carrier.display.println(press); // display the pressure on the screen
  carrier.display.setCursor(160, 160);
  carrier.display.print("KPa");
  delay(5000);
  defaultPage();
}

void showVoltage(){
  // displaying pressure
  carrier.display.fillScreen(0x0000);
  carrier.display.setCursor(54, 40);
  carrier.display.setTextSize(3);
  carrier.display.print("AC Mains");
  carrier.display.setCursor(40, 160);
  carrier.display.println(ac_mains_voltage); // display the pressure on the screen
  carrier.display.setCursor(160, 160);
  carrier.display.print("V");
  delay(2500);
  defaultPage();
}


  
   void updateReport() {
    if((temperature >= 20 && temperature <= 22) && (humidity >= 40 && humidity <=60) && pressure < 125 ){
      report = "Good overall conditions Inside";
       
      }else {
      report = "Non Favourable conditions inside";
    }
 }
 
// void checkTemperature() {
//   if (temperature >= 22) {
//     report = "It is too war inside";
    
//   }
//   else{
//     report = "Normal temperature levels";
//   }
// }
 
// void checkHumidity() {
//   if (humidity >= 60) {
//     report = "It is too humid inside";
    
//   }else if (humidity <= 40) {
//     report = "low humidity levels";
    
//   }
//   else {
//     report = "Good Humidity levels";
//   }
// }
 
// void checkPressure() {
//   if (pressure >= 125) {
//     report = "limited air flow";
//   }
  
//   else{
//   report = "Good air flow"; 
//   }
// }

// void updateMotionTest() {
//   if (motionStatus == 1) {
//     motionTest = "Motion detected";
//   }else {
//     motionTest = "No motion detected";
//   }
//   delay(10000);
// }









