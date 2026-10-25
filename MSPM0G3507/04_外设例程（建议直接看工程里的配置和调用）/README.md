# MSPM0 外设例程（2025 年 4 月刚上手 TI 板时写的）

都是 CCS 20.1 + `mspm0_sdk_2_04_00_06` 的工程，用 Import Project 选目录导入。

| 目录 | 芯片 | 内容 | 编译 |
|---|---|---|---|
| `OLED_编码器_demo` | G3507 | OLED（软件 I2C）+ 两路编码器测速。主循环每 0.5 s 调一次 `ReadSpeed()`，自带 `printf` 重定向（`User/printf.c`） | 通过 |
| `按键_陀螺仪_电机` | **MSPM0L1306**（不是 G3507，工程是从 L1306 的 empty 模板建的） | PWM 呼吸灯（TIMG1，PA0/PA1）、第二路 PWM（TIMG0，PA12/PA13）、4 个按键（PA18/PA2/PA3/PA4）、UART0（PA22 RX / PA17 TX）。`User/` 里有 JY61P、按键、PWM 驱动，但 `main` 只跑了呼吸灯 | 通过 |
| `感为灰度_IMU` | G3507 | 感为灰度官方例程改的：通过 I2C 读 8 路数字量、模拟量和归一化值，用串口打印。另外配了无 MCU 版的地址线（PB0/PB1/PB2）和 ADC，以及串行版的 DAT/CLK（PB8/PB9） | **不通过**：`User/Imu.c` 用到的 `Imu` UART 和 `DMA_CH0` 在 `.syscfg` 里没配，原始备份就是这样。不需要 IMU 的话，把 `Imu.c` 从编译里排除就能过 |

> 原来的 `感为灰度_IMU/User/Driver/` 是从 2024-H 开源工程拷过来的 OLED/电机驱动，引用了不存在的 `empty.h`，整理时删掉了。
> 那个目录下还套着一份约 400 MB 的杂物（360 驱动下载、Keil C51 安装包），也一起删了。

正式比赛用的 MSPM0 底盘驱动（编码器、PWM、JY61P DMA 接收、I2C 灰度、OLED）比这些例程更完整，
直接看 [01_2025E 小车](../01_2025E_简易自行瞄准装置_小车) 的 `User/` 就行。
