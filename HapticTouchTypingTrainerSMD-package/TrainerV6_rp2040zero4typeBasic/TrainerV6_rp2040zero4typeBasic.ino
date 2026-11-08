#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp;

const int ROWS = 3;
const int COLS = 10;
const int resetPin = 8; // ResetピンはMCP23017経由

const int rowPins[ROWS] = {0, 1, 2};
const int colPins[COLS] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

int motorPins[8] = {0, 1, 2, 3, 6, 7, 8, 9};

// ブザー（Arduinoピン）
const int buzzerPin = 10;

const int led1 = 14;
const int led2 = 15;

// ===== タイプ切り替えスイッチ（MCP23017のGPB5等）=====
const int typeSwitchPin = 13;
bool lastSwitchState = HIGH;

// 音階
#define DO 262
#define MI 330
#define SO 392
#define LOW_BUZZ 130

// ===== 4タイプ配列 =====
int motorOrder1[] = {
    0,1,2,3,4,5,6,7,8,9,
    0,1,2,3,4,5,6,7,8,9,
    0,1,2,3,4,5,6,7,8,9
};

int keyRowOrder1[] = {
    0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2
};

int motorOrder2[] = {
    0,1,0,5,7,4,0,1,0,1,6,7,8,6,2,5,2,2,7,8,2,8,6,8,4,0,4,7,4,0,0,1,8,4,6
};
int keyRowOrder2[] = {
    1,1,1,1,0,1,1,1,1,1,0,1,0,0,0,2,1,0,1,0,1,0,2,0,0,1,0,0,1,1,1,1,0,2,0
};

int motorOrder3[] = {
    1,0,7,0,5,0,4,0,7,0,1,0,1,8,8,5,8,4,7,5,0,4,0,3,0,6,7,0,6,5,8,5,0,7,0,2,2,0,1,8,4,6
};
int keyRowOrder3[] = {
    1,1,1,1,2,1,1,1,1,1,0,1,0,0,0,0,0,1,0,2,1,1,1,0,1,2,0,2,0,2,0,2,1,1,1,1,0,1,1,0,2,0
};

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

int sequenceLengths[] = {
    sizeof(motorOrder1) / sizeof(motorOrder1[0]),
    sizeof(motorOrder2) / sizeof(motorOrder2[0]),
    sizeof(motorOrder3) / sizeof(motorOrder3[0]),
    sizeof(motorOrder4) / sizeof(motorOrder4[0])
};

int currentType = 0;
int currentMotorPlace = 0;
bool programEnd = false;

// リピート通知タイマー用
unsigned long lastGuideTime = 0;
const unsigned long REPEAT_INTERVAL = 1000; // ガイド（音＋振動）を繰返す間隔（1秒）

// ★LED2 タイプ通知用変数
unsigned long lastLed2Time = 0;
int led2BlinkCount = 0;
bool led2State = false;

// ===== タイプ設定関数 =====
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
    lastGuideTime = 0; // すぐに初回通知が鳴るようにクリア

    // ★LED2の点滅状態をリセット
    led2BlinkCount = 0;
    led2State = false;
    mcp.digitalWrite(led2, LOW);
    lastLed2Time = millis();
}

void setup() {
    Serial.begin(9600);

    Wire.setSDA(4);
    Wire.setSCL(5);
    Wire.begin();
    Wire.setClock(100000);

    if (!mcp.begin_I2C(0x20, &Wire)) {
        Serial.println("MCP23017が見つかりません。");
        while (1);
    }

    pinMode(buzzerPin, OUTPUT);

    mcp.pinMode(resetPin, INPUT_PULLUP);
    mcp.pinMode(typeSwitchPin, INPUT_PULLUP);

    mcp.pinMode(led1, OUTPUT);
    mcp.pinMode(led2, OUTPUT);
    mcp.digitalWrite(led1, HIGH);
    mcp.digitalWrite(led2, LOW);

    for (int i = 0; i < 8; i++) {
        pinMode(motorPins[i], OUTPUT);
    }

    for (int i = 0; i < ROWS; i++) {
        mcp.pinMode(rowPins[i], INPUT_PULLUP);
    }

    for (int j = 0; j < COLS; j++) {
        mcp.pinMode(colPins[j], OUTPUT);
        mcp.digitalWrite(colPins[j], HIGH);
    }

    setType(0);
}

// 基本音
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
    delay(200);
    noTone(buzzerPin);
    delay(20);
}

