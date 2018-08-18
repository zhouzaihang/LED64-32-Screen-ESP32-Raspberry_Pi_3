> 程序和文档由诌在行编写，如有疑问，请联系我
# LED双屏级联接线

*这里以树莓派上的双屏级联代码为例*

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

完整的双屏级联代码如下：

```c++
    void hc595senddata(byte datar11, byte datar12, byte datag11, byte datag12, byte datar21, byte datar22, byte datag21, byte datag22)
    {
      for (byte i = 0; i < 8; i++)
      {
        //The first screen
        digitalWrite(R11, datar11 & 0x80);
        datar11 = datar11 << 1;
        digitalWrite(G11, datag11 & 0x80);
        datag11 = datag11 << 1;
        digitalWrite(R12, datar12 & 0x80);
        datar12 = datar12 << 1;
        digitalWrite(G12, datag12 & 0x80);
        datag12 = datag12 << 1;

        //The second screen
        digitalWrite(R21, datar21 & 0x80);
        datar21 = datar21 << 1;
        digitalWrite(G21, datag21 & 0x80);
        datag21 = datag21 << 1;
        digitalWrite(R22, datar22 & 0x80);
        datar22 = datar22 << 1;
        digitalWrite(G22, datag22 & 0x80);
        datag22 = datag22 << 1;

        digitalWrite(CLK, HIGH);//拉高时钟信号，上升沿发送数据
        digitalWrite(CLK, LOW);//恢复低电平
      }
}
```



## 接线如图：

![avatar](E:\WorkSpace\LED64-32-Screen-ESP32-Raspberry_Pi_3\双屏级联技术\0.jpg)
![avatar](E:\WorkSpace\LED64-32-Screen-ESP32-Raspberry_Pi_3\双屏级联技术\1.jpg)
![avatar](E:\WorkSpace\LED64-32-Screen-ESP32-Raspberry_Pi_3\双屏级联技术\2.jpg)