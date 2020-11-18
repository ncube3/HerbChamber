// Relay Control Pins
const int RLY_PUMP        = 10;
const int RLY_FANS        = 4;
const int RLY_NUTRIENTS   = 9;
const int RLY_LIGHT       = 11;
const int RLY_VALVE1      = 5;
const int RLY_VALVE2      = 6;
const int RLY_VALVE3      = 7;
const int RLY_VALVE4      = 8;

// Analog Sensor Pins
const int SENSOR_MOISTURE1 = A1;
const int SENSOR_MOISTURE2 = A2;
const int SENSOR_MOISTURE3 = A3;
const int SENSOR_MOISTURE4 = A4;

const int SENSOR_TEMPERATURE = A5;
const int SENSOR_HUMIDITY = 12;


// Constants

const int waterTime = 28;
const int nutrientTime = 5;

const int moistureThreshold = 25;
const int temperatureThreshold = 80;

const int inputProcessInterval = 10;


void setup() {

  Serial.begin(9600); 

  pinMode(RLY_PUMP, OUTPUT);
  pinMode(RLY_FANS, OUTPUT);
  pinMode(RLY_NUTRIENTS, OUTPUT);
  pinMode(RLY_LIGHT, OUTPUT);
  pinMode(RLY_VALVE1, OUTPUT);
  pinMode(RLY_VALVE2, OUTPUT);
  pinMode(RLY_VALVE3, OUTPUT);
  pinMode(RLY_VALVE4, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  initialTest();

  digitalWrite(RLY_LIGHT, HIGH);
  
  
}


void loop() {
  //processSensorInputs();
  delay(inputProcessInterval*1000);
}



void processSensorInputs(){

  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);  
  
  int m1 = analogRead(SENSOR_MOISTURE1);
  int m2 = analogRead(SENSOR_MOISTURE2);
  int m3 = analogRead(SENSOR_MOISTURE3);
  int m4 = analogRead(SENSOR_MOISTURE4);
  
  int temp = analogRead(SENSOR_TEMPERATURE);
  int humid = analogRead(SENSOR_HUMIDITY);

  Serial.println("MOISTURE1: " + m1);
  Serial.println("MOISTURE2: " + m1);
  Serial.println("MOISTURE3: " + m1);
  Serial.println("MOISTURE4: " + m1);
  
  Serial.println("TEMP: " + temp);
  Serial.println("HUMIDITY: " + humid);

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

  if(temp < 80){
    digitalWrite(RLY_FANS, HIGH);
  }

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


  for(int rly = 5; rly <= 11; rly++){
    digitalWrite(rly, HIGH);
    delay(2000);
    digitalWrite(rly, LOW);
  }
}
