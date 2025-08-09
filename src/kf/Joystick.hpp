#pragma once

#include "AnalogAxis.hpp"


namespace kf {

/// Джойстик с двумя осями
struct Joystick final {

    /// Ось джойстика X
    AnalogAxis axis_x;
    /// Ось джойстика Y
    AnalogAxis axis_y;

    explicit Joystick(uint8_t pin_x, uint8_t pin_y, float &&filter_k) noexcept:
        axis_x{pin_x, filter_k},
        axis_y{pin_y, filter_k} {}

    /// Инициализировать джойстик
    inline void init() const noexcept {
        axis_x.init();
        axis_y.init();
    }

    /// выполнить калибровку центра джойстика
    void calibrate(int samples) noexcept {
        constexpr auto period = 10;

        int sum_x = 0;
        int sum_y = 0;

        for (int i = 0; i < samples; i++) {
            sum_x += axis_x.readRaw();
            sum_y += axis_y.readRaw();
            delay(period);
        }

        axis_x.updateCenter(sum_x / samples);
        axis_y.updateCenter(sum_y / samples);
    }

    /// Возвращаемое значение джойстика
    struct Data {
        /// Нормализованное значение по двум осям [0.0..1.0]
        float x, y;
        /// Магнитуда по двум осям [0.0..1.0]
        float magnitude;
    };

    /// Считать значение джойстика
    Data read() noexcept {
        const auto x = axis_x.read();
        const auto y = axis_y.read();
        const auto h = std::hypot(x, y);

        if (h < 1e-3) { return {0, 0, 0}; }
        if (h > 1) { return {x / h, y / h, 1}; }
        return {x, y, h};
    }
};
}