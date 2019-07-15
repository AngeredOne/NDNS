#include "SettingsFields.h"

void LastIP::ApplyValue(std::string value) {
    ip = value;
}

std::string LastIP::GetValue() const {
    return ip;
}


void LastNickname::ApplyValue(std::string value) {
    user = value;
}

std::string LastNickname::GetValue() const {
    return user;
}
