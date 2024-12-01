#!/bin/bash
# USB HID gadget setup script for Raspberry Pi Zero 2 W
# This script sets up a composite USB HID device with keyboard and mouse interfaces
# Move this file to your preferred location and make it executable using chmod +x usb_setup.sh

echo "Starting USB setup script..."
modprobe libcomposite
cd /sys/kernel/config/usb_gadget/
mkdir -p usb
cd usb

# Basic device information
echo 0x1d6b > idVendor       # Linux Foundation
echo 0x0104 > idProduct      # Multifunction Composite Gadget
echo 0x0100 > bcdDevice      # v1.0.0
echo 0x0200 > bcdUSB         # USB2

# Device strings
mkdir -p strings/0x409
echo "83729910693277907514" > strings/0x409/serialnumber
echo "Logitech" > strings/0x409/manufacturer
echo "USB Composite Device" > strings/0x409/product

# Configuration
mkdir -p configs/c.1/strings/0x409
echo "Config 1: HID Composite" > configs/c.1/strings/0x409/configuration
echo 250 > configs/c.1/MaxPower

# Keyboard HID function
mkdir -p functions/hid.keyboard
echo 1 > functions/hid.keyboard/protocol
echo 1 > functions/hid.keyboard/subclass
echo 8 > functions/hid.keyboard/report_length
echo -ne \
  "\\x05\\x01\\x09\\x06\\xa1\\x01\\x05\\x07\\x19\\xe0\\x29\\xe7\\x15\\x00\\x25\\x01\\x75\\x01\\x95\\x08\\x81\\x02\\x95\\x01\\x75\\x08\\x81\\x03\\x95\\x06\\x75\\x08\\x15\\x00\\x25\\x65\\x05\\x07\\x19\\x00\\x29\\x65\\x81\\x00\\xc0" \
  > functions/hid.keyboard/report_desc

# Mouse HID function with 16-bit X and Y movement
mkdir -p functions/hid.mouse
echo 2 > functions/hid.mouse/protocol
echo 1 > functions/hid.mouse/subclass
echo 6 > functions/hid.mouse/report_length
echo -ne \
  "\\x05\\x01\\x09\\x02\\xa1\\x01\\x09\\x01\\xa1\\x00\\x05\\x09\\x19\\x01\\x29\\x03\\x15\\x00\\x25\\x01\\x75\\x01\\x95\\x03\\x81\\x02\\x95\\x01\\x75\\x05\\x81\\x03\\x05\\x01\\x09\\x30\\x09\\x31\\x16\\x00\\x80\\x26\\xff\\x7f\\x75\\x10\\x95\\x02\\x81\\x06\\xc0\\xc0" \
  > functions/hid.mouse/report_desc

# Bind functions to the configuration
ln -s functions/hid.keyboard configs/c.1/
ln -s functions/hid.mouse configs/c.1/

# Enable the USB gadget
ls /sys/class/udc > UDC

echo "USB HID gadget setup completed."
