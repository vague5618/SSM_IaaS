#!/bin/bash
qemu --enable-kvm -sdl -m 1024 -cpu kvm64 -vga vmware -soundhw ac97 -usbdevice tablet -hda ~/PROJECT/$1/androidx86.img