void multipleBeeps(int count) {
    for (int k = 0; k < count; k++) {
        beep(80);
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

// ===== ガイド（ブザー音 ＋ モーター振動）実行関数 =====
void playGuide(int expectedRow, int expectedCol) {
    // 1. 段通知（ブザー）
    switch (expectedRow) {
        case 0: multipleBeeps(3); break;
        case 1: multipleBeeps(2); break;
        case 2: multipleBeeps(1); break;
    }

    // 2. モーター駆動
    int motorIndex = getMotorPinIndex(expectedCol);
    if (motorIndex != -1) {
        if (expectedCol == 7 || expectedCol == 8) {
            digitalWrite(motorPins[motorIndex], HIGH);
            delay(400);
            digitalWrite(motorPins[motorIndex], LOW);
            delay(100);
        }
        else if (expectedCol == 4 || expectedCol == 5) {
            digitalWrite(motorPins[motorIndex], HIGH);
            delay(250);
            digitalWrite(motorPins[motorIndex], LOW);
            delay(200);
            digitalWrite(motorPins[motorIndex], HIGH);
            delay(250);
            digitalWrite(motorPins[motorIndex], LOW);
        } 
        else {
            digitalWrite(motorPins[motorIndex], HIGH);
            delay(300);
            digitalWrite(motorPins[motorIndex], LOW);
            delay(100);
        }
    }
}

// ===== LED2 タイプ点滅制御関数（非ブロック処理） =====
void updateTypeLed() {
    int targetBlinks = currentType + 1; // タイプ0:1回, タイプ1:2回, タイプ2:3回, タイプ3:4回
    unsigned long now = millis();

    if (led2BlinkCount < targetBlinks) {
        // 点滅中処理（ON: 200ms, OFF: 200ms）
        if (!led2State && (now - lastLed2Time >= 200)) {
            led2State = true;
            mcp.digitalWrite(led2, HIGH);
            lastLed2Time = now;
        } 
        else if (led2State && (now - lastLed2Time >= 200)) {
            led2State = false;
            mcp.digitalWrite(led2, LOW);
            lastLed2Time = now;
            led2BlinkCount++;
        }
    } else {
        // 指定回数点滅後のインターバル（1秒待機）
        if (now - lastLed2Time >= 1000) {
            led2BlinkCount = 0; // 点滅カウントをリセットして繰り返す
            lastLed2Time = now;
        }
    }
}

void loop() {

    // ===== LED2 の点滅更新 =====
    updateTypeLed();

    // ===== タイプ切り替え処理 =====
    bool currentSwitchState = mcp.digitalRead(typeSwitchPin);

    if (lastSwitchState == HIGH && currentSwitchState == LOW) {
        currentType++;
        if (currentType >= 4) currentType = 0;

        setType(currentType);

        beep(150);
        delay(200);
    }
    lastSwitchState = currentSwitchState;

    // ===== リセット処理 =====
    if (mcp.digitalRead(resetPin) == LOW) {
        currentMotorPlace = 0;
        programEnd = false;
        lastGuideTime = 0;
        playErrorSound();
        delay(200);
        return;
    }
    
    if (programEnd) {
        mcp.digitalWrite(led2, LOW);
        return;
    }

    int expectedCol = motorOrder[currentMotorPlace];
    int expectedRow = keyRowOrder[currentMotorPlace];

    // ===== 定期的にガイド（音＋振動）を繰り返し実行 =====
    if (millis() - lastGuideTime >= REPEAT_INTERVAL || lastGuideTime == 0) {
        playGuide(expectedRow, expectedCol);
        lastGuideTime = millis();
    }

    // ===== 高速キースキャン =====
    for (int j = 0; j < COLS; j++) {
        mcp.digitalWrite(colPins[j], LOW);

        for (int i = 0; i < ROWS; i++) {
            if (mcp.digitalRead(rowPins[i]) == LOW) {

                if (i == expectedRow && j == expectedCol) {
                    currentMotorPlace++;
                    lastGuideTime = 0; // 次のキーへ進むため即座にガイドを再生させる

                    playCorrectSound();

                    if (currentMotorPlace >= TOTAL_SEQUENCE_LENGTH) {
                        programEnd = true;

                        playCorrectSound();
                        delay(150);
                        playCorrectSound();

                        mcp.digitalWrite(led1, LOW);
                    }

                    mcp.digitalWrite(colPins[j], HIGH);
                    return;
                } else {
                    // ミス音
                    playErrorSound();
                    
                    // キー離脱待ち
                    while (mcp.digitalRead(rowPins[i]) == LOW) {
                        delay(10);
                    }
                }
            }
        }
        mcp.digitalWrite(colPins[j], HIGH);
    }
}