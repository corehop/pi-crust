#include "mouse.h"

#define LEFT_BUTTON 1
#define MIDDLE_BUTTON 2
#define RIGHT_BUTTON 3

Mouse::Mouse(FILE *hid_pipe) {
    this->hid_pipe = hid_pipe;
    this->updated_once = false;
    this->x_previous = 0;
    this->y_previous = 0;
    this->x_delta = 0;
    this->y_delta = 0;
    this->wheel_delta = 0; // Initialize wheel delta
}

void Mouse::button_pressed_handler(int button) {
    this->pressed_buttons[button] = true;
    send_mouse_report(); // Send report immediately on button press
}

void Mouse::button_released_handler(int button) {
    this->pressed_buttons[button] = false;
    send_mouse_report(); // Send report immediately on button release
}

void Mouse::update_position(int x, int y) {
    this->x_delta = x;
    this->y_delta = y;
}

void Mouse::wheel_movement_handler(int delta) {
    this->wheel_delta = (int8_t)delta;  // Store wheel movement delta (cast to 8-bit)
    send_mouse_report(); // Send report immediately on wheel movement
}

void Mouse::send_mouse_report() {
    // Encode the buttons as a single byte
    uint8_t buttons = 0;
    if (pressed_buttons[LEFT_BUTTON]) buttons |= (1 << 0);
    if (pressed_buttons[RIGHT_BUTTON]) buttons |= (1 << 1);
    if (pressed_buttons[MIDDLE_BUTTON]) buttons |= (1 << 2);

    // HID report format: [Buttons][X low byte][X high byte][Y low byte][Y high byte][Wheel]
    fprintf(hid_pipe, "%c%c%c%c%c%c",
            buttons,                  // Button states
            (uint8_t)(x_delta & 0xFF),         // X low byte
            (uint8_t)((x_delta >> 8) & 0xFF),  // X high byte
            (uint8_t)(y_delta & 0xFF),         // Y low byte
            (uint8_t)((y_delta >> 8) & 0xFF),  // Y high byte
            wheel_delta);             // Wheel movement

    fflush(hid_pipe); // Ensure immediate write to the HID device

    // Reset deltas after sending the report to avoid repeated movement
    x_delta = 0;
    y_delta = 0;
    wheel_delta = 0; // Reset wheel delta after sending report
}
