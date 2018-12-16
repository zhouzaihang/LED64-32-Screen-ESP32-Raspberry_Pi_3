------

>  本文档由BlackWalnut——诌在行编辑，如有疑问，请联系我
>
>  部分图片源于百度

# LED点阵原理

## LED点阵点亮原理

> 以8 * 8点阵为例，更容易讲清楚点阵的基本原理

### 基本结构

一颗LED灯就是一个脚接高电平，一个脚接低电平来点亮的，而点阵就是一行LED的高电平或低电平脚级联，一列的高电平或低电平脚级联

![88点阵](https://github.com/zhouzaihang/LED64-32-Screen-ESP32-Raspberry_Pi_3/blob/master/image/88点阵.jpg)

当第一行接入高电平，第一列接入低电平，其它列高电平，那第一个led灯点亮

### 双色点阵

上面的是单色的，红绿双色的点阵也是一样的原理，一模一样的电路COPY了一份，然后把两个灯放在同一个盒子里面

![88双色](https://github.com/zhouzaihang/LED64-32-Screen-ESP32-Raspberry_Pi_3/blob/master/image/88双色.jpg)

如图，在64 * 32的点阵中可以通过H1~H8接入高电平来选定行，同时通过G1接入低电平来控制第一行第一个绿色LED灯发亮

同理，可控制每一个红色和绿色的LED灯有没有亮起来

### LED扫描方式

LED采用的是逐行刷新显示，每次只能选定一行，给一行高电平，控制这一行上所有LED灯的亮暗情况

LED常见扫描方式有静态、1/2、1/4、1/8、1/16，每次刷新一行，16行是一个扫描周期。一个扫描周期（主要取决于程序效率和硬件（ESP32、Raspberry）的时钟周期，因为实践来看点阵本身的刷新速度是足够快的）必须要小于肉眼视觉暂留时间（0.05s~0.2s），否则就会出现LED点阵出现的画面一直闪烁的情况。

## HUB08接口

### 接口说明

![HUB08](https://github.com/zhouzaihang/LED64-32-Screen-ESP32-Raspberry_Pi_3/blob/master/image/HUB08.jpg)

- LA、LB、LC、LD 连接两个3-8译码器74HC138芯片实现行信号传输，74HC138介绍在后面
- R1、R2、G1、G2 分别用来传输上半屏红灯信号、下面屏红灯、上半屏绿灯和下半屏绿灯的信号
- SCK（CLK/CK）时钟信号
- STB（LE/LT）锁存列信号数据，和上面的R1、R2、G1、G2、SCK配合用软件SPI的通信协议给74HC595芯片传输列数据，这块芯片在后面会讲到
- GND 接地，众所周知
- EN 显示使能信号

### 接线示例

按照源代码（“code”文件夹下）接线即可

# 点亮屏幕思路

- 初始化GPIO引脚
- 设置08接口使能信号
- 用行信号选中第1行，移位串行输入，分别发送上下屏第1行的列信号数据，锁存列信号。并行输出显示
- 依次循环，每次循环发送的是上下屏红绿两种列信号数据，直到16行发送完（如果是两屏级联，那每次发送的就是两块屏幕上下屏的红绿列信号数据，也就是每次发送四行的列信号数据）
- 重复循环，维持LED显示

# 代码实现（以ESP32代码为例）

## 初始化

```c++
#define RowA 22
#define RowB 23
#define RowC 21
#define RowD 19
#define OE 18 //EN口

#define R1 17         //数据出    MOSI 输出 main out
#define R2 16
#define G1 2
#define G2 25
#define LE 26       //时钟    SCK
#define CLK 27      //595 刷新显示  SS 片选

void setup()
{
	pinMode(22, OUTPUT);
	pinMode(23, OUTPUT);
	pinMode(21, OUTPUT);
	pinMode(19, OUTPUT);
	pinMode(18, OUTPUT);
	pinMode(17, OUTPUT);
	pinMode(16, OUTPUT);
	pinMode(2, OUTPUT);
	pinMode(25, OUTPUT);
	pinMode(26, OUTPUT);
	pinMode(27, OUTPUT);
	//初始化引脚
	//Serial.begin(9600);
	digitalWrite(OE, LOW);
	digitalWrite(LE, LOW);
	digitalWrite(CLK, LOW);
    //初始化软SPI
}
```

## 74HC138芯片进行行选

### 74HC138基本原理

![74HC138](https://github.com/zhouzaihang/LED64-32-Screen-ESP32-Raspberry_Pi_3/blob/master/image/74HC138.png)

3-8译码器的作用就是把3种输入状态翻译成8种输出状态。如上图，1 ~ 6都是输入，但是4 ~ 6都是使能，剩下的1 ~ 3刚好能表示出8种状态（2^3^ = 8）

两块三八译码器级联就成了四六译码器（2^4^ = 16），所以刚刚好能满足控制一半的LED点整屏（16行）的需求，HUB08接口的ABCD就是这四个输入引脚，通过四个引脚，可以实现下面的功能：

| D    | C    | B    | A    | Row                |
| ---- | ---- | ---- | ---- | ------------------ |
| 0    | 0    | 0    | 0    | 上下屏幕第一行     |
| 0    | 0    | 0    | 1    | 上下屏幕的第二行   |
| ……   | ……   | ……   | ……   | ……                 |
| 1    | 1    | 1    | 1    | 上下屏幕的第十六行 |

### 代码实现

```c++
void hc138sacn(word r) {  //输出行线状态ABCD
	digitalWrite(RowA, (r & 0x01));
	digitalWrite(RowB, (r & 0x02));
	digitalWrite(RowC, (r & 0x04));
	digitalWrite(RowD, (r & 0x08));
}
```

## 74HC595芯片进行列驱动

### 74HC595基本原理

该芯片具有：8位串行输入转8位并行输出，8位锁存的功能，往芯片传输数据用的是SPI的通信协议。对于64 * 32的点阵，因为有64列，所以需要通过8个74HC595芯片进行级联，即可将其扩展成64位串行输入转64位并行输出，从而通过在选定一行后，对64个列信号数据传输，实现对一行的64个LED灯进行控制。HUB08接口中的R1、R2、G1、G2就是用来输入64位串行数据的。其中R1、G1用来控制上半点阵（16行）的红绿信号；R2、G2分别用来控制下半点阵（16行）的红绿信号。因此，64 * 32的双色点阵总共需要32个74HC595芯片。

然而不幸的是，硬件SPI只能传输一行的列信号数据。而我们需要的是选定上下屏的相同行后，通过R1、R2、G1、G2传输那两行的红绿列信号数据。也就是说，行选定一行后，需要用相同的时钟信号和片选信号传输4行的列信号数据。明显，硬件SPI已经没有办法满足64 * 32的显示了。当然，上帝为你关上一扇门，必定会为你打开一扇窗，硬件SPI不行，就用软件SPI（具体实现见代码）。

### 代码实现

- SPI通信协议自行百度

```c++
void hc595senddata(byte datar1, byte datar2, byte datag1, byte datag2)
{
	for (byte i = 0; i < 8; i++)
	{
		digitalWrite(R1, datar1 & 0x80);//用digitalwrite直接写入byte的最高位
		datar1 = datar1 << 1;			//byte数据左移，使下一次要读入的数据变成最高位
		digitalWrite(G1, datag1 & 0x80);
		datag1 = datag1 << 1;
		digitalWrite(R2, datar2 & 0x80);
		datar2 = datar2 << 1;
		digitalWrite(G2, datag2 & 0x80);
		datag2 = datag2 << 1;
		digitalWrite(CLK, HIGH);//上升沿发送数据
		digitalWrite(CLK, LOW);//恢复CLK低电平，让下一次给CLK高电平时能够产生上升沿
	}
}

```

*我们可以看到，上面的SPI只传输了8位的上下点阵红绿数据，而一行有8块74HC595芯片，所以调用8次这个函数即可实现数据的传输*

```c++
for (int i = 0; i < 8; i++)
		{
			hc595senddata(ShowR1[Rownumber][i], ShowR2[Rownumber][i], ShowG1[Rownumber][i], ShowG2[Rownumber][i]);
		}

```

## 行选列控制实现显示

- 用138行选和595列控制实现在LED点阵上为所欲为

```c++
void display()
{
	for (Rownumber = 0; Rownumber < 16; Rownumber = Rownumber + 1)
	{
		for (int i = 0; i < 8; i++)
		{
			hc595senddata(ShowR1[Rownumber][i], ShowR2[Rownumber][i], ShowG1[Rownumber][i], ShowG2[Rownumber][i]);
		}
		digitalWrite(OE, HIGH);//关闭LED不仅仅是为了省电，还有清空的作用，这会帮助你解决一些在你看来是玄学的问题，比如为什么我没有给某些点供电，但是某些点却以极其不引人注目的微弱之光闪耀着
		hc138sacn(Rownumber);  //选定某一行
		digitalWrite(LE, HIGH);//拉高片选，产生上升沿，使595把刚刚上面的串行输入转成并行输出
		digitalWrite(LE, LOW); //恢复片选的低电平，让下一次再拉高片选的时候，产生上升沿
		digitalWrite(OE, LOW); //打开使能，让LED显示
	}
}

```

## 双屏级联

[双屏级联](https://github.com/zhouzaihang/LED64-32-Screen-ESP32-Raspberry_Pi_3/blob/master/%E5%8F%8C%E5%B1%8F%E7%BA%A7%E8%81%94%E6%8A%80%E6%9C%AF/LED%E5%8F%8C%E5%B1%8F%E7%BA%A7%E8%81%94%E6%8E%A5%E7%BA%BF(%E6%A0%91%E8%8E%93%E6%B4%BE).md)
