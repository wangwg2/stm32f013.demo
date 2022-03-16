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
