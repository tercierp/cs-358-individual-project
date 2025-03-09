#include <AccelStepper.h>
#include <MultiStepper.h>

#define MotorInterfaceType 4

// Define stepper motors with new pins
AccelStepper X(MotorInterfaceType, 3, 5, 4, 6);
AccelStepper Y(MotorInterfaceType, A0, A2, A1, A3);
MultiStepper XY;

// Position array
long pos_xy[2] = {0, 0};  

void setup() {
    X.setMaxSpeed(500.0);
    Y.setMaxSpeed(500.0);

    XY.addStepper(X);
    XY.addStepper(Y);

    Serial.begin(9600);
    Serial.println("Enter square size:");
}

void loop() {
    // Wait for serial input to get square size
    if (Serial.available()) {
        long squareSize = Serial.parseInt();
        
        Serial.print("Drawing a square of size: ");
        Serial.println(squareSize);

        drawSquare(squareSize);
    }
}

void drawSquare(long size) {
    // Get current position as the starting point
    long startX = X.currentPosition();
    long startY = Y.currentPosition();

    // Define square corners
    long corners[4][2] = {
        {startX + size, startY},     // Move right
        {startX + size, startY + size}, // Move down
        {startX, startY + size},     // Move left
        {startX, startY}             // Move up (back to start)
    };

    for (int i = 0; i < 4; i++) {
        pos_xy[0] = corners[i][0];
        pos_xy[1] = corners[i][1];

        XY.moveTo(pos_xy);
        XY.runSpeedToPosition();  // Move to next corner

        Serial.print("Moved to: X="); Serial.print(pos_xy[0]);
        Serial.print(" Y="); Serial.println(pos_xy[1]);
    }

    Serial.println("Square drawing complete.");
}
