## STM32F103C8T6 Base
使用 STM32CubeMX 生成 Makefile 开发项目， 使用 VS Code 开发。

- `Core` 生成代码目录，可以改动
- `Drivers` 生成代码目录，不改动
- `User` 用户代码目录
- `.vscode` VS Code 配置
- `openocd.cfg` OpenOCD 配置


## First Application
#### 功能说明
- 按键(K0) 控制 LED(D2) 点亮/熄灭
- UART 通信 (使用 UART1/UART2)

#### UART2
```
STM32F103C8T6               FT232H (UART)
-------------------------------------------------
12  PA2   USART2_TX         
13  PA3   USART2_RX         
30  PA9   USART1_TX         AD0 UART_RXD
31  PA10  USART1_RX         AD1 UART_TXD  	 
```

## PINs configuration

```
STM32F103C8T6                      Function         Connect
-----------------------------------------------------------------------
12  PA2   USART2_TX                USART2_TX        AD0 (FT232H)
13  PA3   USART2_RX                USART2_RX        AD1 (FT232H)
14  PA4   GPIO_Output SPI1_CS      SPI1_CS          CS        (BME280)
15  PA5   SPI1_SCK                 SPI1_SCK         SCL/SCK   (BME280)
16  PA6   SPI1_MISO                SPI1_MISO        ADDR/MISO (BME280)
17  PA7   SPI1_MOSI                SPI1_MOSI        SDA/MOSI  (BME280)
19  PB1   GPIO_Output OLED_RES     (OLED_RES)       RES (OLED)
21  PB10  I2C2_SCL                 I2C2_SCL
22  PB11  I2C2_SDA                 I2C2_SDA
25  PB12  GPIO_Output SPI2_CS      SPI2_CS          CS  (OLED)
26  PB13  SPI2_SCK                 SPI2_SCK         D0  (OLED)
27  PB14  GPIO_Output OLED_DC      OLED_DC          DC  (OLED)
28  PB15  SPI2_MOSI                SPI2_MOSI        D1  (OLED)
30  PA9   USART1_TX                USART1_TX        AD0 (FT232H)
31  PA10  USART1_RX                USART1_RX        AD1 (FT232H)
42  PB6   I2C1_SCL                 I2C1_SCL         SCL/SCK  (BME280)
43  PB7   I2C1_SDA                 I2C1_SDA         SDA/MOSI (BME280)
```


