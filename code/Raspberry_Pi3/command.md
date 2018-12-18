# 编译
gcc -Wall -o blink blink.cpp -lwiringPi
# 运行
sudo ./blink
# ffmeg视频转换为图片
ffmpeg -i bad_apple.mp4 -s 64*64 -an %04d.png
