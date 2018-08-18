> 程序和文档由诌在行编写，如有疑问，请联系我
# LED双屏级联接线
## 原理：

利用LED64*32的板子上，output的信号是完全复制input的

## 操作：

我们把第一块板子除了R1、R2、G1、G2四个引脚外的所有其他引脚全部接到第二块板子上

然后第二块板子的R1、R2、G1、G2四个引脚单独接到树莓派上，后面用软件SPI传输数据

## 代码：

```c++
	//The first screen
    digitalWrite(R11, datar11 & 0x80);
    datar11 = datar11 << 1;
    digitalWrite(G11, datag11 & 0x80);
    datag11 = datag11 << 1;
    digitalWrite(R12, datar12 & 0x80);
    datar12 = datar12 << 1;
    digitalWrite(G12, datag12 & 0x80);
    datag12 = datag12 << 1;
```

不管加多少块屏幕，都只需要仿造这个代码写，但是要保证可用IO引脚足够！

## 如图：

![avatar](E:\WorkSpace\LED64-32-Screen-ESP32-Raspberry_Pi_3\双屏级联技术\0.jpg)
![avatar](E:\WorkSpace\LED64-32-Screen-ESP32-Raspberry_Pi_3\双屏级联技术\1.jpg)
![avatar](E:\WorkSpace\LED64-32-Screen-ESP32-Raspberry_Pi_3\双屏级联技术\2.jpg)