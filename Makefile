# Compiler, directories, and executable setup
CXX             := g++
CXX_FLAGS       := -Wall -Wextra -std=c++17 -ggdb
LINKER_FLAGS    := -DXK_MISCELLANY=1 -DXK_LATIN1=1
BIN             := bin
SRC             := src
INCLUDE         := include
LIB             := LIB
LIBRARIES       := -lX11 -lXi
EXECUTABLE      := kvm

# Default target: Build the executable
all: $(BIN)/$(EXECUTABLE)

run: clean all
    clear
    @echo "Executing..."
    ./$(BIN)/$(EXECUTABLE) /dev/hidg0 /dev/hidg1

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
    @echo "Building..."
    $(CXX) $^  $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) -o $@ $(LIBRARIES) $(LINKER_FLAGS)

install: scripts/*
    @echo "Installing dependencies and setting up the system..."

    # Install necessary packages
    @echo "Installing X server, Barrier, and related tools..."
    apt-get update && apt-get install --no-install-recommends -y xserver-xorg xinit x11-xserver-utils xserver-xorg-video-dummy barrier

    # Copy configuration files
    @echo "Copying configuration files..."
    cp scripts/xorg.conf /etc/X11/xorg.conf || echo "Failed to copy xorg.conf, please check permissions"
    cp scripts/isticktoit_usb /usr/bin/isticktoit_usb
    chmod +x /usr/bin/isticktoit_usb
    cp $(BIN)/$(EXECUTABLE) /usr/bin/kvm
    chmod +x /usr/bin/kvm

    # Install systemd service files
    @echo "Installing systemd service files..."
    cp scripts/kvm.service /etc/systemd/system/kvm.service
    cp scripts/x.service /etc/systemd/system/x.service
    cp scripts/barrierc.service /etc/systemd/system/barrierc.service

    chmod 644 /etc/systemd/system/kvm.service
    chmod 644 /etc/systemd/system/x.service
    chmod 644 /etc/systemd/system/barrierc.service

    # Reload systemd services
    @echo "Reloading systemd daemon to apply new services..."
    systemctl daemon-reload

    # Enable and start services
    @echo "Enabling and starting services..."
    systemctl enable x.service barrierc.service kvm.service
    systemctl start x.service barrierc.service kvm.service

    @echo "Installation complete."

clean:
    @echo "Cleaning..."
    -rm $(BIN)/*
