#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// ===== CẤU HÌNH WiFi =====
#define WIFI_SSID     "TP-Link_E1D6"
#define WIFI_PASSWORD "10110041"

// ===== CẤU HÌNH TELEGRAM =====
#define BOT_TOKEN  "8810074284:AAGLffBa8r51KsvJwNOXiNvW9sWrbhL9P7A"
#define CHAT_ID    "5487490987"

// ===== CẤU HÌNH UART (nhận từ STM32) =====
#define RXD2 16
#define TXD2 17

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  // Kết nối WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Đang kết nối WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi đã kết nối!");

  client.setInsecure(); // Bỏ qua xác thực SSL
  bot.sendMessage(CHAT_ID, "✅ Hệ thống FireAlarm đã khởi động!", "");
}

void loop() {
  if (Serial2.available()) {
    int msg = Serial2.read();

    if (msg == 1) { // FIRE_ALARM
      bot.sendMessage(CHAT_ID, "🔥 CẢNH BÁO CHÁY! Phát hiện lửa!", "");
    }
    else if (msg == 2) { // FIRE_SAFE
      bot.sendMessage(CHAT_ID, "✅ An toàn! Không còn phát hiện lửa.", "");
    }
    else if (msg == 3) { // FIRE_LOW_BATTERY
      bot.sendMessage(CHAT_ID, "🔋 CẢNH BÁO: Pin yếu dưới 10%!", "");
    }
  }
}