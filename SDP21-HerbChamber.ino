#include <DHT.h>
#include "FirebaseESP8266.h"  // Install Firebase ESP8266 library
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>

#define FIREBASE_HOST "sdp21-4b787.firebaseio.com/" //Without http:// or https:// schemes
#define FIREBASE_AUTH "Vw8ZlF4gKtPxfs4i0K7EWKMSVJpP6yvZFVWCwR2l"  
#define WIFI_SSID "nam1plus"
#define WIFI_PASSWORD "Tinkhung123!"

FirebaseData firebaseData; //Define FirebaseESP8266 data object
FirebaseJson json;


// Relay Control Pins
const int RLY_VALVE1      = D2;
const int RLY_VALVE2      = D3;
const int RLY_VALVE3      = D4;
const int RLY_VALVE4      = D5;
const int RLY_PUMP        = D6;
const int RLY_FANS        = D7;
const int RLY_LIGHT       = D8;
const int MUX_PIN1 = D0;
const int MUX_PIN2 = D1;

// Analog Sensor Pins


const int DHT_PIN = 10; //SD3


DHT dht(DHT_PIN, DHT11);

// Constants

const int waterTime = 2;          //should be 28
const int moistureThreshold = 30;
const int tempThreshold = 80;
const int inputProcessInterval = 1;


