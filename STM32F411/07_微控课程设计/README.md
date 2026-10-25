# 微机控制课程设计（非电赛，2025 年 4–5 月）

两个工程都能用 Keil 编译（armcc 逐文件编译检查 0 error）。

## wkkk_TCS+VL6180+ASR+L610

Nucleo-F411RE（STM32F411RET6）。上位机通过 USART1 下命令，MCU 读颜色、距离和语音传感器，结果发回上位机；
另外通过 USART6 给另一块 F4 发命令，由那块板子去驱动 FPGA / 舵机。

| 模块 | 接口 |
|---|---|
| TCS34725 颜色 | I2C1：PB6 SCL / PB7 SDA，地址 0x29 |
| VL6180X 测距 | I2C2：PB10 SCL / PB9 SDA，地址 0x29 |
| 语音识别 ASR | 软件 I2C：PB1 SCL / PB12 SDA，写地址 0xF2；词条：ni hao / kai shi / kai deng / fa guang / xi dian |
| L610 4G 模组 | USART2（PA2/PA3）。连华为云 IoTDA、打电话的 AT 指令写在 `main.c` 里，已经注释掉 |
| 上位机 | USART1：PA9 TX / PA10 RX，115200 |
| 另一块 F4 | USART6：PC6 TX / PC7 RX |
| LED | PA5 |

上位机命令（USART1 单字符）：

| 字符 | 模式 |
|---|---|
| `a` | 测试：读颜色，回传 B 通道值 |
| `b` | 测试：读距离，回传 mm |
| `c` | 测试：给 USART6 发 `'1'`（驱动舵机），回传 `ok` |
| `e` | 正常运行：测距 > 30 就给 USART6 发 `'1'` 并回传 `jiashui`；B 通道 > 900（识别到蓝色异物）就发 `'2'` 并回传 `jinggao`；语音识别到词条就开灯 |
| `d` | 停止 |

> 开源副本里华为云的设备 ID、密码和手机号已经换成了 `<device_id>` 这类占位符，用的时候填自己的。

## 411Uart_final2_K210数字识别投票

STM32F411CEU6。按下 PA0 后，在 12 s 内不停给 K210 发 `'0'` 请求识别结果。K210 回 `'1'`–`'9'`，每个数字计一票，
OLED 实时显示票数最多和第二多的数字。2021F 送药小车里识别病房号用的就是这套逻辑。

| 功能 | 接口 |
|---|---|
| K210 | USART2：PA2 TX / PA3 RX |
| 调试 | USART1：PA9 TX / PA10 RX |
| 备用 | USART6：PA11 TX / PA12 RX |
| OLED | PA4 SCL / PA5 SDA（软件 I2C） |
| 启动按键 | PA0，低电平有效 |
