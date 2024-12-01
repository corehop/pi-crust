#include <map>
#include <cstdio>
#include <string>

#pragma once

class Mouse {
public:
    Mouse(FILE *hid_pipe);

    void update_position(int x, int y);
    void button_pressed_handler(int button);
    void button_released_handler(int button);
    void wheel_movement_handler(int delta);
    void send_mouse_report();

private:
    FILE *hid_pipe;

    bool updated_once;
    int x_previous;
    int y_previous;
    int16_t x_delta;
    int16_t y_delta;
    int8_t wheel_delta;  // Added to store wheel movement
    std::map<int, bool> pressed_buttons; // Stores the state of buttons
};
