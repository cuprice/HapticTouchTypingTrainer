#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp;

const int ROWS = 3; // 行の数
const int COLS = 10; // 列の数

const int rowPins[ROWS] = {2, 3, 4}; // 行ピン (Arduinoピン)
const int colPins[COLS] = {5, 6, 7, 8, 9, 10, 14, 15, 16, 17}; // 列ピン (Arduinoピン)

// 0番から7番までのモーターピン
int motorPins[8] = {0, 1, 2, 3, 4, 5, 6, 7}; // MCPのピン0〜7

// 各段のモーター順序と、対応するキー入力の段・列を統合した配列
int motorOrder[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9 // colPinsの配列（0から9）に対応
};

int keyRowOrder[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2 // rowPinsの配列（0,1,2）に対応
};

const int TOTAL_SEQUENCE_LENGTH = sizeof(motorOrder) / sizeof(motorOrder[0]); // 全体のシーケンス長

int currentMotorPlace = 0; // 現在のモーターの場所
bool programEnd = false; // プログラム終了の状態

// LEDとブザーはMCP23017のピンに接続
const int led = 9; // MCP23017のピン9
const int buzzerPin = 8; // MCP23017のピン8

void setup() {
    Serial.begin(9600);
    mcp.begin_I2C(0x20);

    // MCPのLEDピン(9)とブザーピン(8)を出力に設定
    mcp.pinMode(buzzerPin, OUTPUT);
    mcp.pinMode(led, OUTPUT);
    mcp.digitalWrite(led, HIGH); // LEDを点灯
    
    // motorPinsに定義された全てのピンを出力に設定
    for (int i = 0; i < 8; i++) {
        mcp.pinMode(motorPins[i], OUTPUT);
    }

    // 行ピン（Arduinoピン）を入力プルアップに設定
    for (int i = 0; i < ROWS; i++) {
        pinMode(rowPins[i], INPUT_PULLUP);
    }

    // 列ピン（Arduinoピン）を出力に設定しHIGHに初期化
    for (int j = 0; j < COLS; j++) {
        pinMode(colPins[j], OUTPUT);
        digitalWrite(colPins[j], HIGH); // 初期状態はHIGH
    }
}

/**
 * アクティブブザーを短時間鳴らす関数。MCPのピンを使用。
 * @param duration 鳴らす時間（ミリ秒）
 */
void beep(int duration) {
    mcp.digitalWrite(buzzerPin, HIGH); // MCPのピンでブザーON
    delay(duration);
    mcp.digitalWrite(buzzerPin, LOW); // MCPのピンでブザーOFF
    delay(50); // 音と音の間隔
}

/**
 * 指定回数ブザーを鳴らす関数
 * @param count 鳴らす回数
 */
void multipleBeeps(int count) {
    for (int k = 0; k < count; k++) {
        beep(100); // 100ms鳴らす
    }
}

/**
 * 押されるべきキー（colPinsのインデックス）に基づいて、
 * 振動させるべきmotorPinsのピン番号（0〜7）を決定します。
 */
int getMotorPinIndex(int expectedCol) {
    // 1から4のとき (colPinsインデックス 0〜3)
    if (expectedCol >= 0 && expectedCol <= 3) {
        return expectedCol; // motorPins[0]〜motorPins[3]
    }
    // 7から10のとき (colPinsインデックス 6〜9)
    else if (expectedCol >= 6 && expectedCol <= 9) {
        // expectedCol 6, 7, 8, 9 を motorPinsのインデックス 4, 5, 6, 7 にマップ
        return expectedCol - 2; // motorPins[4]〜motorPins[7]
    }
    // colPinsが5 (インデックス 4) のとき
    else if (expectedCol == 4) {
        return 3; // motorPin 3 (motorPins[3]に対応)
    }
    // colPinsが6 (インデックス 5) のとき
    else if (expectedCol == 5) {
        return 4; // motorPin 4 (motorPins[4]に対応)
    }
    
    return -1; // 該当なし
}


void loop() {
    if (programEnd) return; // trueなら終了

    int expectedCol = motorOrder[currentMotorPlace]; // 現在期待される列（colPinsのインデックス）
    int expectedRow = keyRowOrder[currentMotorPlace];  // 現在期待される行（rowPinsのインデックス）

    int motorIndex = getMotorPinIndex(expectedCol); // 振動させるべき motorPins のインデックス

    // 【条件】colPinsのインデックスが4か5のときの特殊処理
    if (expectedCol == 4 || expectedCol == 5) {
        // 3. 特殊ブザーを5回鳴らす
        // multipleBeeps(5); 
        // delay(50);

        // 1. 段ごとのブザー通知 (何段目かを示す)
        switch (expectedRow) {
            case 0: multipleBeeps(3); break; // 上段 → 3回
            case 1: multipleBeeps(2); break; // 中段 → 2回
            case 2: multipleBeeps(1); break; // 下段 → 1回
        }
        
        // 2. モーターを振動 (colPins 4 -> motorPins[3], colPins 5 -> motorPins[4])
        if (motorIndex != -1) {
            mcp.digitalWrite(motorPins[motorIndex], HIGH);
            delay(300);
            mcp.digitalWrite(motorPins[motorIndex], LOW);
            delay(300);
            mcp.digitalWrite(motorPins[motorIndex], HIGH);
            delay(300);
        }
        
    } 
    // その他のモーターが割り当てられた列 (インデックス 0-3, 6-9)
    else if (motorIndex != -1) {
        // 段ごとのブザー通知 (音階の代わりに回数で通知)
        switch (expectedRow) {
            case 0: multipleBeeps(3); break; // 上段 → 3回
            case 1: multipleBeeps(2); break; // 中段 → 2回
            case 2: multipleBeeps(1); break; // 下段 → 1回
        }

        // モーターを振動
        mcp.digitalWrite(motorPins[motorIndex], HIGH);
        delay(200);
        mcp.digitalWrite(motorPins[motorIndex], LOW);
        delay(200);
    } 
    // モーターが割り当てられていない列 (この構成では発生しないはず)
    else {
        // 音階の代わりに回数で通知
        switch (expectedRow) {
            case 0: multipleBeeps(3); break;
            case 1: multipleBeeps(2); break;
            case 2: multipleBeeps(1); break;
        }
    }

    // キーマトリックスのスキャン処理
    for (int j = 0; j < COLS; j++) {
        digitalWrite(colPins[j], LOW); // 現在の列をLOWにする

        for (int i = 0; i < ROWS; i++) {
            if (digitalRead(rowPins[i]) == LOW) { // 行ピンがLOWになったらボタンが押されたと判断
                
                if (i == expectedRow && j == expectedCol) {
                    currentMotorPlace++; // 次のモーターに進む
                    
                    // 正解時のブザー: 短く2回
                    beep(200);
                    beep(200);

                    if (currentMotorPlace >= TOTAL_SEQUENCE_LENGTH) {
                        programEnd = true;
                        // 終了音: 少し長めのブザーを3回
                        beep(200);
                        beep(500);
                        beep(200);
                        beep(500);
                        beep(200);
                        beep(500);
                        beep(200);
                        beep(500);
                        mcp.digitalWrite(led, LOW); // 終了時にLEDを消灯
                    }

                    digitalWrite(colPins[j], HIGH); // 列を元に戻す
                    return; // 処理を終了し、次のループへ
                }
            }
        }
        digitalWrite(colPins[j], HIGH); // 列を元に戻す
    }
}