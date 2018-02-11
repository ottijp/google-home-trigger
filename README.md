# google-home-trigger

Google Homeのスキルを自動で起動するためのトリガーデバイスです．

## ビルド

### ライブラリ

以下２つのライブラリをインストールしてください．

* [ESP8266WiFi](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi)
* [MQTT](https://github.com/256dpi/arduino-mqtt)

### `google-home-trigger.ino`

* `ssid`と`password`を接続したいWi-Fiの情報に変更してください
* `host`をAWS IoTのエンドポイントに変更してください

### `CACert.ino`

* AWS IoTのCA認証ファイルをDER形式でchar[]リテラルとして記述してください
* PEMからDERへの変換コマンド例

```
openssl x509 -inform PEM -in VeriSign-Class%203-Public-Primary-Certification-Authority-G5.pem -outform der -out VeriSign-Class%203-Public-Primary-Certification-Authority-G5.der
```

### `ClientCert.ino`

* AWS IoTで作成したクライアント認証ファイルをDER形式でchar[]リテラルとして記述してください
* PEMからDERへの変換コマンド例

```
openssl x509 -inform PEM -in 06bb2b8e40-certificate.pem.crt -outform der -out 06bb2b8e40-certificate.der.crt
```

### `ClientKey.ino`

* AWS IoTで作成したクライアント秘密鍵ファイルをDER形式でchar[]リテラルとして記述してください
* PEMからDERへの変換コマンド例

```
openssl rsa -inform PEM -in 06bb2b8e40-private.pem.key -outform der -out 06bb2b8e40-private.der.key
```

### ESP8266への書き込み

書き込み時はIO0（回路図中の黄色いワイヤ）をLOWにしてリセットボタン（S1）を押してから書き込みます．

## パーツファイル

ESP-WROOM-02のFritzing用パーツファイルとして，以下のものを使わせてもらっています．

[ESP-WROOM-02-AE (with Akizuki-Electronics breakout board) - parts submit - fritzing forum](http://forum.fritzing.org/t/esp-wroom-02-ae-with-akizuki-electronics-breakout-board/1753)

## ToDo

* Fritzingの回路でATP3011F4-PUのVCCとGNDが結線されているようにRatsnestが出てしまう問題が解決できない
