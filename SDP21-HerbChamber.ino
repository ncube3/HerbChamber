#include <DHT.h>
#include <DHT_U.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>


// Relay Control Pins
const int RLY_VALVE1      = 2;
const int RLY_VALVE2      = 3;
const int RLY_VALVE3      = 4;
const int RLY_VALVE4      = 5;
const int RLY_PUMP        = 6;
const int RLY_FANS        = 7;
const int RLY_LIGHT       = 8;

// Analog Sensor Pins
const int SENSOR_MOISTURE1 = A0;
const int SENSOR_MOISTURE2 = A1;
const int SENSOR_MOISTURE3 = A2;
const int SENSOR_MOISTURE4 = A3;

const int DHT_PIN = 2;


DHT dht(DHT_PIN, DHT11);
SoftwareSerial serialtoESP(9,10);

// Constants

const int waterTime = 5;          //should be 28


const int moistureThreshold = 30;
const int tempThreshold = 80;

const int minMoisture = 250;
const int maxMoisture = 700;

const int inputProcessInterval = 1;


void setup() {

  pinMode(RLY_PUMP, OUTPUT);
  pinMode(RLY_FANS, OUTPUT);
  pinMode(RLY_LIGHT, OUTPUT);
  pinMode(RLY_VALVE1, OUTPUT);
  pinMode(RLY_VALVE2, OUTPUT);
  pinMode(RLY_VALVE3, OUTPUT);
  pinMode(RLY_VALVE4, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(SENSOR_MOISTURE1, INPUT);
  pinMode(SENSOR_MOISTURE2, INPUT);
  pinMode(SENSOR_MOISTURE3, INPUT);
  pinMode(SENSOR_MOISTURE4, INPUT);
  
  digitalWrite(RLY_PUMP, LOW);
  digitalWrite(RLY_FANS, LOW);
  digitalWrite(RLY_LIGHT, LOW);
  digitalWrite(RLY_VALVE1, LOW);
  digitalWrite(RLY_VALVE2, LOW);
  digitalWrite(RLY_VALVE3, LOW);
  digitalWrite(RLY_VALVE4, LOW);


  
  dht.begin();                  //DHT Sensor
  serialtoESP.begin(115200);   // Serial to ESP
  Serial.begin(9600);
  Serial.println("Starting up");
  statusBlink(3);
  
  //turning on the fan and light
  digitalWrite(RLY_LIGHT, HIGH);
  digitalWrite(RLY_FANS, HIGH);
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
  Serial.println("Loop");
  processSensorInputs();
  delay(inputProcessInterval*1000);
  statusBlink(1);
}



void processSensorInputs(){

  
  int ma1 = analogRead(SENSOR_MOISTURE1);   //make average of 100 measuremnts
  int ma2 = analogRead(SENSOR_MOISTURE2);
  int ma3 = analogRead(SENSOR_MOISTURE3);
  int ma4 = analogRead(SENSOR_MOISTURE4);
  
  double temp = dht.readTemperature(true);
  double humid = dht.readHumidity();

  if (isnan(temp) || isnan(humid)) {
     Serial.println("Failed to read from DHT sensor");
     temp = -1;
     humid = -1;
  }
  

  
  int m1 = map(ma1, maxMoisture, minMoisture, 0, 100)-14;
  int m2 = map(ma2, maxMoisture, minMoisture, 0, 100)-122;
  int m3 = map(ma3, maxMoisture, minMoisture, 0, 100)-17;
  int m4 = map(ma4, maxMoisture, minMoisture, 0, 100)+9;


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

  if(temp < tempThreshold){
    digitalWrite(RLY_FANS, HIGH);
  }
  else{
    digitalWrite(RLY_FANS, LOW);
  }

  //Sending data to ESP
  String humid1 =(String)humid;
  String temp1 = (String)temp;
  String moisture1 =(String)m1;
  String moisture2 = (String)m2;
  String moisture3 =(String)m3;
  String moisture4 = (String)m4;
  String serialOut = "| "+humid1+" | " +temp1+" | "+m1+"  "+m2+"  "+m3+"  "+m4+"                   ";
  Serial.println(serialOut);
  serialtoESP.print(serialOut);


}


void runWaterPump(int bedNumber){ 
  selectValve(bedNumber);
  delay(1000);
  digitalWrite(RLY_PUMP, HIGH);
  delay(waterTime*1000);
  digitalWrite(RLY_PUMP, LOW);
  selectValve(-1); 
}


void selectValve(int valveNum){
  
  int selectedValve = RLY_VALVE1 + (valveNum-1);

  
  digitalWrite(RLY_VALVE1, LOW);
  digitalWrite(RLY_VALVE2, LOW);
  digitalWrite(RLY_VALVE3, LOW);
  digitalWrite(RLY_VALVE4, LOW);
  
  digitalWrite(selectedValve, HIGH);

}

