// FastBot2_Muratism — работа через свой хост (Cloudflare Worker / прокси),
// когда api.telegram.org заблокирован.
//
// 1. Создайте Cloudflare Worker с кодом из README (простой reverse-proxy).
// 2. Впишите ниже WIFI_SSID / WIFI_PASS / BOT_TOKEN / CHAT_ID / PROXY_HOST.
// 3. Укажите PROXY_HOST = адрес вашего воркера, например:
//    "shrill-frost-5867.saivankyanhrb.workers.dev"
#include <Arduino.h>

#define WIFI_SSID ""
#define WIFI_PASS ""
#define BOT_TOKEN ""
#define CHAT_ID ""

// адрес вашего Cloudflare Worker / прокси (БЕЗ https:// и слэшей)
#define PROXY_HOST "shrill-frost-5867.saivankyanhrb.workers.dev"

// можно подключить как <FastBot2.h>, так и <FastBot2_Muratism.h>
#include <FastBot2.h>
FastBot2 bot;

void updateh(fb::Update& u) {
    Serial.println("NEW MESSAGE");
    Serial.println(u.message().from().username());
    Serial.println(u.message().text());

    // эхо-ответ
    bot.sendMessage(fb::Message(u.message().text(), u.message().chat().id()));
}

void setup() {
    Serial.begin(115200);
    Serial.println();

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected");

    // ============ FastBot2_Muratism ============
    // главное отличие от стокового FastBot2: перенаправляем бота
    // на свой прокси-хост (Cloudflare Worker), чтобы обойти блокировку
    bot.setHost(PROXY_HOST);   // <- весь HTTP-трафик пойдёт на воркер
    bot.attachUpdate(updateh);
    bot.setToken(F(BOT_TOKEN));
    bot.setPollMode(fb::Poll::Long, 60000);

    bot.sendMessage(fb::Message("Hello from proxy!", CHAT_ID));
}

void loop() {
    bot.tick();
}
