#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>  // for std::max and std::min
#include "keyboard.h"
#include "mouse.h"

enum params { P_EXE, P_DEV_KEYBOARD, P_DEV_MOUSE, NUM_P };

int main(int argc, char **argv) {
    if (argc != NUM_P) {
        fprintf(stderr, "Usage: %s <keyboard device file> <mouse device file>\n", argv[P_EXE]);
        return 1;
    }

    // Set DISPLAY to :0 if it is not set
    if (getenv("DISPLAY") == NULL) {
        printf("DISPLAY not set, defaulting to :0\n");
        setenv("DISPLAY", ":0", 1);
    }

    FILE *kbd_pipe = fopen(argv[P_DEV_KEYBOARD], "w");
    if (!kbd_pipe) {
        perror("Failed to open keyboard device");
        return 1;
    }

    FILE *mouse_pipe = fopen(argv[P_DEV_MOUSE], "w");
    if (!mouse_pipe) {
        perror("Failed to open mouse device");
        fclose(kbd_pipe);
        return 1;
    }

    Display *display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Cannot open display\n");
        fclose(kbd_pipe);
        fclose(mouse_pipe);
        return 1;
    }

    // Get screen resolution
    int screen_width = DisplayWidth(display, DefaultScreen(display));
    int screen_height = DisplayHeight(display, DefaultScreen(display));
    printf("Screen resolution: %dx%d\n", screen_width, screen_height);

    Keyboard kb(kbd_pipe);
    Mouse mouse(mouse_pipe);

    Window window = DefaultRootWindow(display);
    XSelectInput(display, window, KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask);

    int last_x = -1, last_y = -1;

    while (1) {
        while (XPending(display)) {
            XEvent event;
            XNextEvent(display, &event);

            if (event.type == KeyPress) {
                kb.key_down_handler(XLookupKeysym(&event.xkey, 0));
                kb.send_keyboard_reports();
            } else if (event.type == KeyRelease) {
                kb.key_up_handler(XLookupKeysym(&event.xkey, 0));
                kb.send_keyboard_reports();
            } else if (event.type == MotionNotify) {
                // Capture current mouse position
                int current_x = event.xmotion.x;
                int current_y = event.xmotion.y;

                // Initialize last position
                if (last_x == -1 || last_y == -1) {
                    last_x = current_x;
                    last_y = current_y;
                    continue;
                }

                // Calculate relative movement
                int delta_x = current_x - last_x;
                int delta_y = current_y - last_y;

                // Clamp the deltas to the 16-bit HID range (-32768 to 32767)
                delta_x = std::max(-32768, std::min(32767, delta_x));
                delta_y = std::max(-32768, std::min(32767, delta_y));

                // Update mouse position and send report
                if (delta_x != 0 || delta_y != 0) {
                    mouse.update_position(delta_x, delta_y);
                    mouse.send_mouse_report();

                    // Update last position
                    last_x = current_x;
                    last_y = current_y;
                }
            } else if (event.type == ButtonPress) {
                if (event.xbutton.button == Button4) {
                    mouse.wheel_movement_handler(1); // Wheel up
                } else if (event.xbutton.button == Button5) {
                    mouse.wheel_movement_handler(-1); // Wheel down
                } else {
                    mouse.button_pressed_handler(event.xbutton.button);
                }
            } else if (event.type == ButtonRelease) {
                if (event.xbutton.button != Button4 && event.xbutton.button != Button5) {
                    mouse.button_released_handler(event.xbutton.button);
                }
            }
        }

        // Continuously send keyboard reports to ensure responsiveness
        kb.send_keyboard_reports();
    }

    fclose(kbd_pipe);
    fclose(mouse_pipe);
    XCloseDisplay(display);
    return 0;
}