void setup() {
  //WIFI set up
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  Serial.begin(9600);

  //Assign pins
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(RLY_PUMP, OUTPUT);
  pinMode(RLY_FANS, OUTPUT);
  pinMode(RLY_LIGHT, OUTPUT);
  pinMode(RLY_VALVE1, OUTPUT);
  pinMode(RLY_VALVE2, OUTPUT);
  pinMode(RLY_VALVE3, OUTPUT);
  pinMode(RLY_VALVE4, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(RLY_PUMP, LOW);
  digitalWrite(RLY_FANS, LOW);
  digitalWrite(RLY_LIGHT, LOW);
  digitalWrite(RLY_VALVE1, HIGH);
  digitalWrite(RLY_VALVE2, LOW);
  digitalWrite(RLY_VALVE3, LOW);
  digitalWrite(RLY_VALVE4, LOW);


  
  dht.begin();                  //DHT Sensor
  Serial.println("Starting up...");
  delay(3000);
}

void loop() {
  digitalWrite(RLY_LIGHT, HIGH);
  processSensorInputs();
  delay(inputProcessInterval*1000);
  //statusBlink(1);
}

void processSensorInputs(){
  
  digitalWrite(MUX_PIN1, LOW);
  digitalWrite(MUX_PIN2, LOW);
  delay(200);
  int ma1 = analogRead(A0); 

  digitalWrite(MUX_PIN1, HIGH);
  digitalWrite(MUX_PIN2, LOW);
  delay(200);
  int ma2 = analogRead(A0);

  
  digitalWrite(MUX_PIN1, HIGH);
  digitalWrite(MUX_PIN2, LOW);
  delay(200);
  int ma3 = analogRead(A0);

  
  digitalWrite(MUX_PIN1, HIGH);
  digitalWrite(MUX_PIN2, HIGH);
  delay(200);
  int ma4 = analogRead(A0);
  
  
  double temp = dht.readTemperature(true);
  double humid = dht.readHumidity();

  if (isnan(temp) || isnan(humid)) {
     Serial.println("Failed to read from DHT sensor");
     temp = -1;
     humid = -1;
  }
  

  
  int m1 = map(ma1, 620, 440, 0, 100);
  int m2 = map(ma2, 625, 320, 0, 100);
  int m3 = map(ma3, 625, 320, 0, 100);
  int m4 = map(ma4, 835, 440, 0, 100);


  // Make decisions based on threshold

  if(m1 < moistureThreshold){
    Serial.println("Running water pump 1");
    delay(1000);
    runWaterPump(1);
  }

  if(m2 < moistureThreshold){
    Serial.println("Running water pump 2");
    delay(1000);
    runWaterPump(2);
  }

  if(m3 < moistureThreshold){
    Serial.println("Running water pump 3");
    delay(1000);
    runWaterPump(3);
  }

  if(m4 < moistureThreshold){
    Serial.println("Running water pump 4");
    delay(1000);
    runWaterPump(4);
  }

  if(temp > tempThreshold){
    digitalWrite(RLY_FANS, HIGH);
  }
  else{
    digitalWrite(RLY_FANS, LOW);
  }

  //Sending data to cloud
String h = (String)humid + " %";
String t = (String)temp + " F";
String s1 = (String)m1 + " %";
String s2 = (String)m2 + " %";
String s3 = (String)m3 + " %";
String s4 = (String)m4 + " %";
Firebase.setString(firebaseData,"/Humidity(Percentage)", h);
Firebase.setString(firebaseData,"/Temperature", t);
Firebase.setString(firebaseData,"/Soil_1", s1);
Firebase.setString(firebaseData,"/Soil_2", s2);
Firebase.setString(firebaseData,"/Soil_3", s3);
Firebase.setString(firebaseData,"/Soil_4", s4);

Serial.println(humid);
Serial.println(temp);
Serial.println(ma1);
Serial.println(ma2);
Serial.println(ma3);
Serial.println(ma4);
Serial.println("-------");

}


void runWaterPump(int bedNumber){ 
  selectValve(bedNumber);
  delay(1000);
  digitalWrite(RLY_PUMP, HIGH);
  delay(waterTime*1000);
  digitalWrite(RLY_PUMP, LOW);
  deselectValves(); 
}


void selectValve(int valveNum){
  

  digitalWrite(RLY_VALVE1, HIGH);
  digitalWrite(RLY_VALVE2, LOW);
  digitalWrite(RLY_VALVE3, LOW);
  digitalWrite(RLY_VALVE4, LOW);

  
  if(valveNum == 1) digitalWrite(RLY_VALVE1, LOW);
  if(valveNum == 2) digitalWrite(RLY_VALVE2, HIGH);
  if(valveNum == 3) digitalWrite(RLY_VALVE3, HIGH);
  if(valveNum == 4) digitalWrite(RLY_VALVE4, HIGH);

}

void deselectValves(){
  digitalWrite(RLY_VALVE1, HIGH);
  digitalWrite(RLY_VALVE2, LOW);
  digitalWrite(RLY_VALVE3, LOW);
  digitalWrite(RLY_VALVE4, LOW);
}




void allRelaysTest(){

  
  digitalWrite(D2, HIGH);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);
  digitalWrite(D5, LOW);
  digitalWrite(D6, LOW);
  digitalWrite(D7, LOW);
  digitalWrite(D8, LOW);

  Serial.println("Light test");
  digitalWrite(RLY_LIGHT, LOW);
  delay(2000);
  digitalWrite(RLY_LIGHT, HIGH);
  delay(2000);
  
  Serial.println("Fans test");
  digitalWrite(RLY_FANS, LOW);
  delay(2000);
  digitalWrite(RLY_FANS, HIGH);
  delay(2000);

  Serial.println("Pump test");
  digitalWrite(RLY_PUMP, LOW);
  delay(2000);
  digitalWrite(RLY_PUMP, HIGH);
  delay(2000);
  
  Serial.println("solenoid 1 test");
  digitalWrite(RLY_VALVE1, LOW);
  delay(2000);
  digitalWrite(RLY_VALVE1, HIGH);
  delay(2000);

  Serial.println("solenoid 2 test");
  digitalWrite(RLY_VALVE2, LOW);
  delay(2000);
  digitalWrite(RLY_VALVE2, HIGH);
  delay(2000);
  
  Serial.println("solenoid 3 test");
  digitalWrite(RLY_VALVE3, LOW);
  delay(2000);
  digitalWrite(RLY_VALVE3, HIGH);
  delay(2000);
  
  Serial.println("solenoid 4 test");
  digitalWrite(RLY_VALVE4, LOW);
  delay(2000);
  digitalWrite(RLY_VALVE4, HIGH);
  delay(2000);


}
