#include <AccelStepper.h>
#include <MultiStepper.h>

#define MotorInterfaceType 4

// Define stepper motors with new pins
AccelStepper X(MotorInterfaceType, 3, 5, 4, 6);
AccelStepper Y(MotorInterfaceType, A0, A2, A1, A3);
MultiStepper XY;

long pos_xy[2] = {0, 0};  // Position array for MultiStepper

// Start and end positions
long startX = 0;
long startY = 0;
long endX = 100;  // Change this for different movement distances
long endY = 100;

void setup() {
    X.setMaxSpeed(500.0);
    Y.setMaxSpeed(500.0);

    XY.addStepper(X);
    XY.addStepper(Y);

    Serial.begin(9600);
    Serial.println("Enter startX, startY, endX, endY:");
}

void loop() {
    // Wait for serial input to set parameters
    if (Serial.available()) {
        startX = Serial.parseInt();
        startY = Serial.parseInt();
        endX = Serial.parseInt();
        endY = Serial.parseInt();
        
        Serial.print("StartX: "); Serial.print(startX);
        Serial.print(" StartY: "); Serial.print(startY);
        Serial.print(" EndX: "); Serial.print(endX);
        Serial.print(" EndY: "); Serial.println(endY);
        

        moveStraightLine();
    }
}

void moveStraightLine() {
    pos_xy[0] = startX;
    pos_xy[1] = startY;
    XY.moveTo(pos_xy);
    XY.runSpeedToPosition();  // Move to start position

    pos_xy[0] = endX;
    pos_xy[1] = endY;
    XY.moveTo(pos_xy);
    XY.runSpeedToPosition();  // Move to end position

    delay(5000);  // Wait before allowing another movement
}
