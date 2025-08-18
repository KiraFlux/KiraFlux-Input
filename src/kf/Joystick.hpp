#pragma once

#include "AnalogAxis.hpp"


namespace kf {

/// Джойстик с двумя осями
struct Joystick final {

    /// Ось джойстика X
    AnalogAxis axis_x;
    /// Ось джойстика Y
    AnalogAxis axis_y;

    explicit Joystick(gpio_num_t pin_x, gpio_num_t pin_y, float filter_k) noexcept:
        axis_x{pin_x, filter_k},
        axis_y{pin_y, filter_k} {}

    /// Инициализировать джойстик
    inline void init() const noexcept {
        axis_x.init();
        axis_y.init();
    }

    /// выполнить калибровку центра джойстика
    void calibrate(int samples) noexcept {
        constexpr auto period_ms = 1;

        int center_x = 0;
        int center_y = 0;
        int max_dev_x = 0;
        int max_dev_y = 0;

        // Первый проход: вычисление центра
        for (int i = 0; i < samples; i++) {
            const int x = axis_x.readRaw();
            const int y = axis_y.readRaw();
            center_x += x;
            center_y += y;
            delay(period_ms);
        }

        center_x /= samples;
        center_y /= samples;

        // Второй проход: вычисление максимального отклонения
        for (int i = 0; i < samples; i++) {
            const int x = axis_x.readRaw();
            const int y = axis_y.readRaw();

            max_dev_x = std::max(max_dev_x, std::abs(x - center_x));
            max_dev_y = std::max(max_dev_y, std::abs(y - center_y));

            delay(period_ms);
        }

        axis_x.updateCenter(center_x / samples);
        axis_y.updateCenter(center_y / samples);
        axis_x.dead_zone = max_dev_x * 3 / 2;
        axis_y.dead_zone = max_dev_y * 3 / 2;
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