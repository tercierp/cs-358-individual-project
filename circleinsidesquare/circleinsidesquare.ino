#include <Servo.h>
#include <AccelStepper.h>
#include <MultiStepper.h>

#define MotorInterfaceType 4

// Servo setup
Servo servo;
int angle = 90;
int delta = 3;

// Define stepper motors with new pins
AccelStepper X(MotorInterfaceType, 2, 4, 3, 5);
AccelStepper Y(MotorInterfaceType, 8, 10, 9, 11);
MultiStepper XY;

// Position array for movement
long pos_xy[2] = {0, 0};

void setup() {
    // Initialize servo and set to initial (pen up) position
    servo.attach(6);
    penup(); // Ensure the pen starts in the up position

    // Set up stepper motors
    X.setMaxSpeed(400.0);
    Y.setMaxSpeed(400.0);
    XY.addStepper(X);
    XY.addStepper(Y);

    Serial.begin(9600);
    Serial.println("Enter square size:");
}

void loop() {
    penup(); // Keep pen up while waiting for input

    // Wait for serial input to get square size
    if (Serial.available()) {
        long squareSize = Serial.parseInt();
        
        Serial.print("Drawing square of size: ");
        Serial.println(squareSize);

        drawSquareThenCircle(squareSize);
        
        // Ensure pen stays up at the end of the drawing
        penup();
    }
}

void drawSquareThenCircle(long size) {
    // Lower the pen to begin drawing
    pendown();
    
    // Compute circle radius (circle fits perfectly inside the square)
    long radius = size / 2;

    // Get current position as the starting point
    long startX = X.currentPosition();
    long startY = Y.currentPosition();

    // Calculate the middle of the right side of the square (starting point)
    long midRightX = startX + size / 2;

    // Define square corners, starting from middle-right and ending back there
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
        XY.runSpeedToPosition();  // Move to the next corner

        Serial.print("Moved to: X=");
        Serial.print(pos_xy[0]);
        Serial.print(" Y=");
        Serial.println(pos_xy[1]);
    }

    Serial.println("Square complete. Starting circle...");

    // Compute circle center (perfectly inside the square)
    long centerX = startX + radius;
    long centerY = startY + radius;

    // Draw the circle inside the square
    drawCircle(centerX, centerY, radius);

    // Lift the pen after drawing is complete
    penup();
    delay(10000000);
}

void drawCircle(long centerX, long centerY, long radius) {
    int steps = 400;  // More points means a smoother circle

    for (int i = 0; i <= steps; i++) {
        float angle_rad = (2 * PI * i) / steps - PI / 2;  // Calculate the angle in radians
        pos_xy[0] = centerX + (long)(radius * cos(angle_rad));
        pos_xy[1] = centerY + (long)(radius * sin(angle_rad));

        XY.moveTo(pos_xy);
        XY.runSpeedToPosition();  // Execute the movement

        Serial.print("Moved to: X=");
        Serial.print(pos_xy[0]);
        Serial.print(" Y=");
        Serial.println(pos_xy[1]);

        delay(50);  // Adjust delay for smoother movement
    }

    Serial.println("Circle complete.");
}

void penup() {
    // Lift the pen by setting the servo to a higher position
    servo.write(angle - delta);
    delay(500); // Delay to allow the servo to move
}

void pendown() {
    // Lower the pen by setting the servo to a lower position
    servo.write(angle + delta + 7);
    delay(500); // Delay to allow the servo to move
}
