## タイピング練習用キーボード基板 Haptic Touch Typing Trainer (Prototype)

![基板写真](images/v5smd-top.jpg)
![基板写真](images/short-gif.gif)

# これは何？
What is this?

A haptic and audio-guided touch typing trainer that tells you which key to press next through vibration and sound — no need to look at your hands.

振動と音で次に押すべきキーを知らせる、触覚と聴覚を使ったタッチタイピング練習機です。

# はじめに

このリポジトリは回路・プログラミング初学者の学習用です。あたたかく見守っていただけると嬉しいです。

これは、プログラミングや回路の初心者である私が、自分なりに学びながら作ってみた実装基板です。  
8つのキーに振動する部品を付け、目で確認しなくてもどこを押せば良いかがわかる機能を目指しました。
~~公開しようと思ったきっかけは、似たようなアイデアのタイピング練習機を見つけることができなかったためです。~~

触覚を利用した複雑なタッチタイピングの練習機械が他にもあることを知り、私はまだまだ勉強不足であると思いました。もしお嫌でなければ、学習の活動を見守っていただければ幸いです。

このリポジトリでは、タイピング練習を目的としたキーボード基板の設計データを公開しています。  
基板はKiCad 9.0で設計されています。振動するキー順番を変更するには、Arduinoのスケッチを編集する必要があります。

ただし、現在はアイデア段階であり、完全な完成品ではありません。今後の改善や追加機能に関しては更新を行う予定です。

This repository is for beginners who are learning circuits and programming.
I’d be happy if you could kindly support it.

This is a keyboard I made while learning programming and circuits by myself.
I am still a beginner, but I tried my best to build it.

I added small parts that can vibrate to 8 keys.
This way, you can feel which key to press without looking.

~~I decided to share this because I couldn't find a similar typing practice device online.~~

I learned that there are other machines that use touch to practice touch typing in a complex way, and I felt that I still have a lot to learn.
If you don’t mind, I would be happy if you could watch over my learning.

In this repository, I show the design data for this keyboard.
I made the design using KiCad 9.0.
If you want to change the order of the vibrating keys, you need to change the Arduino program.

Please note that this is still just an idea, not a finished product.
I want to improve it and add more features in the future.

---

## 特徴

- 10列、3段のキー(計30個)による練習用構成
- リセットスイッチ付き
- Cherry MX対応
- Arduino Nano Everyで完璧ではありませんが、一応動作確認済みです (後継機のモデルは動作未確認)
- KiCad 9.0で基板設計
- 基板サイズ：約261.1mm × 81.5mm(捨て基板除く)
- 電源：Arduino Nano Every経由でのUSB給電

This is a practice keyboard layout with 30 keys, arranged in 10 columns and 3 rows.
It includes a reset switch. It's compatible with Cherry MX switches. I've confirmed it works with the Arduino nano every, although it's not perfect. I haven't tested it with newer Arduino models. The circuit board was designed using KiCad 9.0. The board size is about 261.1mm × 81.5mm, and it's powered by USB through the Arduino nano every.

どの段のキーを押せば良いかを、3段階のブザー音で教えるためのコードを加筆修正しました。上の段ならば3回鳴り、下の段ならば１回鳴り、真ん中の段なら２回鳴るという仕組みにしました。振動モーターがないキーの場合は、振動する長さを変えて位置を知らせるようにしました。

I changed the code to use three kinds of buzzer sounds to tell the user which row of keys to press.
If the key is on the top row, the buzzer beeps three times.
If the key is on the bottom row, it beeps one time.
If the key is on the middle row, it beeps two times.
For keys that do not have a vibration motor, the device changes the length of the vibration to show the position.

---

## ファイル構成

~~~

