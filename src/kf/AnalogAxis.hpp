#pragma once

#include <esp32-hal-gpio.h>
#include <esp32-hal-adc.h>

#include "tfb/Exponential.hpp"
#include "tfb/Median.hpp"


namespace kf {

/// Джойстик с одной осью
struct AnalogAxis final {

private:

    /// Максимальное аналоговое значение
    static constexpr auto max_analog_value = 4095;
    /// Аналоговый центр по умолчанию (Среднее значение)
    static constexpr auto default_analog_center = max_analog_value / 2;

public:

    /// Ось инвертирована
    bool inverted{false};

private:

    /// Пин подключения джойстика
    const uint8_t pin;
    /// фильтр значений
    tfb::Exponential<float> filter;
    int range_negative{default_analog_center};
    int range_positive{max_analog_value - default_analog_center};

public:

    /// Сырое Значение меньше которого ось считается 0
    int dead_zone{0};

    explicit AnalogAxis(gpio_num_t pin, float k) noexcept:
        pin{static_cast<uint8_t>(pin)}, filter{k} {}

    /// Инициализировать джойстик
    inline void init() const noexcept { pinMode(pin, INPUT); }

    /// Обновить значение аналогового цента
    void updateCenter(int new_center) noexcept {
        range_negative = new_center;
        range_positive = max_analog_value - new_center;
    }

    /// Считать (сырое) аналоговое значение
    inline int readRaw() const noexcept { return analogRead(pin); }

    /// Считать нормализованное значение [0.0..1.0]
    float read() noexcept {
        return inverted ? -pureRead() : pureRead();
    }

private:

    float pureRead() noexcept {
        const auto deviation = readRaw() - range_negative;

        if (std::abs(deviation) < dead_zone) {
            return 0.0f;
        }

        const auto filtered = filter.calc(static_cast<float>(deviation));

        if (filtered < 0.0f) {
            return filtered / static_cast<float>(range_negative);
        } else {
            return filtered / static_cast<float>(range_positive);
        }
    }

};
}