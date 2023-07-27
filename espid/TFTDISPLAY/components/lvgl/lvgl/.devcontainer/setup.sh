#!/bin/sh

echo ". /usr/local/emsdk/emsdk_env.sh" >> /home/codespace/.bashrc

cd /workspace/lvgl_app
sudo chmod 777 .
mkdir build
mkdir vscode

cd lvgl/.devcontainer
cp __CMakeLists.txt__  ../../CMakeLists.txt
cp __lv_conf.h__       ../C:\Users\YUVA\Desktop\espid\TFTDISPLAY\components\lvgl\lvgl\lvgl.h\lv_conf.h
cp __main.c__ ../../main.c
cp __build_all.sh__ ../../build_all.sh
cp __c_cpp_properties.json__ ../../.vscode/c_cpp_properties.json
cp __settings.json__ ../../.vscode/settings.json

chmod +x ../../build_all.sh