TypingTrainer/
    ├── TYPING-TRAINER-V5-SMD/ # KiCad設計ファイル
    ├── TYPING-TRAINER-V5-SMD.zip # ガーバーデータ（基板製造用）
    ├── TrainerV5-smd/ # Arduinoスケッチ 通常版
    ├── TrainerV5-smd2/ # Arduinoスケッチ 切替式例文プリセットタイプ
    ├── numorder.py # キーの順番を決めるPythonスクリプト
    ├── keycap.stl # 3Dプリンタ用、穴有りキャップ
    ├── images/ # 写真・完成図
    ├── docs/ # 部品表・座標データなど
    ├── OldVersion-Uno # 旧バージョン、Uno用
    ├── OldVersion-Nano # 旧バージョン、Nano用
    └── README.md # このファイル

~~~

---

## 完成イメージ、回路図、動作の様子

![基板写真](images/v5smd-top.jpg)

![基板写真](images/v5smd-bottom.jpg)

![基板写真](images/schematic.jpg)



[動作の様子](images/working.mp4)

[モーターの動き](images/shaker.mp4)

---

## 使い方
1. ガーバーファイルをJLCPCBなどで製造依頼
2. 回路図などを見ながら部品を実装
3. Arduino Nano EveryをUSBケーブルでPCと接続
4. PCからArduinoへ、ファームウェアを書き込み

---

## 必要部品

| Value               | Reference      | Qty | 備考                                           | 画像 |
|---------------------|----------------|-----|------------------------------------------------|------|
| Arduino Nano Every  | A1             | 1   | マイクロコントローラー（ソケットピン経由で実装） |  |
| Passive Buzzer      | BZ1            | 1   | パッシブブザー                                         | <img src="images/buzzer.jpg" width="60"> |
| 1N4148              | D1 〜 D30      | 30  | スイッチのマトリックス用ダイオード             | <img src="images/diode.jpg" width="60"> |
| LED                 | D100, D101     | 2   | インジケーターLED                              | <img src="images/led.jpg" width="60"> |
| Motor DC            | M1 〜 M8       | 8   | DCモーター                                     | <img src="images/motor.jpg" width="60"> |
| Resist 10k                 | R1 〜 R12      | 12  | 抵抗（10kΩ）                                   | <img src="images/resist10k.jpg" width="60"> |
| Resist 330                 | R13, R14       | 2   | 抵抗（330Ω, LED電流制限用）                    | <img src="images/resist330.jpg" width="60"> |
| Cherry MX Switch    | SW1 〜 SW30    | 30  | Cherry MX互換キースイッチ(キーキャップはお好みで)                      | <img src="images/cherrysw.jpg" width="60"> |
| 2pin Button         | SW100, SW101   | 2   | 2ピンのタクトスイッチなど                      | <img src="images/button.jpg" width="60"> |
| MCP23017_SP         | U2             | 1   | I/Oエキスパンダー（ICソケットの上に実装）      | <img src="images/ic23017.jpg" width="60"> |
| TerminalBlock         | M1 〜 M8             | 8   | ターミナルブロック(裏面実装)      | <img src="images/terminalblock.jpg" width="60"> |
| SW socket         | SW1 〜 SW30             | 30   | キースイッチソケット(裏面実装)      | <img src="images/swsocket.jpg" width="60"> |



---

## KiCad情報

- KiCad バージョン: 9.0
- 古いバージョンでは一部ファイルが開けない可能性があります。
- 最新バージョンでの動作は未確認です。


使用したライブラリURL
- ブザーフットプリント
 https://jp.sameskydevices.com/product/resource/pcbfootprint/cmt-9648-85t
- キースイッチフットプリント
 https://github.com/daprice/keyswitches.pretty?tab=readme-ov-file

---

## ライセンス

この基板デザインは MIT ライセンスで公開しています。  

---

## よくある質問

準備中
**Q. ？**  
A. 

---

## 注意事項

- 初心者が学びながら作ったものなので、不完全な点もあるかもしれません。
- Arduinoスケッチの一部のコード作成に、生成AI（ChatGPT）を補助的に活用しました。

---

## Special Thanks

- ネット上のチュートリアルや電子工作記事の皆さま

---

# 今後の予定
- キーを押す順番の配列を簡単に作るツール
- 製作手順のガイドを作る
