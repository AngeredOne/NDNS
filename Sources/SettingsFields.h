#pragma once
#include "NDNS.h"
#include "Settings.h"

typedef std::pair<std::string, std::shared_ptr<SettingsField>> FieldPair;

class SettingsField
{
public:
    virtual void ApplyValue(std::string value) = 0;
    virtual std::string GetValue() const = 0;
};

class VolumeControl : public SettingsField
{
public:
    virtual void ApplyValue(std::string value);
    virtual std::string GetValue() const;

    float GetVolume() const;

protected:
    const float MAX_VOLUME = 4.f;
    const float MIN_VOLUME = 0.01f;
    float volume = 1.f;
};


class ThresholdControl : public SettingsField
{
public:

    virtual void ApplyValue(std::string value);
    virtual std::string GetValue() const;

    float GetThreshold() const;

protected:
    const int MAX_THRESHOLD = 100;
    const int MIN_THRESHOLD = 0;
    int threshold = 15;
};

class LastIP : public SettingsField {
public:
    virtual void ApplyValue(std::string value);
    virtual std::string GetValue() const;
protected:
    std::string ip = "127.0.0.1";
};

class Device : public SettingsField {
    public:
    Device(bool _isInput) : isInput(_isInput) {}
    virtual void ApplyValue(std::string value);
    virtual std::string GetValue() const;
protected:
    int id = -1;
    bool isInput = false;
};
