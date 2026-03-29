2025.11.12 今後の予定 Future Plan
基板にArduino Nano Everyを乗せる。AからZまでのキーを押せるようにする

I’m going to put an Arduino Nano Every on the board and make it work with keys from A to Z.

![基板写真](images/trainer-v5.jpg)

2025.8.2 更新
どの段のキーを押せば良いかを、3段階のブザー音で教えるためのコードを加筆修正しました。上の段ならば高めの音、下の段ならば低めの音、真ん中の段ならその中間の音という仕組みにしました。

I changed the code to help users know which row of keys to press by using three different buzzer sounds.
A high sound means the top row, a low sound means the bottom row, and a middle sound means the middle row.

2025.7.27 追記
Arduinoのスケッチを、AIを使用して加筆修正し、複数の段にまたがって順番を指定できるように変更しました。また、段を切り替えるボタンは無効となっています。フォルダ名は、TrainerV4-i

I edited the Arduino sketch using AI. Now, you can set the order across several steps. The button to change steps is disabled. The folder name is TrainerV4-i.

## タイピング練習用キーボード基板
このリポジトリは回路・プログラミング初学者の学習用です。あたたかく見守っていただけると嬉しいです。

これは、プログラミングや回路の初心者である私が、自分なりに学びながら作ってみた実装基板です。  
8つのキーに振動する部品を付け、目で確認しなくてもどこを押せば良いかがわかる機能を目指しました。
公開しようと思ったきっかけは、似たようなアイデアのタイピング練習機を見つけることができなかったためです。

このリポジトリでは、タイピング練習を目的としたキーボード基板の設計データを公開しています。  
基板はKiCad 8.0で設計されています、基板上に部品を実装し、Arduino Uno R3に接続すれば使えるようになっています。
振動するキー順番を変更するには、Arduinoのスケッチを編集する必要があります。

ただし、現在はアイデア段階であり、完全な完成品ではありません。今後の改善や追加機能に関しては更新を行う予定です。

This repository is for beginners who are learning circuits and programming.
I’d be happy if you could kindly support it.

This is a keyboard I made while learning programming and circuits by myself.
I am still a beginner, but I tried my best to build it.

I added small parts that can vibrate to 8 keys.
This way, you can feel which key to press without looking.

I decided to share this because I couldn't find a similar typing practice device online.

In this repository, I show the design data for this keyboard.
I made the design using KiCad 8.0.
If you put the parts on the board and connect it to an Arduino Uno R3, it will work.
If you want to change the order of the vibrating keys, you need to change the Arduino program.

Please note that this is still just an idea, not a finished product.
I want to improve it and add more features in the future.

---

## 特徴

- 8列、3段のキー(計24個)による練習用構成
- リセットスイッチ、段の変更スイッチ機能付き
- Cherry MX対応
- Arduino Uno R3で完璧ではありませんが、一応動作確認済みです (後継機のモデルは動作未確認)
- KiCad 8.0で基板設計
- 基板サイズ：約215.9mm × 104.1mm
- 電源：Arduino Uno R3経由でのUSB給電

This is a practice keyboard layout with 24 keys, arranged in 8 columns and 3 rows.
It includes a reset switch and a switch to change the rows. It's compatible with Cherry MX switches. I've confirmed it works with the Arduino Uno R3, although it's not perfect. I haven't tested it with newer Arduino models. The circuit board was designed using KiCad 8.0. The board size is about 215.9mm x 104.1mm, and it's powered by USB through the Arduino Uno R3.

---

## ファイル構成

~~~

TypingTrainer/
    ├── TYPING-TRAINER-V4/ # KiCad設計ファイル
    ├── TYPING-TRAINER-V4.zip # ガーバーデータ（基板製造用）
    ├── TrainerV4_h-custom/ # Arduinoスケッチ
    ├── TrainerV4-i/ # Arduinoスケッチ (複数の段用スケッチ)
    ├── TrainerV4_i_example_words/ # Arduinoスケッチ (例文を使ったスケッチ)
    ├── images/ # 写真・完成図
    ├── docs/ # 部品表、ピン配置
    └── README.md # このファイル

~~~

---

## 完成イメージ

![基板写真](images/pcbimage.jpg)

---

## 使い方
1. ガーバーファイルをPCBwayなどで製造依頼
2. 回路図などを見ながら部品を実装
3. Arduino Uno R3と基板をジャンパワイヤで接続後、ArduinoをUSBケーブルでPCと接続
4. PCからArduinoへ、ファームウェアを書き込み

---

## 必要部品

| パーツ                   | 型番 / 種類                  | 数量 | 備考         |
| :----------------------- | :--------------------------- | :--- | :----------- |
| マイコン                 | Arduino Uno R3               | 1個  |              |
| USBケーブル              | USB-A to USB-B               | 1本  | PC接続用     |
| I/Oエキスパンダー        | MCP23017                     | 1個  |              |
| ICソケット               | 28ピン スリム300milタイプ    | 1個  | IC直付けなら不要 |
| メカニカルスイッチ       | Cherry MX互換  5ピン         | 24個 |              |
| キーキャップ             | 上記スイッチ対応のもの        | 24個 |              |
| ダイオード               | 1N4148                       | 3個  | キーマトリクス用 |
| 抵抗                     | 10kオーム                    | 10個 |              |
| 抵抗                     | 330オーム                    | 1個  |              |
| LED (5mm)                | 色はお好みで                 | 1個  |              |
| タクトスイッチ           |                              | 2個  |              |
| パッシブ電子ブザー       |                              | 1個  | \*1          |
| 振動モーター             |                              | 8個  |              |
| コネクタ用のピンソケット | 20pin                        | 1個  |              |
| コネクタ用のピンソケット | 2pin                         | 1個  |              |
| ジャンパ線              | 10cm～20cm                    | 20本くらいあれば     |                   

*1 ブザーのフットプリント参考URL https://jp.sameskydevices.com/product/resource/pcbfootprint/cmt-9648-85t

---

## KiCad情報

- KiCad バージョン: 8.0
- 古いバージョンでは一部ファイルが開けない可能性があります。
- 最新バージョンでの動作は未確認です。

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


