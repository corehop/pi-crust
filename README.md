# KVM USB HID Control Project

This project provides a virtual KVM (Keyboard, Video, Mouse) solution for controlling a target computer using a Raspberry Pi Zero 2 W. It includes functionality for capturing keyboard and mouse events and sending them as USB HID reports over a virtual USB connection.

The project uses `Barrier` to connect to the Raspberry Pi from a host machine, allowing the control of both the mouse and keyboard for a target machine.

## Features

- **Keyboard and Mouse Control**: Captures keyboard and mouse input and sends it to a target system using a USB HID device.
- **Mouse Wheel Support**: Includes support for scrolling via the mouse wheel.
- **X11 Integration**: Uses an X11 server to capture input from a desktop environment.
- **Automatic Startup**: Systemd services for running X, Barrier, and the KVM executable on boot.

## System Requirements

- Raspberry Pi Zero 2 W or similar device.
- Linux operating system with X11 installed.
- Barrier for capturing mouse and keyboard input from the host.
- USB OTG capabilities for emulating a USB HID device.

## Installation Instructions

### Prerequisites

- A working Linux distribution (such as Raspbian) on your Raspberry Pi Zero 2 W.
- Make sure that your Raspberry Pi has USB OTG support enabled.

### Build and Install

1. **Clone the Repository**:
    ```sh
    git clone <your-repo-url>
    cd <your-repo-directory>
    ```

2. **Build the Project**:
    To compile the KVM executable, run:
    ```sh
    make
    ```

3. **Install Dependencies and Services**:
    Run the following to install necessary packages and configure services:
    ```sh
    sudo make install
    ```

    The `install` target performs the following actions:
    - Installs required packages: `xserver-xorg`, `xinit`, `x11-xserver-utils`, `xserver-xorg-video-dummy`, `barrier`.
    - Sets up necessary files such as `xorg.conf` for the dummy X server.
    - Copies the `kvm` binary to `/usr/bin/kvm`.
    - Installs systemd services for automatic startup.

### Systemd Services

The installation will set up three services:
- **x.service**: Starts the X11 server.
- **barrierc.service**: Connects to the Barrier server.
- **kvm.service**: Starts the KVM HID control script.

To manually start or enable these services:

```sh
sudo systemctl start x.service
sudo systemctl start barrierc.service
sudo systemctl start kvm.service

sudo systemctl enable x.service
sudo systemctl enable barrierc.service
sudo systemctl enable kvm.service
```

### Uninstall

To remove the installed files and services, run:

```sh
make clean
```

This command will remove the built executable from the `bin` directory. For manually removing installed services and files, execute the following:

```sh
sudo rm /usr/bin/kvm /usr/bin/isticktoit_usb
sudo rm /etc/X11/xorg.conf
sudo rm /etc/systemd/system/kvm.service /etc/systemd/system/x.service /etc/systemd/system/barrierc.service
sudo systemctl daemon-reload
```

## Usage

1. **Connect the Raspberry Pi Zero to Your Host Machine**:
   - Connect the Pi via a USB cable. Make sure the USB connection supports OTG mode.

2. **Run the System**:
   - Once the system starts, the Raspberry Pi will launch `X11`, connect to the `Barrier` server, and run the KVM executable to emulate keyboard and mouse events to the target computer.

3. **Barrier Setup**:
   - On the host machine, set up `Barrier` to share control with the Raspberry Pi. The Raspberry Pi will connect as a client with the name `kvm-01`.

4. **Testing**:
   - Move your mouse and press keys on the host. You should see the actions being reflected on the target computer connected to the Raspberry Pi.

## Configuration

- **X11 Configuration**:
  - The X11 server uses a dummy display configured by `xorg.conf`. You can find and modify this configuration file in the `scripts/` directory to match your desired resolution.

- **Barrier Configuration**:
  - The `barrierc.service` is configured to use the `kvm-01` hostname and connect to the Barrier server IP address `10.0.1.6`. Update the IP address in `scripts/barrierc.service` to match your Barrier server's IP if different.

## Development

### Directory Structure

- **src/**: Source code for the project.
- **include/**: Header files.
- **bin/**: Directory where compiled executables are placed.
- **scripts/**: Setup scripts and systemd service files.

### Building

To build the project after making changes, use:

```sh
make
```

### Running the Project

To build and immediately run the KVM project:

```sh
make run
```

This will clean, build, and run the executable.

## Troubleshooting

- **"Cannot open display" Error**:
  - Ensure that `DISPLAY` is set to `:0`. This is done automatically in the startup script, but if it persists, check your X11 installation and permissions.

- **Mouse or Keyboard Not Working**:
  - Verify that the USB gadget is correctly configured using `ls /dev/hidg*`. If the device nodes are missing, re-run the USB gadget setup script.

- **Barrier Client Issues**:
  - If the client fails to connect to the Barrier server, verify that the server IP address is correct and that there are no network issues.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgments

- This project is a fork of [pi-crust](https://github.com/frbncis/pi-crust). Many thanks to the original author for providing the foundation for this work.
- Thanks to the creators of [Barrier](https://github.com/debauchee/barrier) for providing open-source software for sharing input devices across machines.
- Special thanks to the community for resources on USB HID emulation, X11 configuration, and Raspberry Pi usage.

## Contributions

Contributions are welcome! Please feel free to submit issues, fork the repository, and create pull requests.

## Contact

For any questions, please create an issue in this repository.