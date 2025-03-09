#include <Servo.h>
Servo servo;
int angle = 90;
int delta = 15;
void setup() {
  servo.attach(9);
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
