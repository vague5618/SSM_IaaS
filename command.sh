#!/bin/bash

COMMAND=$1
ID=$2
OS=$3
INDEX=$4
ANDROID=$5

if [ "$COMMAND" -eq 0 ];then
	cd ./"$ID"
	echo "start qemu"
	if [ "$OS" = "windows7" ] || [ "$OS" = "ubuntu" ];then
		qemu --enable-kvm -sdl -m 1024 -cpu kvm64 -vga cirrus -soundhw es1370 -usbdevice tablet -hda "$OS"_"$INDEX".img
	elif [ "$OS" = "tizen" ];then
		../tizen/qemu/i386-softmmu/qemu-system-i386 -c ../tizen/info.ini --resolution 1280x768
	elif [ "$OS" = "android" ];then
		qemu --enable-kvm -sdl -m 1024 -cpu kvm64 -vga std -soundhw es1370 -usbdevice tablet -hda "$OS"_"$INDEX".img
	fi
	
elif [ "$COMMAND" -eq 1 ];then
	echo "create"
	mkdir "$ID"
	if [ "$OS" = "windows7" ] || [ "$OS" = "ubuntu" ];then
		qemu-img create ./"$ID"/"$OS"_"$INDEX".img 100G

	elif [ "$OS" = "android" ];then
		mv ./baseImage/"$OS"_"$ANDROID".img ./"$ID"/"$OS"_"$INDEX".img
	fi

elif [ "$COMMAND" -eq 2 ];then
	echo "delete"
	rm -rf ./"$ID"/"$OS"_"$INDEX".img

else
	cd ./"$ID"
	echo "install"
	qemu --enable-kvm -sdl -m 1024 -cpu kvm64 -vga cirrus -soundhw ac97 -usbdevice tablet -hda "$OS"_"$INDEX".img -cdrom ../baseImage/"$OS".iso -boot dc
fi

