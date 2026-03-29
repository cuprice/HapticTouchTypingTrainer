#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp;

const int ROWS = 3; // 行の数
const int COLS = 8; // 列の数

const int rowPins[ROWS] = {2, 3, 4}; // 行ピン
const int colPins[COLS] = {12, 11, 10, 9, 8, 7, 6, 5}; // 列ピン

// 0番から7番までのモーターピン
int motorPins[8] = {7, 6, 5, 4, 3, 2, 1, 0};

// 各段のモーター順序と、対応するキー入力の段・列を統合した配列
// motorOrderには「どのモーターを振動させるか（=どの列のキーが押されるべきか）」を、
// keyRowOrderには「どの段のキーが押されるべきか」を格納します。
// 例: { motorOrder[0], keyRowOrder[0] } の組み合わせで最初のキー入力を判定
int motorOrder[] = {0, 1, 2, 3, 4, 5, 6, 7,   // 段1のシーケンス
                    0, 1, 2, 3, 4, 5, 6, 7,   // 段2のシーケンス
                    0, 1, 2, 3, 4, 5, 6, 7};  // 段3のシーケンス

int keyRowOrder[] = {1, 1, 1, 1, 1, 1, 1, 1,   // 段2のキーはrow 1
                     0, 0, 0, 0, 0, 0, 0, 0,   // 段1のキーはrow 0
                     2, 2, 2, 2, 2, 2, 2, 2};  // 段3のキーはrow 2

const int TOTAL_SEQUENCE_LENGTH = sizeof(motorOrder) / sizeof(motorOrder[0]); // 全体のシーケンス長

int currentMotorPlace = 0; // 現在のモーターの場所（シーケンス全体での位置）
bool programEnd = false; // プログラム終了の状態

const int led = 14; // 基板のLED
const int buzzerPin = 13; // ブザー
const int noteC = 261;   // ド
const int noteD = 294;   // レ
const int noteE = 329;   // ミ
const int noteG = 392;   // ソ
const int ResetPin = A1; // アナログピンA1をリセットスイッチ

void softwareReset() {
  asm volatile ("jmp 0"); // プログラム最初に
}

void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  pinMode(led, OUTPUT);
  mcp.begin_I2C(0x20);

  pinMode(ResetPin, INPUT_PULLUP); 

  digitalWrite(led, HIGH);
  
  for (int i = 0; i <= 7; i++) {
    mcp.pinMode(i, OUTPUT);
  }

  for (int i = 0; i < ROWS; i++) {
    pinMode(rowPins[i], INPUT_PULLUP);
  }

  for (int j = 0; j < COLS; j++) {
    pinMode(colPins[j], OUTPUT);
    digitalWrite(colPins[j], HIGH); // 初期状態はHIGH
  }
}

void playTone(int note) {
  tone(buzzerPin, note);    // 指定した音階でブザーを鳴らす
  delay(200);             // 200ミリ秒待つ
  noTone(buzzerPin);      // 音を止める
}

void loop() {
  // リセットスイッチが押されたらソフトウェアリセット
  if (digitalRead(ResetPin) == LOW) {
    delay(50);
    if (digitalRead(ResetPin) == LOW) {
      softwareReset();
    }
  }

  if (programEnd) return; // trueなら終了

  int expectedMotor = motorOrder[currentMotorPlace]; // 現在期待される列（モーター）
  int expectedRow = keyRowOrder[currentMotorPlace];   // 現在期待される行

  // 段ごとの音階通知
  switch (expectedRow) {
    case 0:
      playTone(noteG); // 上段 → ソ
      break;
    case 1:
      playTone(noteE); // 中段 → ミ
      break;
    case 2:
      playTone(noteC); // 下段 → ド
      break;
  }


  // モーターを振動
  mcp.digitalWrite(motorPins[expectedMotor], HIGH);
  delay(200);
  mcp.digitalWrite(motorPins[expectedMotor], LOW);
  delay(200);

  // キーマトリックスのスキャン処理
  for (int j = 0; j < COLS; j++) {
    digitalWrite(colPins[j], LOW); // 現在の列をLOWにする

    for (int i = 0; i < ROWS; i++) {
      if (digitalRead(rowPins[i]) == LOW) { // 行ピンがLOWになったらボタンが押されたと判断
        if (i == expectedRow && j == expectedMotor) {
          currentMotorPlace++; // 次のモーターに進む

          if (currentMotorPlace >= TOTAL_SEQUENCE_LENGTH) {
            programEnd = true;
            playTone(noteC);
            playTone(noteD);
            playTone(noteE);
            playTone(noteD);
            playTone(noteC);
          }

          digitalWrite(colPins[j], HIGH);
          return;
        }
      }
    }
    digitalWrite(colPins[j], HIGH);
  }
}
