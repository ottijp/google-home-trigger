#include <time.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>

// WiFi設定
const char* ssid = "<your Wi-Fi ssid>";
const char* password = "<your Wi-Fi password>";

// MQTT設定
const char* host = "<your AWS IoT endpoint";
const int port = 8883;
const char* mqttClientId = "google-home-01";

// CA証明書
extern const unsigned char caCert[] PROGMEM;
extern const unsigned int caCertLen;

// クライアント証明書
extern const unsigned char clientCert[] PROGMEM;
extern const unsigned int clientCertLen;

// クライアント秘密鍵
extern const unsigned char clientKey[] PROGMEM;
extern const unsigned int clientKeyLen;

// SSLクライアント
WiFiClientSecure sslClient;

// MQTTクライント
MQTTClient mqttClient;

// GPIO
#define ATP3011_RESET_PIN 14
#define NETWORK_INDICATOR_PIN 5

void setup() {
  // GPIO
  pinMode(ATP3011_RESET_PIN, OUTPUT);
  pinMode(NETWORK_INDICATOR_PIN, OUTPUT);
  digitalWrite(ATP3011_RESET_PIN, LOW);
  digitalWrite(NETWORK_INDICATOR_PIN, LOW);

  // シリアル通信
  Serial.begin(9600);  // デバッグ用
  Serial1.begin(9600); // ATP3011へのコマンド送出用

  // ATP3011の起動
  digitalWrite(ATP3011_RESET_PIN, HIGH);
  delay(80);

  // Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // DERフォーマットでSSLに必要な情報を読み込み
  // CA証明書
  bool res = sslClient.setCACert_P(caCert, caCertLen);
  if (!res) {
    Serial.println("Failed to load CA certificate!");
    while (true) {
      yield();
    }
  }
  // クライアント証明書
  res = sslClient.setCertificate_P(clientCert, clientCertLen);
  if (!res) {
    Serial.println("Failed to load client certificate!");
    while (true) {
      yield();
    }
  }
  // クライアント秘密鍵
  res = sslClient.setPrivateKey_P(clientKey, clientKeyLen);
  if (!res) {
    Serial.println("Failed to load client key!");
    while (true) {
      yield();
    }
  }

  // MQTT
  mqttClient.begin(host, port, sslClient);
  mqttClient.onMessage(messageReceived);

  // ATP3011の初期化（ボーレートの自動設定）
  Serial1.println("?");

  connect();
}

void connect() {
  digitalWrite(NETWORK_INDICATOR_PIN, LOW);

  // Wi-Fiへ接続
  Serial.println();
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // SNTPで時刻同期
  // サーバから要求されるTLS認証が妥当か検証するために必要
  Serial.print("Setting time using SNTP");
  configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println();
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));

  // MQTTブローカへ接続
  Serial.print("connecting to ");
  Serial.println(host);
  while (!mqttClient.connect(mqttClientId)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("MQTT connected");

  // サーバ証明書を検証
  if (sslClient.verifyCertChain(host)) {
    Serial.println("Server certificate verified");
    digitalWrite(NETWORK_INDICATOR_PIN, HIGH);
  } else {
    Serial.println("ERROR: certificate verification failed!");
    digitalWrite(NETWORK_INDICATOR_PIN, LOW);
    return;
  }

  mqttClient.subscribe(mqttClientId);
  Serial.println("MQTT subscribing...");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("MQTTメッセージ受信: " + topic + " - " + payload);
  if (payload != "") {
    Serial.println("ATP3011へコマンド送信");
    Serial1.println(payload);
  }
}

void loop() {
  mqttClient.loop();

  if (!mqttClient.connected()) {
    connect();
  }
}

