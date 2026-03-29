#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp;

const int ROWS = 3; // 行の数
const int COLS = 8; // 列の数

const int rowPins[ROWS] = {2, 3, 4}; // 行ピン
const int colPins[COLS] = {12, 11, 10, 9, 8, 7, 6, 5}; // 列ピン

// 0番から7番までのLEDピン
int motorPins[8] = {7, 6, 5, 4, 3, 2, 1, 0};

// 点滅させるLEDの順序を指定
int motorOrder1[8] = {0, 1, 2, 3, 4, 5, 6, 7};
int motorOrder2[8] = {0, 1, 2, 3, 4, 5, 6, 7};
int motorOrder3[8] = {0, 1, 2, 3, 4, 5, 6, 7};
//int motorOrder1[8] = {0, 1, 2, 3, 4, 5, 6, 7};
//int motorOrder2[8] = {7, 6, 5, 4, 3, 2, 1, 0};
//int motorOrder3[8] = {0, 2, 3, 1, 4, 6, 7, 5};
//int motorOrder[22] = {3, 5, 0, 4, 5, 0, 4, 0, 6, 0, 1, 0, 5, 0, 5, 0, 1, 0, 2, 0, 5, 0};

// LEDシーケンスの配列
int* motorOrders[] = {motorOrder1, motorOrder2, motorOrder3};
// 各シーケンスの長さ
int motorOrderLen[] = {8, 8, 8};

int currentMotorPlace = 0; // 現在のLEDの場所
bool programEnd = false; // プログラム終了の状態

const int led = 14; // 基板のLED
const int buzzerPin = 13; // ブザー
const int noteC = 261;  // ド
const int noteD = 294;  // レ
const int noteE = 329;  // ミ
const int ResetPin = A1; // アナログピンA1をリセットスイッチ
const int changeSwitch = A2; // 切り替えスイッチ
int currentRowSelect = 0; // 現在選択されている列
unsigned long lastSwitchTime = 0; // チャタリング防止
const unsigned long Delay = 200; // ディレイ時間

void softwareReset() {
  asm volatile ("jmp 0"); // プログラム最初に
}

void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(changeSwitch, INPUT_PULLUP);
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
  tone(buzzerPin, note);  // 指定した音階でブザーを鳴らす
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

  // タクトスイッチ状態確認
  if (digitalRead(changeSwitch) == LOW) {
    // チャタリング防止
    if (millis() - lastSwitchTime > Delay) {
      currentRowSelect = (currentRowSelect + 1) % ROWS; // rowを0, 1, 2と切り替える
      currentMotorPlace = 0; // 新しい要素の最初から開始
      playTone(noteC); // 音を鳴らす
      lastSwitchTime = millis();
    }
  }

  int* currentStatus = motorOrders[currentRowSelect];
  int statusLen = motorOrderLen[currentRowSelect];
  int currentMotor = currentStatus[currentMotorPlace]; // 現在のモーター

  // モーターを振動
  mcp.digitalWrite(motorPins[currentMotor], HIGH);
  delay(200);
  mcp.digitalWrite(motorPins[currentMotor], LOW);
  delay(200);

  // キーマトリックスのスキャン処理
  for (int j = 0; j < COLS; j++) {
    digitalWrite(colPins[j], LOW); // 現在の列をLOWにする

    for (int i = 0; i < ROWS; i++) {
      if (digitalRead(rowPins[i]) == LOW) { // 行ピンがLOWになったらボタンが押されたと判断
        // 現在選択されているrowと押された行、現在のLEDと押された列が一致する場合
        if (i == currentRowSelect && j == currentMotor) {

          currentMotorPlace++; // 次のモーターに進む

          // 押すキーの数を増やすときに下記の数字を変更すること
          if (currentMotorPlace >= 8) {
            programEnd = true;
            playTone(noteC);
            playTone(noteD);
            playTone(noteE);
            playTone(noteD);
            playTone(noteC);
          }

          // キーが押されたらループを抜ける
          break;
        }
      }
    }
    digitalWrite(colPins[j], HIGH); // 現在の列をHIGHに戻す
  }
}