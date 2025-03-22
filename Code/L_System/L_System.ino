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

// Memory-efficient parameters
#define MAX_BUFFER 50
#define STACK_SIZE 15
#define MAX_RULES 5
#define MAX_RULE_LENGTH 30
#define MOTOR_SPEED 400

char alphabet[10] = {0};
char axiom[30] = {0};
char rules[MAX_RULES][MAX_RULE_LENGTH] = {{0}};
int depth = 0, angle = 0;
float scale = 100.0;

// Turtle State
float posX = 0, posY = 0;
float dir = 0; // Persistent direction
long pos_xy[2] = {0, 0};

// Stack for push/pop operations
struct State {
    float x, y, direction;
};
State stack[STACK_SIZE];
int stackIndex = 0;

void parseInput(char *input) {
    char *token;
    int ruleIndex = 0;

    // Serial.println("\n[DEBUG] Parsing Input...");
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

    memset(rules, 0, sizeof(rules));
    while ((token = strtok(NULL, ";")) && ruleIndex < MAX_RULES) {
        if (strlen(token) > 2 && token[1] == '=') {
            strncpy(rules[ruleIndex], token, MAX_RULE_LENGTH - 1);
            ruleIndex++;
        }
    }
}


void moveTo(float x, float y, bool draw) {
    //Serial.print("[DEBUG] Moving to: (");
    //Serial.print(x);
    //Serial.print(", ");
    //Serial.print(y);
    //Serial.print(") with pen ");
    //Serial.println(draw ? "DOWN" : "UP");

    pos_xy[0] = (long)x;
    pos_xy[1] = (long)y;

    servo.write(draw ? penDownAngle : penUpAngle);
    XY.moveTo(pos_xy);
    XY.runSpeedToPosition();
    delay(100);
}

void computeAndDrawLSystem(const char *instructions, int index) {
    if (instructions[index] == '\0') {
        return;  // Base case: Stop when reaching the end of the string
    }

    char cmd = instructions[index];

    if (cmd == '+') {  
        dir += angle;
        Serial.print("[DEBUG] Turning +");
        Serial.print(angle);
        Serial.println(" degrees");
    } 
    else if (cmd == '-') {
        dir -= angle;
        Serial.print("[DEBUG] Turning -");
        Serial.print(angle);
        Serial.println(" degrees");
    } 
    else if (strchr(alphabet, cmd) != NULL) {  // If it's a drawable character
        float newX = posX + cos(radians(dir)) * scale;
        float newY = posY + sin(radians(dir)) * scale;

        Serial.print("[DEBUG] Moving to: (");
        Serial.print(newX);
        Serial.print(", ");
        Serial.print(newY);
        Serial.println(") with pen DOWN");

        moveTo(newX, newY, true);
        posX = newX;
        posY = newY;
    } 
    else if (cmd == '[') {
        if (stackIndex < STACK_SIZE) {
            stack[stackIndex++] = {posX, posY, dir};
            Serial.println("[DEBUG] Pushing state to stack.");
        }
    } 
    else if (cmd == ']') {
        if (stackIndex > 0) {
            stackIndex--;
            posX = stack[stackIndex].x;
            posY = stack[stackIndex].y;
            dir = stack[stackIndex].direction;

            Serial.println("[DEBUG] Popping state from stack.");
            moveTo(posX, posY, false);
        }
    }

    // Recursive call for the next character
    computeAndDrawLSystem(instructions, index + 1);
}

void iterativeExpandAndDraw() {
    // Serial.println("\n[DEBUG] Starting Streamed Recursive Drawing...");
    // Serial.print("[DEBUG] Axiom: ");
    // Serial.println(axiom);

    for (int i = 0; axiom[i] != '\0'; i++) {
        Serial.print("[EXPAND] ");
        Serial.println(axiom[i]);
        expandAndDraw(axiom[i], depth);
    }

    Serial.println("[DEBUG] Finished Drawing. Lifting pen...");
    servo.write(penUpAngle);
    delay(1000000);
}



void expandAndDraw(char symbol, int currentDepth) {
    if (currentDepth > 0) {
        for (int i = 0; i < MAX_RULES; i++) {
            if (rules[i][0] == symbol && rules[i][1] == '=') {
                const char* expansion = rules[i] + 2;
                for (int j = 0; expansion[j] != '\0'; j++) {
                    expandAndDraw(expansion[j], currentDepth - 1);
                }
                return;
            }
        }
    }

    if (symbol == '+') {
        dir += angle;
        // Serial.print("[DEBUG] Turning +"); Serial.println(angle);
    } 
    else if (symbol == '-') {
        dir -= angle;
        // Serial.print("[DEBUG] Turning -"); Serial.println(angle);
    } 
    else if (symbol == '[') {
        if (stackIndex < STACK_SIZE) {
            stack[stackIndex++] = {posX, posY, dir};
            // Serial.println("[DEBUG] Pushing state to stack.");
        }
    } 
    else if (symbol == ']') {
        if (stackIndex > 0) {
            stackIndex--;
            posX = stack[stackIndex].x;
            posY = stack[stackIndex].y;
            dir = stack[stackIndex].direction;
            moveTo(posX, posY, false);
            // Serial.println("[DEBUG] Popping state from stack.");
        }
    } 
    else if (strchr(alphabet, symbol) != NULL) {
        float newX = posX + cos(radians(dir)) * scale;
        float newY = posY + sin(radians(dir)) * scale;

        moveTo(newX, newY, true);
        posX = newX;
        posY = newY;

        Serial.print("[POSITION] ");
        Serial.print(posX);
        Serial.print(", ");
        Serial.println(posY);
    }
}




/**
 * Setup function - Initializes motors and serial communication.
 */
void setup() {
    delay(1000);
    Serial.begin(115200);

    X.setMaxSpeed(MOTOR_SPEED);
    Y.setMaxSpeed(MOTOR_SPEED);
    XY.addStepper(X);
    XY.addStepper(Y);

    servo.attach(6);
    servo.write(penUpAngle);

    Serial.println("\nArduino Ready! Enter L-System parameters:");
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
        iterativeExpandAndDraw();
    }
}
