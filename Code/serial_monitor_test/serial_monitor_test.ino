#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Servo.h>

#define MotorInterfaceType 4

AccelStepper X(MotorInterfaceType, 3, 5, 4, 6);
AccelStepper Y(MotorInterfaceType, A0, A2, A1, A3);
MultiStepper XY;

Servo servo;
int penDownAngle = 100;
int penUpAngle = 87;

#define MAX_EXPANSION 500
#define STACK_SIZE 20
#define MAX_RULES 10
#define MAX_RULE_LENGTH 50

char alphabet[10] = {0};
char axiom[50] = {0};
char rules[MAX_RULES][MAX_RULE_LENGTH] = {{0}};
int depth = 0, angle = 0;

void parseInput(char *input) {
    char *token;
    int ruleIndex = 0;

    token = strtok(input, ";");
    if (token) depth = atoi(token);

    token = strtok(NULL, ";");
    if (token) angle = atoi(token);

    token = strtok(NULL, ";");
    if (token) strncpy(alphabet, token, sizeof(alphabet) - 1);

    token = strtok(NULL, ";");
    if (token) strncpy(axiom, token, sizeof(axiom) - 1);

    while ((token = strtok(NULL, ";")) && ruleIndex < MAX_RULES) {
        strncpy(rules[ruleIndex], token, MAX_RULE_LENGTH - 1);
        ruleIndex++;
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Arduino Ready!");
}

void loop() {
    if (Serial.available()) {
        char inputBuffer[200];  // Adjust buffer size based on expected input
        int bytesRead = Serial.readBytesUntil('\n', inputBuffer, sizeof(inputBuffer) - 1);
        inputBuffer[bytesRead] = '\0';  // Null terminate the string

        Serial.println("Received: " + String(inputBuffer));
        parseInput(inputBuffer);

        Serial.print("Depth: "); Serial.println(depth);
        Serial.print("Angle: "); Serial.println(angle);
        Serial.print("Alphabet: "); Serial.println(alphabet);
        Serial.print("Axiom: "); Serial.println(axiom);
        Serial.println("Rules:");
        for (int i = 0; i < MAX_RULES && rules[i][0] != '\0'; i++) {
            Serial.print("- Rule "); Serial.print(i + 1); Serial.print(": "); Serial.println(rules[i]);
        }
    }
}
