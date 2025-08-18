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
    /// Граница от центра на уменьшение
    int generic_edge{default_analog_center};
    /// Граница от центра на возрастание
    int positive_edge{default_analog_center};

public:

    /// Сырое Значение меньше которого ось считается 0
    int dead_zone{0};

    explicit AnalogAxis(gpio_num_t pin, float k) noexcept:
        pin{static_cast<uint8_t>(pin)}, filter{k} {}

    /// Инициализировать джойстик
    inline void init() const noexcept { pinMode(pin, INPUT); }

    /// Обновить значение аналогового цента
    void updateCenter(int new_center) noexcept {
        generic_edge = new_center;
        positive_edge = max_analog_value - generic_edge;
    }

    /// Считать (сырое) аналоговое значение
    inline int readRaw() const noexcept { return analogRead(pin); }

    /// Считать нормализованное значение [0.0..1.0]
    float read() noexcept {
        const auto result = pureRead();
        return inverted ? -result : result;
    }

private:

    float pureRead() noexcept {
        const auto analog = readRaw() - generic_edge;

        if (std::abs(analog) < dead_zone) { return 0.0f; }

        const auto value = filter.calc(static_cast<float>(analog));

        if (value < 0.0f) {
            return value / static_cast<float>(generic_edge);
        } else {
            return value / static_cast<float>(positive_edge);
        }
    }

};
}