FT232H (UART#1 / UART#2)
```
STM32F103C8T6                      Function         Connect (FT232H)
-----------------------------------------------------------------------
12  PA2   USART2_TX                USART2_TX        AD0 (FT232H)
13  PA3   USART2_RX                USART2_RX        AD1 (FT232H)
30  PA9   USART1_TX                USART1_TX        AD0 (FT232H)
31  PA10  USART1_RX                USART1_RX        AD1 (FT232H)
```

BME280 (I2C#1)
```
STM32F103C8T6                      Function         Connect (BME280)
-----------------------------------------------------------------------
42  PB6   I2C1_SCL                 I2C1_SCL         SCL/SCK  (BME280)
43  PB7   I2C1_SDA                 I2C1_SDA         SDA/MOSI (BME280)
```

BME280 (SPI#1)
```
STM32F103C8T6                      Function         Connect (BME280)
-----------------------------------------------------------------------
14  PA4   GPIO_Output SPI1_CS      SPI1_CS          CS        (BME280)
15  PA5   SPI1_SCK                 SPI1_SCK         SCL/SCK   (BME280)
16  PA6   SPI1_MISO                SPI1_MISO        ADDR/MISO (BME280)
17  PA7   SPI1_MOSI                SPI1_MOSI        SDA/MOSI  (BME280)
```

OLED (SPI#2)
```
STM32F103C8T6                      Function         Connect (OLED)
-----------------------------------------------------------------------
19  PB1   GPIO_Output OLED_RES     (OLED_RES)       RES (OLED)
25  PB12  GPIO_Output SPI2_CS      SPI2_CS          CS  (OLED)
26  PB13  SPI2_SCK                 SPI2_SCK         D0  (OLED)
27  PB14  GPIO_Output OLED_DC      OLED_DC          DC  (OLED)
28  PB15  SPI2_MOSI                SPI2_MOSI        D1  (OLED)
```



## STM32F103C8T6 BME280 SPI
使用 STM32CubeMX 生成 MDK-ARM 开发项目， 使用 uVision MDK-ARM 开发。

### 项目说明

- `Inc`, `Src` 生成代码目录，可以改动
- `Drivers` 生成代码目录，不改动
- `MDK-ARM` 生成 MDK-ARM 目录, 不手工改动
- `User` 用户代码目录

**功能说明**

- UART 通信 (使用 UART1)
- BME280 (SPI) 测量值通过 UART1 发送

**UART**

```
STM32F103C8T6               FT232H (UART#2)
-------------------------------------------------
30  PA9  USART1_TX           AD0  UART_RXD
31  PA10 USART1_RX           AD1  UART_TXD
```

**BME280 (SPI)**

```
STM32F103C8T6               BME280 (SPI#1)               连接线
------------------------------------------------------------------       
14  PA4 GPIO_Output         SPI1_CS   (CS)               绿
15  PA5 SPI1_SCK            SPI1_SCK  (SCL/SCK)          黄
16  PA6 SPI1_MISO           SPI1_MISO (ADDR/MISO)        橙
17  PA7 SPI1_MOSI           SPI1_MOSI (SDA/MOSI)         蓝
```

**Tip**

GNU Arm Compiler `printf` 输出浮点数，需要在连接选项添加 `-u _printf_float`
```
LDFLAGS = $(LDFLAGS) -u _printf_float
```



### BME280 SPI

#### SPI基础知识

SPI 接口一般使用 **4 条线**通信： 

- **MISO** 主设备数据输入，从设备数据输出；
- **MOSI** 主设备数据输出，从设备数据输入；
- **SCLK** (**SCK**) 时钟信号，由主设备控制发出，MISO和MOSI上的数据在SCK信号的跳变沿发送变化；
- **CS** (**NSS**/**CS**) 从设备片选信号，由主设备控制；当CS为低电平时，表示选中从设备，主设备可以与该从设备通信 

当SPI总线上只有一个从设备时，可以把从机的片选直接接地，也即一直选中：

当SPI总线上有多个从器件时，需要给每个从设备分配一个CS信号，以保证可以和每个从设备独立通信，而其他三根线可以共用。

#### SPI 总线四种工作方式

*SPI 主模块和与之通信的外设备时钟相位和极性 应该一致。*

SPI的时钟和数据的关系可以有四种模式：

- 时钟极性（CPOL）定义了时钟空闲状态电平：

  - CPOL=0：空闲时，时钟为低电平；

  - CPOL=1：空闲时，时钟为高电平；


- 时钟相位（CPHA）定义数据的采集时间：

  - CPHA=0：在时钟的第一个跳变沿（上升沿或下降沿）进行数据采样；


  - CPHA=1：在时钟的第二个跳变沿（上升沿或下降沿）进行数据采样；

使用前我们需要依据从设备的要求，把主设备设置为相同的模式。

则我们使用STM32的SPI接口和BMP280通信时，需要设置为CPOL=1空闲时，时钟为高电平、CPHA=1在时钟的第二个跳变沿进行数据采样。



#### STM32的SPI总线使用

STM32带了硬件SPI，下面我们就使用stm32的硬件SPI读取气压计BMP280的数据。

Mode：选择全双工主设备（因为我们的STM32是主控，BMP280是从设备；全双工就是我们前面讲的4线制SPI，收、发各用一条线；也用收发共用一条线的，那么就是半双工，3线制SPI）；

Prescaler：设置分频系数，即SPI总线的时钟频率（BMP280手册中要求不大于10M）；

CPOL和CPHA：前面有详细说过，不罗嗦了；

NSS：CS片选这里使用软件方式，主要是软件方式可以适应挂载多个从设备，以后移植起来方便很多；片选设置为软件方式就是用软件控制一个IO口作为CS信号，所以还要设置一个输出的GPIO，图中设置的是PA4。

设置完成后，芯片引脚的PA4、5、6、7被占用为SPI的几个信号线。

在硬件连接上，我们把BMP280也按四线制SPI方式连接

- CSB连接到PA4
- SCK连接到PA5
- SDO连接到PA6
- SDI连接到PA7

生成工程代码，在keil中打开。STM32的HAL库已经将SPI的初始化、SPI的读写操作封装成了函数，我们直接调用即可。

首先在main主循环之前，增加如下代码，拉高PA4（即拉高片选CS，初始状态不选中）：

```c
/* USER CODE BEGIN 2 */
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
```

之后添加如下代码，读取BMP280的芯片id并打印：

```c
tx_data[0] = 0xD0;
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 2, 0x10);
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);		
Uart1_printf("spi: 0x%x\r\n",rx_data[1]);
HAL_Delay(100);
```

操作过程是先拉低片选CS线，再访问从设备，访问完毕后拉高CS线。

主要讲讲访问从设备的HAL_SPI_TransmitReceive这个函数，用于读取从设备的的数据。

该函数的第二个参数是要发送的数据首地址，第三个参数是接收数据存放的首地址，第四个参数是要传输的数据字节数，最后是超时。

我们结合BMP280读取数据的方法来看：

- 按照数据手册，读取数据时，先发送寄存器地址（一个字节），然后之后从机就会回传数据（多个字节）。
- 这里要注意一下，因为4线制的SPI总线，本质上是主设备和从设备在数据交换。
- BMP280的芯片id存放在其寄存器地址0xD0处，所以把0xD0存到tx_data的第一个字节处，后面的我们不关心，因为只有第一个字节是起作用的。
- HAL_SPI_TransmitReceive执行完之后，从机发来的数据存放在rx_data中，由于从机返回的数据是从第二个字节开始有效的，所以从机发来的数据是从rx_data[1]开始的。
- 我们“交换”了两个字节数据，从机返回的一个字节数据在rx_data[1]中。

程序运行结果如下，可以看到成功读取到了BMP280的id值0x58：

能够通过SPI成功读取BMP280芯片内的数据，我们就已经成功了一大半。之后的操作只需要启动转换、读取数值计算结果，就可以获取气压值了。

启动转换的过程如下，即是往0x74寄存器地址写入0xff，操作过程如下：

（注意这一步是必须的，否则回读的气压数据会一直不变化。）

```c
tx_data[0] = 0x74;
tx_data[1] = 0xff;
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 2, 0x10);
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);	

/* USER CODE END 2 */
```

然后就是读取数值、计算气压结果了；这里也需要几步，先要从芯片内读取已经存好的校准参数，然后读取芯片内的AD转换值，最后依据手册中给出的公式计算出温度、气压值结果。

读取校准参数和AD转换值的过程也是用HAL_SPI_TransmitReceive这个函数，限于篇幅就不贴代码了，可以到文末获取完整工程代码。

实测气压的分辨率很高，上下移动电路板10cm就能出现比较稳定的气压变化，实际中很多情况下用来测量相对高度的变化。另外说一下BMP280同系列的BME280，还具有测量湿度的功能。
