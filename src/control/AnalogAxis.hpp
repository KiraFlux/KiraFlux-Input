#pragma once

#include <esp32-hal-gpio.h>
#include <esp32-hal-adc.h>

#include "tfb/Exponential.hpp"
#include "tfb/Median.hpp"


namespace joystick {

/// Джойстик с одной осью
struct AnalogAxis {

private:

    /// Максимальное аналоговое значение
    static constexpr auto max_analog_value = 4095;
    /// Аналоговый центр по умолчанию (Среднее значение)
    static constexpr auto default_analog_center = max_analog_value / 2;

    /// Пин подключения джойстика
    const uint8_t pin;
    /// Внешний фильтр значений
    tfb::Exponential<float> outer_filter;
    /// Внутренний фильтр аналоговых значений
    tfb::MedianFilter<int, 5> inner_filter{default_analog_center};
    /// Граница от центра на уменьшение
    float generic_edge{default_analog_center};
    /// Граница от центра на возрастание
    float positive_edge{default_analog_center};

public:

    explicit AnalogAxis(uint8_t pin, const float &k) :
        pin{pin}, outer_filter{k} {}

    /// Инициализировать джойстик
    inline void init() const { pinMode(pin, INPUT); }

    /// Обновить значение аналогового цента
    void updateCenter(int new_center) {
        generic_edge = float(new_center);
        positive_edge = max_analog_value - generic_edge;
    }

    /// Считать (сырое) аналоговое значение
    inline int readRaw() const { return analogRead(pin); }

    /// Считать нормализованное значение
    float read() {
        const auto raw = inner_filter.calc(readRaw());
        const auto value = outer_filter.calc(float(raw) - generic_edge);

        if (value < 0) {
            return value / generic_edge;
        } else {
            return value / positive_edge;
        }
    }
};
}