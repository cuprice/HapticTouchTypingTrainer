#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp;

const int ROWS = 3;
const int COLS = 10;

const int rowPins[ROWS] = {2, 3, 4};
const int colPins[COLS] = {5, 6, 7, 8, 9, 13, 14, 15, 16, 17};

int motorPins[8] = {0, 1, 2, 3, 4, 5, 6, 7};

// ★ブザー（Arduinoピン）
const int buzzerPin = 10;

const int led = 9;

// 音階
#define DO 262
#define MI 330
#define SO 392
#define LOW_BUZZ 130

int motorOrder[] = {
    0,1,2,3,4,5,6,7,8,9,
    0,1,2,3,4,5,6,7,8,9,
    0,1,2,3,4,5,6,7,8,9
};

int keyRowOrder[] = {
    0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2
};

const int TOTAL_SEQUENCE_LENGTH = sizeof(motorOrder) / sizeof(motorOrder[0]);

int currentMotorPlace = 0;
bool programEnd = false;

void setup() {
    Serial.begin(9600);
    mcp.begin_I2C(0x20);

    pinMode(buzzerPin, OUTPUT);

    mcp.pinMode(led, OUTPUT);
    mcp.digitalWrite(led, HIGH);

    for (int i = 0; i < 8; i++) {
        mcp.pinMode(motorPins[i], OUTPUT);
    }

    for (int i = 0; i < ROWS; i++) {
        pinMode(rowPins[i], INPUT_PULLUP);
    }

    for (int j = 0; j < COLS; j++) {
        pinMode(colPins[j], OUTPUT);
        digitalWrite(colPins[j], HIGH);
    }
}

// 基本音
void playTone(int freq, int duration) {
    tone(buzzerPin, freq);
    delay(duration);
    noTone(buzzerPin);
    delay(20);
}

// 段通知（ド）
void beep(int duration) {
    playTone(DO, duration);
}

// ★正解音（ドミソ）
void playCorrectSound() {
    playTone(DO, 80);
    playTone(MI, 80);
    playTone(SO, 120);
}

// ★ミス音（低音ブー）
void playErrorSound() {
    tone(buzzerPin, LOW_BUZZ);
    delay(400);
    noTone(buzzerPin);
    delay(50);
}

void multipleBeeps(int count) {
    for (int k = 0; k < count; k++) {
        beep(100);
    }
}

int getMotorPinIndex(int expectedCol) {
    if (expectedCol >= 0 && expectedCol <= 3) {
        return expectedCol;
    } else if (expectedCol >= 6 && expectedCol <= 9) {
        return expectedCol - 2;
    } else if (expectedCol == 4) {
        return 3;
    } else if (expectedCol == 5) {
        return 4;
    }
    return -1;
}

void loop() {
    if (programEnd) return;

    int expectedCol = motorOrder[currentMotorPlace];
    int expectedRow = keyRowOrder[currentMotorPlace];

    int motorIndex = getMotorPinIndex(expectedCol);

    // 段通知
    switch (expectedRow) {
        case 0: multipleBeeps(3); break;
        case 1: multipleBeeps(2); break;
        case 2: multipleBeeps(1); break;
    }

    // モーター
    if (motorIndex != -1) {
        if (expectedCol == 4 || expectedCol == 5) {
            mcp.digitalWrite(motorPins[motorIndex], HIGH);
            delay(300);
            mcp.digitalWrite(motorPins[motorIndex], LOW);
            delay(300);
            mcp.digitalWrite(motorPins[motorIndex], HIGH);
            delay(300);
        } else {
            mcp.digitalWrite(motorPins[motorIndex], HIGH);
            delay(200);
            mcp.digitalWrite(motorPins[motorIndex], LOW);
            delay(200);
        }
    }

    // キースキャン
    for (int j = 0; j < COLS; j++) {
        digitalWrite(colPins[j], LOW);

        for (int i = 0; i < ROWS; i++) {
            if (digitalRead(rowPins[i]) == LOW) {

                if (i == expectedRow && j == expectedCol) {
                    currentMotorPlace++;

                    // ★正解音
                    playCorrectSound();

                    if (currentMotorPlace >= TOTAL_SEQUENCE_LENGTH) {
                        programEnd = true;

                        // 終了音（ちょっと豪華）
                        playCorrectSound();
                        delay(200);
                        playCorrectSound();

                        mcp.digitalWrite(led, LOW);
                    }

                    digitalWrite(colPins[j], HIGH);
                    return;
                } else {
                    // ★ミス音
                    playErrorSound();
                }
            }
        }
        digitalWrite(colPins[j], HIGH);
    }
}