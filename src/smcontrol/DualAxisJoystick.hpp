#pragma once

#include "AnalogAxis.hpp"


namespace smcontrol {

/// Джойстик с двумя осями
struct DualAxisJoystick {

private:

    /// Ось джойстика X
    AnalogAxis axis_x;

    /// Ось джойстика Y
    AnalogAxis axis_y;

public:

    explicit DualAxisJoystick(uint8_t pin_x, uint8_t pin_y, float &&filter_k) :
        axis_x{pin_x, filter_k},
        axis_y{pin_y, filter_k} {}

    /// Инициализировать джойстик
    void init() const {
        axis_x.init();
        axis_y.init();
    }

    /// выполнить калибровку центра джойстика
    void calibrate(int iterations) {
        constexpr auto period = 10;

        int sum_x = 0;
        int sum_y = 0;

        for (int i = 0; i < iterations; i++) {
            sum_x += axis_x.readRaw();
            sum_y += axis_y.readRaw();
            delay(period);
        }

        axis_x.updateCenter(sum_x / iterations);
        axis_y.updateCenter(sum_y / iterations);
    }

    /// Возвращаемое значение джойстика
    struct Value {
        /// Нормализованное значение по двум осям
        float x, y;
        /// Вычисленная магнитуда по двум осям
        float magnitude;
    };

    /// Считать значение джойстика
    Value read() {
        const auto x = axis_x.read();
        const auto y = axis_y.read();
        const auto h = std::hypot(x, y);

        if (h < 1e-3) { return {0, 0, 0}; }
        if (h > 1) { return {x / h, y / h, 1}; }
        return {x, y, h};
    }
};
}