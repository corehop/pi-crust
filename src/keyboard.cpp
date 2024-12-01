#include "keyboard.h"
#include <X11/keysymdef.h>
#include "scancodes.h"
#include <iostream>

Keyboard::Keyboard(FILE *hid_pipe) {
    this->hid_pipe = hid_pipe;
}

void Keyboard::key_down_handler(unsigned long key_sym) {
    this->held_keys[key_sym] = true;
}

void Keyboard::key_up_handler(unsigned long key_sym) {
    this->held_keys.erase(key_sym);
}

void Keyboard::send_keyboard_reports() {
    unsigned char report_buffer[6] = {0, 0, 0, 0, 0, 0};
    unsigned char modifiers = get_modifier_report(held_keys);

    int buffer_position = 0;

    for (const auto &[key, is_held] : held_keys) {
        if (!is_modifier(key) && is_held && buffer_position < 6) {
            unsigned char *scancode = get_scancode(key);
            if (scancode) report_buffer[buffer_position++] = *scancode;
        }
    }

    fprintf(
        hid_pipe, "%c%c%c%c%c%c%c%c",
        modifiers, 0,
        report_buffer[0], report_buffer[1],
        report_buffer[2], report_buffer[3],
        report_buffer[4], report_buffer[5]
    );
    fflush(hid_pipe); // Ensure immediate write
}

bool Keyboard::is_modifier(unsigned long key_sym) {
    return key_sym == XK_Control_L || key_sym == XK_Shift_L || key_sym == XK_Alt_L || key_sym == XK_Super_L ||
           key_sym == XK_Control_R || key_sym == XK_Shift_R || key_sym == XK_Alt_R || key_sym == XK_Super_R;
}

unsigned char Keyboard::get_modifier_report(std::map<KeySym, bool> held_keys) {
    return (held_keys[XK_Control_L] << 0) +
           (held_keys[XK_Shift_L] << 1) +
           (held_keys[XK_Alt_L] << 2) +
           (held_keys[XK_Super_L] << 3) +
           (held_keys[XK_Control_R] << 4) +
           (held_keys[XK_Shift_R] << 5) +
           (held_keys[XK_Alt_R] << 6) +
           (held_keys[XK_Super_R] << 7);
}
