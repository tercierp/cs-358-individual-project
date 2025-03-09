#include <AccelStepper.h>
#include <MultiStepper.h>

#define MotorInterfaceType 4

// Define stepper motors with new pins
AccelStepper X(MotorInterfaceType, 3, 5, 4, 6);
AccelStepper Y(MotorInterfaceType, A0, A2, A1, A3);
MultiStepper XY;

// Position array for movement
long pos_xy[2] = {0, 0};

void setup() {
    X.setMaxSpeed(300.0);
    Y.setMaxSpeed(300.0);

    XY.addStepper(X);
    XY.addStepper(Y);

    Serial.begin(9600);
    Serial.println("Enter square size:");
}

void loop() {
    // Wait for serial input to get square size
    if (Serial.available()) {
        long squareSize = Serial.parseInt();
        
        Serial.print("Drawing square of size: ");
        Serial.println(squareSize);

        drawSquareThenCircle(squareSize);
    }
}

void drawSquareThenCircle(long size) {
    // Compute circle radius (must fit perfectly inside the square)
    long radius = size / 2;

    // Get current position as the starting point
    long startX = X.currentPosition();
    long startY = Y.currentPosition();

    // Start from the middle
    long midRightX = startX + size / 2;

    // Define square corners, starting from middle-right
    long corners[5][2] = {
        {startX + size, startY},
        {startX + size, startY + size},
        {startX, startY + size},
        {startX, startY},
        {midRightX, startY}
    };

    for (int i = 0; i < 5; i++) {
        pos_xy[0] = corners[i][0];
        pos_xy[1] = corners[i][1];

        XY.moveTo(pos_xy);
        XY.runSpeedToPosition();  // Move to next corner

        Serial.print("Moved to: X="); Serial.print(pos_xy[0]);
        Serial.print(" Y="); Serial.println(pos_xy[1]);
    }

    Serial.println("Square complete. Starting circle...");

    // Compute circle center (perfectly inside the square)
    long centerX = startX + radius;
    long centerY = startY + radius;

    // Draw the perfect circle inside the square
    drawCircle(centerX, centerY, radius);
}

void drawCircle(long centerX, long centerY, long radius) {
    int steps = 100;  // Number of points in the circle (higher = smoother)

    for (int i = 0; i <= steps; i++) {
        float angle = (2 * PI * i) / steps - PI/2;  // Angle in radians
        pos_xy[0] = centerX + (long)(radius * cos(angle));
        pos_xy[1] = centerY + (long)(radius * sin(angle));

        XY.moveTo(pos_xy);
        XY.runSpeedToPosition();  // Execute movement

        Serial.print("Moved to: X="); Serial.print(pos_xy[0]);
        Serial.print(" Y="); Serial.println(pos_xy[1]);

        delay(50);  // Adjust delay for smoother movement
    }

    Serial.println("Circle complete.");
}
