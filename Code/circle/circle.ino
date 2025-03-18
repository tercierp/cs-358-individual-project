#include <AccelStepper.h>
#include <MultiStepper.h>
#define MotorInterfaceType 4

// Define stepper motors
AccelStepper X(MotorInterfaceType, 8, 10, 9, 11);
AccelStepper Y(MotorInterfaceType, 2, 4, 3, 5);
MultiStepper XY;

// Scaling factors for X and Y axes
float scaleX = 1.0;  // Adjust for non-uniform scaling
float scaleY = 1.0;

// Origin offsets
long originX = 0;
long originY = 0;

// Circle parameters
float radius = 2000;  // Set the desired radius
int steps = 100;  // Number of points in the circle (higher = smoother)

long pos_xy[2] = {0, 0};

void setup() {
    X.setMaxSpeed(500.0);
    Y.setMaxSpeed(500.0);

    XY.addStepper(X);
    XY.addStepper(Y);

    Serial.begin(9600);
    Serial.println("Enter scaleX, scaleY, originX, originY, radius:");
}

void loop() {
    // Wait for serial input to set parameters
    if (Serial.available()) {
        scaleX = Serial.parseFloat();
        scaleY = Serial.parseFloat();
        originX = Serial.parseInt();
        originY = Serial.parseInt();
        radius = Serial.parseFloat();
        
        Serial.print("ScaleX: "); Serial.print(scaleX);
        Serial.print(" ScaleY: "); Serial.print(scaleY);
        Serial.print(" OriginX: "); Serial.print(originX);
        Serial.print(" OriginY: "); Serial.print(originY);
        Serial.print(" Radius: "); Serial.println(radius);
        
        drawCircle();
    }
}

void drawCircle() {
    for (int i = 0; i <= steps; i++) {
        float angle = (2 * PI * i) / steps;  // Angle in radians
        pos_xy[0] = originX + (long)(radius * cos(angle) * scaleX);
        pos_xy[1] = originY + (long)(radius * sin(angle) * scaleY);

        XY.moveTo(pos_xy);
        XY.runSpeedToPosition();  // Execute movement

        delay(50);  // Adjust delay for smoother movement
    }
}
