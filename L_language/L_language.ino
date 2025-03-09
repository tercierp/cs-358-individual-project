#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Servo.h>

#define MotorInterfaceType 4

// Stepper motor control for X and Y
AccelStepper X(MotorInterfaceType, 3, 5, 4, 6);
AccelStepper Y(MotorInterfaceType, A0, A2, A1, A3);
MultiStepper XY;

// Servo motor for pen control
Servo servo;
int penDownAngle = 100;
int penUpAngle = 80;

// L-System parameters
#define MAX_EXPANSION 500
#define STACK_SIZE 20
char alphabet[10];
char axiom[50];
char rules[10][50];
int depth, angle;

// Turtle Graphics State
float posX = 0, posY = 0;
float dir = 0;
long pos_xy[2] = {0, 0};

// Stack for push/pop operations
float stackX[STACK_SIZE], stackY[STACK_SIZE], stackDir[STACK_SIZE];
int stackIndex = 0;

// Expands L-System string
void expandLSystem(char *result) {
    char current[MAX_EXPANSION] = "";
    strcpy(current, axiom);

    for (int i = 0; i < depth; i++) {
        char next[MAX_EXPANSION] = "";
        for (int j = 0; current[j] != '\0'; j++) {
            bool replaced = false;
            for (int k = 0; alphabet[k] != '\0'; k++) {
                if (current[j] == alphabet[k]) {
                    strcat(next, rules[k]);
                    replaced = true;
                    break;
                }
            }
            if (!replaced) {
                strncat(next, &current[j], 1);
            }
        }
        strcpy(current, next);
    }
    strcpy(result, current);
}

// Moves stepper motors to a position
void moveTo(float x, float y, bool draw) {
    pos_xy[0] = (long)x;
    pos_xy[1] = (long)y;

    if (draw) {
        servo.write(penDownAngle);
    } else {
        servo.write(penUpAngle);
    }

    XY.moveTo(pos_xy);
    XY.runSpeedToPosition();
}

// Draws the L-System path
void drawLSystem(const char *instructions) {
    posX = 0; posY = 0; dir = 0;
    moveTo(posX, posY, false);

    for (int i = 0; instructions[i] != '\0'; i++) {
        char cmd = instructions[i];

        if (cmd == 'F' || cmd == 'G') {
            float newX = posX + cos(radians(dir)) * 10;
            float newY = posY + sin(radians(dir)) * 10;
            moveTo(newX, newY, true);
            posX = newX;
            posY = newY;
        } 
        else if (cmd == '+') {
            dir += angle;
        } 
        else if (cmd == '-') {
            dir -= angle;
        } 
        else if (cmd == '[') {
            if (stackIndex < STACK_SIZE) {
                stackX[stackIndex] = posX;
                stackY[stackIndex] = posY;
                stackDir[stackIndex] = dir;
                stackIndex++;
            }
        } 
        else if (cmd == ']') {
            if (stackIndex > 0) {
                stackIndex--;
                posX = stackX[stackIndex];
                posY = stackY[stackIndex];
                dir = stackDir[stackIndex];
                moveTo(posX, posY, false);
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    
    // Initialize motors
    X.setMaxSpeed(300.0);
    Y.setMaxSpeed(300.0);
    XY.addStepper(X);
    XY.addStepper(Y);

    // Initialize servo
    servo.attach(9);
    servo.write(penUpAngle);

    Serial.println("Enter L-System parameters:");
}

void loop() {
    if (Serial.available()) {
        char input[100];
        Serial.readBytesUntil('\n', input, sizeof(input) - 1);
        input[strlen(input)] = '\0';

        // Parse input format: 4;120;FG;F-G-G;F-G+F+G-F;GG
        sscanf(input, "%d;%d;%[^;];%[^;];%[^;];%s", &depth, &angle, alphabet, axiom, rules[0], rules[1]);

        Serial.println("Expanding L-System...");
        char expanded[MAX_EXPANSION] = "";
        expandLSystem(expanded);

        Serial.println("Drawing L-System...");
        drawLSystem(expanded);
    }
}
