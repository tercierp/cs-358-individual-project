#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Servo.h>

#define MotorInterfaceType 4

// Stepper motor control for X and Y
AccelStepper X(MotorInterfaceType, 2, 4, 3, 5);
AccelStepper Y(MotorInterfaceType, 8, 10, 9, 11);
MultiStepper XY;

// Servo motor for pen control
Servo servo;
int penDownAngle = 100;
int penUpAngle = 87;

// Optimized L-System parameters
#define MAX_EXPANSION 100
#define STACK_SIZE 15
#define MAX_RULES 5
#define MAX_RULE_LENGTH 30

char alphabet[10] = {0};
char axiom[30] = {0};
char rules[MAX_RULES][MAX_RULE_LENGTH] = {{0}};
int depth = 0, angle = 0;
float scale = 50.0;  // Step size for movements

// Turtle Graphics State
float posX = 0, posY = 0;
float dir = 0;
long pos_xy[2] = {0, 0};

// Stack for push/pop operations
struct State {
    float x, y, direction;
};
State stack[STACK_SIZE];
int stackIndex = 0;

/**
 * Parses input safely with reduced memory usage.
 * Format: depth;angle;scale;alphabet;axiom;rule1;rule2;...
 */
void parseInput(char *input) {
    char *token;
    int ruleIndex = 0;

    token = strtok(input, ";");
    if (token) depth = atoi(token);

    token = strtok(NULL, ";");
    if (token) angle = atoi(token);

    token = strtok(NULL, ";");
    if (token) scale = atof(token);

    token = strtok(NULL, ";");
    if (token) strncpy(alphabet, token, sizeof(alphabet) - 1);

    token = strtok(NULL, ";");
    if (token) strncpy(axiom, token, sizeof(axiom) - 1);

    while ((token = strtok(NULL, ";")) && ruleIndex < MAX_RULES) {
        strncpy(rules[ruleIndex], token, MAX_RULE_LENGTH - 1);
        ruleIndex++;
    }
}

/**
 * Expands the L-System step-by-step to avoid large memory usage.
 */
void expandLSystem(char *result) {
    char buffer[MAX_EXPANSION] = "";
    strcpy(buffer, axiom);

    for (int i = 0; i < depth; i++) {
        char next[MAX_EXPANSION] = "";
        int nextLen = 0;

        for (int j = 0; buffer[j] != '\0'; j++) {
            bool replaced = false;
            for (int k = 0; alphabet[k] != '\0'; k++) {
                if (buffer[j] == alphabet[k]) {
                    int ruleLen = strlen(rules[k]);
                    if (nextLen + ruleLen < MAX_EXPANSION - 1) {
                        strcat(next, rules[k]);
                        nextLen += ruleLen;
                    }
                    replaced = true;
                    break;
                }
            }
            if (!replaced && nextLen < MAX_EXPANSION - 1) {
                strncat(next, &buffer[j], 1);
                nextLen++;
            }
        }
        strcpy(buffer, next);
    }

    strncpy(result, buffer, MAX_EXPANSION - 1);
}

/**
 * Moves stepper motors safely and prints the coordinates.
 */
void moveTo(float x, float y, bool draw) {
    char buffer[50];
    sprintf(buffer, "Moving from (%.1f, %.1f) to (%.1f, %.1f)", posX, posY, x, y);
    Serial.println(buffer);

    pos_xy[0] = (long)x;
    pos_xy[1] = (long)y;

    servo.write(draw ? penDownAngle : penUpAngle);

    XY.moveTo(pos_xy);
    XY.runSpeedToPosition();

    sprintf(buffer, "Arrived at (%.1f, %.1f)\n", x, y);
    Serial.println(buffer);
}

/**
 * Draws the L-System path with optimized memory handling.
 */
void drawLSystem(const char *instructions) {
    posX = 0;
    posY = 0;
    dir = 0;
    moveTo(posX, posY, false);

    for (int i = 0; instructions[i] != '\0'; i++) {
        char cmd = instructions[i];

        if (cmd == 'F' || cmd == 'G') {
            float newX = posX + cos(radians(dir)) * scale;
            float newY = posY + sin(radians(dir)) * scale;
            moveTo(newX, newY, true);
            posX = newX;
            posY = newY;
        } else if (cmd == '+') {
            dir += angle;
            char buffer[30];
            sprintf(buffer, "Turning +%d degrees", angle);
            Serial.println(buffer);
        } else if (cmd == '-') {
            dir -= angle;
            char buffer[30];
            sprintf(buffer, "Turning -%d degrees", angle);
            Serial.println(buffer);
        } else if (cmd == '[') {
            if (stackIndex < STACK_SIZE) {
                stack[stackIndex++] = {posX, posY, dir};
            }
        } else if (cmd == ']') {
            if (stackIndex > 0) {
                State restored = stack[--stackIndex];
                posX = restored.x;
                posY = restored.y;
                dir = restored.direction;
                moveTo(posX, posY, false);
            }
        }
    }
}

/**
 * Setup function - Initializes motors and serial communication.
 */
void setup() {
    delay(1000);
    Serial.begin(115200);

    // Initialize motors
    X.setMaxSpeed(400.0);
    Y.setMaxSpeed(400.0);
    XY.addStepper(X);
    XY.addStepper(Y);

    // Initialize servo
    servo.attach(6);
    servo.write(penUpAngle);

    Serial.println("Arduino Ready! Enter L-System parameters:");
}

/**
 * Loop function - Receives input and processes L-System instructions.
 */
void loop() {
    if (Serial.available()) {
        char inputBuffer[100];
        int bytesRead = Serial.readBytesUntil('\n', inputBuffer, sizeof(inputBuffer) - 1);
        inputBuffer[bytesRead] = '\0';

        parseInput(inputBuffer);

        char expanded[MAX_EXPANSION] = "";
        expandLSystem(expanded);

        drawLSystem(expanded);
    }
}
