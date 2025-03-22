#include <Servo.h>
Servo servo;
int angle = 90;
int delta = 3;
void setup() {
  servo.attach(6);
  servo.write(angle);
}
void loop() 
{ 
  pendown();
}
void penup() {
  servo.write(angle-delta);
}
void pendown() {
  servo.write(angle+delta+7);
  
}
