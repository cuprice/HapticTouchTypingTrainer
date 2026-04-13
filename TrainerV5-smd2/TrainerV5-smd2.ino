#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp;

const int ROWS = 3;
const int COLS = 10;
const int resetPin = 8;

const int rowPins[ROWS] = {2, 3, 4};
const int colPins[COLS] = {5, 6, 7, 8, 9, 13, 14, 15, 16, 17};

int motorPins[8] = {0, 1, 2, 3, 4, 5, 6, 7};

// ブザー
const int buzzerPin = 10;
const int led = 9;

// ===== タイプ切り替えスイッチ（GPB3）=====
const int typeSwitchPin = 11;
bool lastSwitchState = HIGH;

// ===== 音階 =====
#define DO 262
#define MI 330
#define SO 392
#define LOW_BUZZ 130

// ===== 4タイプ配列 =====
// 赤坂の朝日が坂を上がる光を探す
int motorOrder1[] = {
    0,7,0,1,0,7,0,5,8,0,1,0,5,7,4,0,1,0,7,0,1,8,0,4,0,3,6,5,7,7,0,3,7,1,8,1,0,4,0,1,6
};

int keyRowOrder1[] = {
    1,1,1,1,1,1,1,2,0,1,1,1,1,0,1,1,1,1,1,1,0,0,1,1,1,0,0,1,0,1,1,0,0,0,0,1,1,1,1,1,0
};

// 朝日が差す公園で子どもたちが遊ぶ
int motorOrder2[] = {
    0,1,0,5,7,4,0,1,0,1,6,7,8,6,2,5,2,2,7,8,2,8,6,8,4,0,4,7,4,0,0,1,8,4,6
};

int keyRowOrder2[] = {
    1,1,1,1,0,1,1,1,1,1,0,1,0,0,0,2,1,0,1,0,1,0,2,0,0,1,0,0,1,1,1,1,0,2,0
};

// 魚が川を泳ぎながら水の中で遊ぶ
int motorOrder3[] = {
    1,0,7,0,5,0,4,0,7,0,1,0,1,8,8,5,8,4,7,5,0,4,0,3,0,6,7,0,6,5,8,5,0,7,0,2,2,0,1,8,4,6
};

int keyRowOrder3[] = {
    1,1,1,1,2,1,1,1,1,1,0,1,0,0,0,0,0,1,0,2,1,1,1,0,1,2,0,2,0,2,0,2,1,1,1,1,0,1,1,0,2,0
};

// 今日は天気が良くて公園でゆっくり散歩した
int motorOrder4[] = {
    7,5,8,6,5,0,4,2,5,7,7,4,0,5,8,7,6,4,2,7,8,6,2,5,2,2,5,6,7,7,6,3,7,1,0,5,9,8,1,7,4,0
};

int keyRowOrder4[] = {
    1,0,0,0,1,1,0,0,2,1,0,1,1,0,0,1,0,0,0,1,0,0,0,2,1,0,0,0,1,1,0,0,0,1,1,2,0,0,1,0,0,1
};

// ===== ポインタ管理 =====
int* motorOrder;
int* keyRowOrder;
int TOTAL_SEQUENCE_LENGTH;

// 長さ
int sequenceLengths[] = {18, 20, 22, 27};

int currentType = 0;

int currentMotorPlace = 0;
bool programEnd = false;

// ===== 初期設定 =====
void setType(int type) {
    currentType = type;

    switch (type) {
        case 0:
            motorOrder = motorOrder1;
            keyRowOrder = keyRowOrder1;
            break;
        case 1:
            motorOrder = motorOrder2;
            keyRowOrder = keyRowOrder2;
            break;
        case 2:
            motorOrder = motorOrder3;
            keyRowOrder = keyRowOrder3;
            break;
        case 3:
            motorOrder = motorOrder4;
            keyRowOrder = keyRowOrder4;
            break;
    }

    TOTAL_SEQUENCE_LENGTH = sequenceLengths[type];

    currentMotorPlace = 0;
    programEnd = false;
}

// ===== setup =====
void setup() {
    Serial.begin(9600);
    mcp.begin_I2C(0x20);

    pinMode(buzzerPin, OUTPUT);

    mcp.pinMode(resetPin, INPUT_PULLUP);
    mcp.pinMode(typeSwitchPin, INPUT_PULLUP);

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

    // 初期タイプ
    setType(0);
}

// ===== 音関連 =====
void playTone(int freq, int duration) {
    tone(buzzerPin, freq);
    delay(duration);
    noTone(buzzerPin);
    delay(20);
}

void beep(int duration) {
    playTone(DO, duration);
}

void playCorrectSound() {
    playTone(DO, 80);
    playTone(MI, 80);
    playTone(SO, 120);
}

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

// ===== モーター変換 =====
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

// ===== loop =====
void loop() {

    // ===== タイプ切り替え =====
    bool currentSwitchState = mcp.digitalRead(typeSwitchPin);

    if (lastSwitchState == HIGH && currentSwitchState == LOW) {

        currentType++;
        if (currentType >= 4) currentType = 0;

        setType(currentType);

        beep(150); // 切替音
        delay(300);
    }

    lastSwitchState = currentSwitchState;

    // ===== リセット =====
    if (mcp.digitalRead(resetPin) == LOW) {
        currentMotorPlace = 0;
        programEnd = false;
        playErrorSound();
        delay(300);
        return;
    }

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

                    playCorrectSound();

                    if (currentMotorPlace >= TOTAL_SEQUENCE_LENGTH) {
                        programEnd = true;

                        playCorrectSound();
                        delay(200);
                        playCorrectSound();

                        mcp.digitalWrite(led, LOW);
                    }

                    digitalWrite(colPins[j], HIGH);
                    return;
                } else {
                    playErrorSound();
                }
            }
        }
        digitalWrite(colPins[j], HIGH);
    }
}