//Arduino code
#include <SoftwareSerial.h>
SoftwareSerial s(3,2);
 
void setup() {
  s.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}
 
void loop() {
  int data=50;

  digitalWrite(LED_BUILTIN, !digitalRead(13));
  s.write(data);
   
  delay(500);
}
