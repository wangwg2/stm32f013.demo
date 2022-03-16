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


## STM32F103C8T6 BME280
使用 STM32CubeMX 生成 Makefile 开发项目， 使用 VS Code 开发。

- `Core` 生成代码目录，可以改动
- `Drivers` 生成代码目录，不改动
- `User` 用户代码目录

#### 功能说明
- 按键(K0) 控制 LED(D2) 点亮/熄灭
- UART 通信 (使用 UART1/UART2)
- BME280 (SPI/I2C) 测量值通过 UART2 发送, 按键(K) 控制发送开关

#### BME280 (I2C/SPI)

Core/Src/main.c
```c
/** @name BME280 (I2C / SPI) */
#define BME280_I2C_ENABLE 1
// #define BME280_SPI_ENABLE 1
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

#### Tip
GNU Arm Compiler printf 输出浮点数，需要在连接选项添加 `-u _printf_float`
```
LDFLAGS = $(LDFLAGS) -u _printf_float
```
