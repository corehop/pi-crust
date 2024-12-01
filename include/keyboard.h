#include <map>
#include <X11/Xlib.h>
#include <cstdio>

#pragma once

class Keyboard {
public:
    Keyboard(FILE *hid_pipe);

    void key_down_handler(unsigned long key_sym);
    void key_up_handler(unsigned long key_sym);
    void send_keyboard_reports();

private:
    std::map<KeySym, bool> held_keys;
    FILE *hid_pipe;

    bool is_modifier(unsigned long key_sym);
    unsigned char get_modifier_report(std::map<KeySym, bool> held_keys);
};
