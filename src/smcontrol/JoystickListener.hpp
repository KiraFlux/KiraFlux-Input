#pragma once

namespace smcontrol {

/// Прослушивает изменения джойстика
struct JoystickListener {

public:

    /// Событие джойстика (Срабатывают однократно)
    enum class Direction {
        /// Джойстик переместился исходную позицию
        Home,
        /// Джойстик переместился вверх
        Up,
        /// Джойстик переместился вниз
        Down,
        /// Джойстик переместился влево
        Left,
        /// Джойстик переместился вправо
        Right,
    };

    /// Обработчик событий джойстика
    std::function<void(Direction)> handler{nullptr};
    /// Порог срабатывания
    const float threshold{0.2};

private:

    /// Предыдущее направление
    Direction last_direction{Direction::Home};
    /// Прослушиваемый джойстик
    Joystick &joystick;

public:

    explicit JoystickListener(Joystick &joy) :
        joystick{joy} {}

    /// Пул обновлений событий джойстика
    void pull() {
        if (not handler) {
            return;
        }

        const auto current_direction = getCurrentMove();

        if (last_direction != current_direction) {
            handler(current_direction);
            last_direction = current_direction;
        }
    }

private:

    Direction getCurrentMove() {
        const auto x = joystick.axis_x.read();
        const auto y = joystick.axis_y.read();
        const auto abs_x = std::abs(x);
        const auto abs_y = std::abs(y);

        if (abs_x < threshold and abs_y < threshold) {
            return Direction::Home;
        }

        if (abs_x > abs_y) {
            return x > 0 ? Direction::Right : Direction::Left;
        } else {
            return y > 0 ? Direction::Up : Direction::Down;
        }
    }

};

}