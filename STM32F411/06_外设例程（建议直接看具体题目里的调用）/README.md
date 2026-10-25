# STM32 外设例程

单个模块的驱动测试。"Nucleo" 指 Nucleo-F411RE 开发板（STM32F411RET6，板载 ST-Link 虚拟串口接在 USART2），其余都是 F411CEU6 核心板。
引脚都是从 `.ioc` 里读出来的。

| 目录 | 板子 | 模块 | 接线 | 现象 |
|---|---|---|---|---|
| `GPIO按键LED_Nucleo` | Nucleo | 板载按键 / LD2 | PC13 按键，PA5 LED | 按住蓝色按键 LD2 亮 |
| `UART轮询收发_Nucleo` | Nucleo | 板载虚拟串口 | USART2 PA2/PA3 | `HAL_UART_Receive` 轮询收字符：收到 `R`/`B`/`S` 分别回复 reset / begin / stop |
| `UART中断收发_Nucleo` | Nucleo | 同上 | USART2 | 中断收发；里面的 `MDK-ARM_POLLING` 是同一套代码的轮询版 Keil 工程 |
| `串口Debug` | F411CE | USB-TTL | USART1 PA15 TX / PB3 RX | 循环打印 `state:UARTOK`。`DEBUG_info(tag, fmt, ...)` 这个打印宏后面的工程都在用 |
| `JY61P陀螺仪_OLED` | F411CE | 维特 JY61P、OLED | JY61P→USART2 PA2/PA3（DMA+空闲中断）；OLED PA4 SCL / PA5 SDA | OLED 显示 Yaw |
| `VL6180X测距` | Nucleo | VL6180X ToF 测距 | I2C2：PB10 SCL / PB9 SDA，地址 0x29 | 每 3 s 串口打印 `Range: xx mm` |
| `TCS34725颜色` | Nucleo | TCS34725 颜色传感器 | I2C1：PB6 SCL / PB7 SDA，地址 0x29 | 每 1 s 读 C/R/G/B 原始值，从串口发出 |
| `语音识别ASR` | Nucleo | I2C 离线语音识别模块（写地址 0xF2） | 软件 I2C：PB1 SCL / PB12 SDA | 上电用 `AsrAddWords` 写入 5 个拼音词条，识别到就打印词条编号 |
| `步进电机_脉冲方向` | F411CE | 42 步进 + 驱动器 | 脉冲 PA0（TIM2_CH1），DIR PA1，EN PA2 | 正转 60 rpm → 反转 → 正转 120 rpm 循环 |
| `编码器电机_JY61P_灰度_OLED` | F411CE | 编码器电机、JY61P、感为灰度（串行版）、OLED | PWM TIM3 PA6/PA7/PB0/PB1；编码器 TIM2 PA15/PB3、TIM4 PB6/PB7；JY61P USART2；灰度 CLK PB10 / DAT PB9；电压采样 PA5 ADC | 速度 PI + 转向 PID 循迹，OLED 显示 Turn/Yaw/左右轮速度。结构和自己写的工程不一样，`control.c` 的参数风格像网上的平衡车/循迹例程，**可能不是原创** |
| `感为八路灰度_F401` | F401 | 感为 8 路灰度（I2C 版） | 软件 I2C：PB12 SCL / PB13 SDA | OLED 用二进制显示 8 路数字量。用的是**标准库**，不是 HAL。缺芯片型号宏，打开工程后在 C/C++ → Define 里加 `STM32F401xx` 才能编译 |

## 步进电机调速原理（`User/Stepper_Motor`）

步进驱动器一个脉冲走一步，转速 = 脉冲频率 × 60 / 每转脉冲数。代码里固定占空比 50%，通过改 TIM 的 ARR 改频率：

```c
uint32_t pwm_freq = speed_rpm * steps_per_rev / 60;          // 每秒脉冲数
uint32_t period   = (SystemCoreClock / 100) / pwm_freq - 1;  // TIM 计数频率 = SystemCoreClock/100（PSC=99）
__HAL_TIM_SET_AUTORELOAD(&htim2, period);
__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, period / 2);
```

`steps_per_rev` 要和驱动器拨码的细分对上。`speed_rpm = 0` 会除零，停机要直接拉低 EN。
