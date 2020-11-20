#include <DHT.h>
#include <DHT_U.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>


// Relay Control Pins
const int RLY_FANS        = 5;
const int RLY_VALVE1      = 9;
const int RLY_VALVE2      = 10;
const int RLY_VALVE3      = -1;
const int RLY_VALVE4      = -1;
const int RLY_NUTRIENTS   = -1;
const int RLY_PUMP        = 11;
const int RLY_LIGHT       = 12;

// Analog Sensor Pins
const int SENSOR_MOISTURE1 = A0;
const int SENSOR_MOISTURE2 = A1;
const int SENSOR_MOISTURE3 = A2;
const int SENSOR_MOISTURE4 = A3;

const int DHT_PIN = 12;
const int SENSOR_TEMPERATURE = 12;
const int SENSOR_HUMIDITY = 12;


DHT dht(DHT_PIN, DHT11);
SoftwareSerial serial(5,6);

// Constants

const int waterTime = 28;
const int nutrientTime = 5;

const int moistureThreshold = 25;
const int tempThreshold = 80;

const int minMoisture = 640;
const int maxMoisture = 360;

const int inputProcessInterval = 1;


void setup() {

  pinMode(RLY_PUMP, OUTPUT);
  pinMode(RLY_FANS, OUTPUT);
  pinMode(RLY_NUTRIENTS, OUTPUT);
  pinMode(RLY_LIGHT, OUTPUT);
  pinMode(RLY_VALVE1, OUTPUT);
  pinMode(RLY_VALVE2, OUTPUT);
  pinMode(RLY_VALVE3, OUTPUT);
  pinMode(RLY_VALVE4, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  
  digitalWrite(RLY_PUMP, LOW);
  digitalWrite(RLY_FANS, LOW);
  digitalWrite(RLY_NUTRIENTS, LOW);
  digitalWrite(RLY_LIGHT, LOW);
  digitalWrite(RLY_VALVE1, LOW);
  digitalWrite(RLY_VALVE2, LOW);
  digitalWrite(RLY_VALVE3, LOW);
  digitalWrite(RLY_VALVE4, LOW);

  statusBlink(3);

  //initialTest();

  //statusBlink(3);

  dht.begin();
  serial.begin(4800);

  digitalWrite(RLY_LIGHT, HIGH);
  
  
}

void statusBlink(int num){
  for(int i = 0; i < num; i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);  
    delay(1000);
  }
}

void loop() {
  processSensorInputs();
  delay(inputProcessInterval*1000);
}



void processSensorInputs(){

  
  int m1 = analogRead(SENSOR_MOISTURE1);
  int m2 = analogRead(SENSOR_MOISTURE2);
  int m3 = analogRead(SENSOR_MOISTURE3);
  int m4 = analogRead(SENSOR_MOISTURE4);
  
  double temp = dht.readTemperature();
  double humid = dht.readHumidity();

  if (isnan(temp) || isnan(humid)) {
    Serial.println("Failed to read from DHT sensor!");
  }


  m1 = map(m1, minMoisture, maxMoisture, 0, 100);
  m2 = map(m2, minMoisture, maxMoisture, 0, 100);
  m3 = map(m3, minMoisture, maxMoisture, 0, 100);
  m4 = map(m4, minMoisture, maxMoisture, 0, 100);

  // make decisions based on threshold

  if(m1 < moistureThreshold){
    runWaterPump(1);
  }

  if(m2 < moistureThreshold){
    runWaterPump(2);
  }

  if(m3 < moistureThreshold){
    runWaterPump(3);
  }

  if(m4 < moistureThreshold){
    runWaterPump(4);
  }

  if(temp < tempThreshold){
  //  digitalWrite(RLY_FANS, HIGH);
  }

  else{
 ///   digitalWrite(RLY_FANS, LOW);
  }

  
    statusBlink(1);

   StaticJsonDocument<1000> doc;
   JsonObject root = doc.to<JsonObject>();
   root["moisture1"] = 100;
   root["moisture2"] = 50;
   serializeJson(doc,serial);

}


void logInputs(){

  
}


void runWaterPump(int bedNumber){
  
  selectValve(bedNumber);
  digitalWrite(RLY_PUMP, HIGH);
  delay(waterTime*1000);
  digitalWrite(RLY_PUMP, LOW);
  selectValve(-1);
  
}

void runNutrientPump(int bedNumber){
  
  digitalWrite(RLY_NUTRIENTS, HIGH);
  delay(nutrientTime*1000);
  digitalWrite(RLY_NUTRIENTS, LOW);

  selectValve(bedNumber);
  digitalWrite(RLY_PUMP, HIGH);
  delay((nutrientTime+5)*1000);
  digitalWrite(RLY_PUMP, LOW);
  selectValve(-1);
  
}

void toggleFans(){
  digitalWrite(RLY_FANS, !digitalRead(RLY_FANS));
}


void toggleLights(){
  digitalWrite(RLY_LIGHT, !digitalRead(RLY_LIGHT));
}


void selectValve(int valveNum){
  
  int selectedValve = RLY_VALVE1 + (valveNum-1);

  for(int valve = RLY_VALVE1; valve <= RLY_VALVE4; valve++){
    
    if(valve == selectedValve)
      digitalWrite(valve, HIGH);
    else
      digitalWrite(valve, LOW);
  }
}


void initialTest(){

  delay(1000);
  digitalWrite(RLY_FANS, HIGH);
  delay(5000);
  digitalWrite(RLY_FANS, LOW);
  delay(500);
  
  digitalWrite(RLY_VALVE1, HIGH);
  delay(5000);
  digitalWrite(RLY_VALVE1, LOW);
  delay(500);
  
  digitalWrite(RLY_VALVE2, HIGH);
  delay(5000);
  digitalWrite(RLY_VALVE2, LOW);
  delay(1000);
  
  digitalWrite(RLY_PUMP, HIGH);
  delay(5000);
  digitalWrite(RLY_PUMP, LOW);
  delay(1000);

  digitalWrite(RLY_LIGHT, HIGH);
  delay(5000);
  digitalWrite(RLY_LIGHT, LOW);
  delay(1000);
  
  digitalWrite(RLY_FANS, HIGH);
  digitalWrite(RLY_PUMP, HIGH);
  delay(5000);
  digitalWrite(RLY_FANS, LOW);
  digitalWrite(RLY_PUMP, LOW);
  delay(1000);

  digitalWrite(RLY_FANS, HIGH);
  digitalWrite(RLY_PUMP, HIGH);
  digitalWrite(RLY_LIGHT, HIGH);
  delay(5000);
  digitalWrite(RLY_FANS, LOW);
  digitalWrite(RLY_PUMP, LOW);
  digitalWrite(RLY_LIGHT, LOW);
  delay(1000);
}
