#include "core_class.h"
#include "packet.h"

namespace fb {

Core* thisBot = nullptr;

// текущий хост Telegram API (по умолчанию — api.telegram.org)
String Packet::hostName = TELEGRAM_HOST;

}
