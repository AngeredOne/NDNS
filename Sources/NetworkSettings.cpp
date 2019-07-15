#include "SettingsFields.h"

void LastIP::ApplyValue(std::string value) {
    ip = value;
}

std::string LastIP::GetValue() const {
    return ip;
}