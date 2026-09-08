#pragma once
#include <Arduino.h>
#include <GyverHTTP.h>

#include "../bot_config.h"
#include "packet.h"

namespace fb {

class Http {
   public:
    Http(Client& client) : _hostName(TELEGRAM_HOST), http(client, TELEGRAM_HOST, TELEGRAM_PORT) {}

    // установить адрес (хост) сервера Telegram API
    void setHost(const String& host, uint16_t port = TELEGRAM_PORT) {
        _hostName = host;
        _applyHost();
        _port = port;
    }

    // установить адрес (хост) сервера Telegram API
    void setHost(const char* host, uint16_t port = TELEGRAM_PORT) {
        _hostName = host;
        _applyHost();
        _port = port;
    }

    // установить адрес (хост) сервера Telegram API
    void setHost(const IPAddress& ip, uint16_t port = TELEGRAM_PORT) {
        _hostName = ip.toString();
        http.setHost(ip, port);
        _ip = ip;
        _port = port;
        Packet::hostName = _hostName;
    }

    // получить текущий хост
    const String& getHost() {
        return _hostName;
    }

    // вернуть хост Telegram API по умолчанию
    void clearHost() {
        _hostName = TELEGRAM_HOST;
        _applyHost();
        _port = TELEGRAM_PORT;
    }

    // установить proxy (хост:порт для TCP/TLS-коннекта)
    void setProxy(const char* host, uint16_t port) {
        http.setHost(host, port);
        _ip.fromString(host);
        _port = port;
    }

    // установить proxy
    void setProxy(const IPAddress& ip, uint16_t port) {
        http.setHost(ip, port);
        _ip = ip;
        _port = port;
    }

    // получить proxy IP
    const IPAddress& getProxyIP() {
        return _ip;
    }

    // Получить proxy port
    uint16_t getProxyPort() {
        return _port;
    }

    // удалить proxy
    void clearProxy() {
        http.setHost(TELEGRAM_HOST, TELEGRAM_PORT);
    }

   protected:
    ghttp::Client http;

   private:
    String    _hostName;  // хост, который пишется в HTTP-запросах (Packet)
    IPAddress _ip;
    uint16_t  _port;

    // применить хост к коннекту и HTTP-пакетам
    void _applyHost() {
        http.setHost(_hostName.c_str(), TELEGRAM_PORT);
        _ip.fromString(_hostName);
        Packet::hostName = _hostName;
    }
};

}  // namespace fb
