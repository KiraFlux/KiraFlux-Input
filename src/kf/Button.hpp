#pragma once

#include <Arduino.h>
#include <functional>


namespace kf {

/// Тактовая кнопка
struct Button {

    enum class Mode : uint8_t {
        /// Режим подтягивания вверх
        PullUp,
        /// Режим подтягивания вниз
        PullDown
    };

private:

    /// Длительность дребезга в миллисекундах
    static constexpr auto debounce_ms = 50;

public:

    /// Номер пина кнопки
    const uint8_t pin;
    /// Режим работы кнопки
    const Mode mode;
    /// Обработчик нажатия кнопки
    std::function<void()> handler{nullptr};

private:

    /// Предыдущее состояние кнопки
    bool last_state{false};
    /// Время последнего нажатия
    uint32_t last_press_ms{0};

public:

    explicit Button(gpio_num_t pin, Mode mode = Mode::PullDown) :
        pin{static_cast<uint8_t>(pin)}, mode{mode} {}

    /// Инициализация кнопки
    inline void init(bool external_pull) const noexcept {
        pinMode(pin, matchMode(external_pull));
    }

    /// Обновление состояния кнопки
    void poll() noexcept {
        const auto current_state = read();
        const auto now = millis();

        if (current_state and not last_state) {
            if (now - last_press_ms > debounce_ms) {
                if (handler) {
                    handler();
                }
                last_press_ms = now;
            }
        }

        last_state = current_state;
    }

    /// Считать значение кнопки
    bool read() const noexcept {
        if (mode == Mode::PullUp) {
            return not digitalRead(pin);
        }
        return digitalRead(pin);
    }

private:

    inline int matchMode(bool external) const noexcept {
        if (external) {
            return INPUT;
        }

        if (mode == Mode::PullUp) {
            return INPUT_PULLUP;
        }

        return INPUT_PULLDOWN;
    }
};

} // namespace kf
