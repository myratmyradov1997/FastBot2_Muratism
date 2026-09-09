# FastBot2_Muratism

Форк библиотеки [FastBot2](https://github.com/GyverLibs/FastBot2) (AlexGyver) — Telegram-бот для Arduino / ESP8266 / ESP32.

**Главное отличие от оригинала:** добавлен метод `setHost()`, позволяющий указать **свой адрес Telegram API** (например, Cloudflare Worker или свой прокси-домен). Это нужно, когда `api.telegram.org` заблокирован (например, в РФ) — бот продолжает работать через обходной хост, оставаясь зашифрованным (HTTPS).

Оригинальный README и документация GyverLibs — в [README_FASTBOT2_ORIGINAL.md](README_FASTBOT2_ORIGINAL.md) и папке [docs/](docs/).

---

## Быстрый старт

```cpp
#include <Arduino.h>

#define WIFI_SSID "...."
#define WIFI_PASS "...."
#define BOT_TOKEN "123456:ABC-DEF..."
#define CHAT_ID   "...."

// адрес вашего Cloudflare Worker / прокси (без https:// и слэшей)
#define PROXY_HOST "shrill-frost-5867.saivankyanhrb.workers.dev"

#include <FastBot2.h>   // или <FastBot2_Muratism.h>
FastBot2 bot;

void setup() {
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) delay(500);

    bot.setHost(PROXY_HOST);   // ← главная фича форка
    bot.setToken(F(BOT_TOKEN));
    bot.attachUpdate([](fb::Update& u) {
        bot.sendMessage(fb::Message(u.message().text(), u.message().chat().id()));
    });
    bot.setPollMode(fb::Poll::Long, 60000);
}

void loop() {
    bot.tick();
}
```

Готовый пример: `examples/withProxy/withProxy.ino`

## API (что добавлено к FastBot2)

| Метод | Описание |
|---|---|
| `bot.setHost("my-worker.workers.dev")` | Перенаправить весь трафик бота на свой хост |
| `bot.clearHost()` | Вернуть стандартный `api.telegram.org` |
| `bot.getHost()` | Узнать текущий хост |

> `setHost()` принимает `const char*`, `String` или `IPAddress`. По умолчанию хост — `api.telegram.org` (443).

## Как сделать Cloudflare Worker (бесплатно, ~3 минуты)

В [Cloudflare](https://dash.cloudflare.com/) → **Workers & Pages → Create → Worker** вставьте код:

```js
export default {
  async fetch(request) {
    const url = new URL(request.url);
    url.hostname = 'api.telegram.org';
    url.port = '443';
    const modifiedRequest = new Request(url.toString(), {
      method: request.method,
      headers: request.headers,
      body: request.body,
      redirect: 'follow'
    });
    return fetch(modifiedRequest);
  }
};
```

Нажмите **Deploy** — получите адрес вида `my-tg-proxy.<subdomain>.workers.dev`. Его и передавайте в `bot.setHost(...)`.

**Важные нюансы:**

- В `setHost()` указывайте **только домен**, без `https://` и слэшей.
- У Cloudflare бесплатный тариф ~100 000 запросов/сутки — класс из 20–30 ESP32 укладывается с запасом.
- Домен `*.workers.dev` в РФ иногда придушивают. Для 100% стабильности привяжите к воркеру свой домен (Cloudflare → Worker → Settings → Custom Domains), например `tg.myschool.ru` (домен ~150–200 ₽/год), и указывайте уже его.
- Проверка в браузере: `https://ВАШ_ВОРКЕР/bot<ТОКЕН>/getMe` должен вернуть `{"ok":true,...}`.

## Как работает `setHost()` (технически)

В оригинальном FastBot2 хост `api.telegram.org` «зашит» в двух местах:
1. в HTTP-запросе (request-line `POST https://api.telegram.org/bot…` и заголовок `Host:`);
2. в адресе TCP/TLS-соединения.

В форке хост вынесен в рантайм-переменную `fb::Packet::hostName`. `setHost()` меняет и адрес соединения, и хост внутри HTTP-запроса, поэтому запросы идут **в origin-form** (`POST /bot<token>/sendMessage` + `Host: ваш-воркер`) — именно такую форму принимает Cloudflare Worker.

---

## Установка

### Способ 1. ZIP (быстрее всего, для учеников)

1. Скачайте архив: https://github.com/myratmyradov1997/FastBot2_Muratism/archive/refs/heads/main.zip
2. Arduino IDE: **Скетч → Подключить библиотеку → Добавить .ZIP библиотеку…** → выберите скачанный архив.
3. Готово — `FastBot2_Muratism` появится в списке библиотек.

### Способ 2. PlatformIO

В `platformio.ini`:

```ini
lib_deps =
    https://github.com/myratmyradov1997/FastBot2_Muratism.git
```

### Способ 3. Вручную

Распакуйте архив в папку библиотек Arduino: `~/Documents/Arduino/libraries/FastBot2_Muratism/`

> Для работы нужны зависимости оригинала (ставятся автоматически при установке через ZIP-менеджер Arduino IDE / PlatformIO):
> [GSON](https://github.com/GyverLibs/GSON), [GyverHTTP](https://github.com/GyverLibs/GyverHTTP), [StringUtils](https://github.com/GyverLibs/StringUtils), [GTL](https://github.com/GyverLibs/GTL).

### Почему не «по названию» в менеджере библиотек Arduino?

Чтобы библиотека ставилась по названию из **Library Manager** (Arduino IDE), её нужно опубликовать в официальном реестре [arduino-libraries/library-registry](https://github.com/arduino/libraries) — это отдельный публичный pull-request с ревью. Форк можно опубликовать позже (имя `FastBot2_Muratism` в реестре свободно), но для занятий достаточно ZIP/PlatformIO-способов выше.

---

## Лицензия и атрибуция

- Код форка — **MIT**, © 2023 GyverLibs (оригинальный [LICENSE](LICENSE) сохранён).
- Форк: Muratism, 2026.
- Оригинал: [GyverLibs/FastBot2](https://github.com/GyverLibs/FastBot2) © AlexGyver.

## Обратная связь

Для своих занятий/школьников: вопросы — в Issues этого репозитория.
