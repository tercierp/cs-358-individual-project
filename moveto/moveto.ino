#include <AccelStepper.h>
#include <MultiStepper.h>

#define MotorInterfaceType 4

// Define stepper motors with new pins
AccelStepper X(MotorInterfaceType, 3, 5, 4, 6);
AccelStepper Y(MotorInterfaceType, A0, A2, A1, A3);
MultiStepper XY;

// Position array
long pos_xy[2] = {0, 0};
bool isDone = true;  // Flag to ensure movement happens only once

void setup() {
    Serial.begin(9600);
    Serial.println("Enter target X, Y position:");

    X.setMaxSpeed(500.0);
    Y.setMaxSpeed(500.0);

    XY.addStepper(X);
    XY.addStepper(Y);
}

void loop() {
    if (Serial.available()) {
        delay(10);  // Small delay to ensure complete data reception

        long endX = Serial.parseInt();
        long endY = Serial.parseInt();

        // Ignore invalid input (empty input or only newlines)
        if (endX == 0 && endY == 0) {
            Serial.println("Ignoring invalid input...");
            Serial.read();  // Clear any extra characters
            return;
        }

        Serial.print("Received target -> X: "); Serial.print(endX);
        Serial.print(" Y: "); Serial.println(endY);

        isDone = false;  // Enable movement
        moveToPosition(endX, endY);
    }
}

void moveToPosition(long endX, long endY) {
    if (!isDone) {  // Move only if not already done
        pos_xy[0] = endX;
        pos_xy[1] = endY;

        XY.moveTo(pos_xy);
        XY.runSpeedToPosition();  // Move to target position

        isDone = true;  // Set flag to prevent re-running
        Serial.println("Movement complete. Waiting for new target...");
    }
}
