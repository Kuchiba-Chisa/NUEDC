# 2023 E 题 运动目标控制与自动追踪系统（STM32F103，2025 年 2–3 月学习）

红色激光云台沿 A4 靶纸黑框走一圈，绿色激光云台自动追踪红点。

> **这个工程以 DuRuofu 的开源项目 `STM32_Target_Tracking_System` 为基础**，版权归原作者，原 README 见 [原仓库README_DuRuofu.md](原仓库README_DuRuofu.md)。
> 我在它上面改了舵机标定、插值算法和 OpenMV 识别脚本，具体见下面的"我的改动"。
> 题目原文和原作者的报告在 `Docs/`。

## 目录

| 目录 | 内容 |
|---|---|
| `RedServo/STM32` | 红色激光云台（Keil 工程 `STM32code.uvprojx`） |
| `RedServo/openmv` | 红色云台视觉。`thisone.py` 是我最后用的版本，`先识别外框.py` 是试验版 |
| `RedServo/K210` | 原作者的 K210 版本 |
| `GreenServo/STM32`、`GreenServo/openmv`、`GreenServo/K210` | 绿色追踪云台，结构同上 |
| `2023-E openmv/find_rects.py` | 识别 A4 靶纸矩形的单独脚本 |
| `识别矩形内外框_循环十次_成功/` | 识别黑框内外两条边、循环 10 次取稳定值的脚本 |
| `1.mp4` | 调试效果录像 |
| `我的改动说明.txt` | 当时记的问题和改进思路 |

红绿两个 STM32 工程都用 Keil 5 + `Keil.STM32F1xx_DFP` 编译过，0 error。

## 硬件

- STM32F103C8T6 最小系统板 ×2（红、绿各一块）
- 二维舵机云台 ×2 + 激光笔
- OpenMV（原作者用 K210），串口 115200
- OLED、蜂鸣器、按键（红色云台 11 个，绿色 1 个）

## 引脚（来自 `STM32code.ioc`，红绿相同的部分）

| 功能 | 引脚 | 外设 |
|---|---|---|
| 舵机 A（水平） | PA8 | TIM1_CH1，72 分频 / 20000 = 50 Hz，CCR 500–2500 对应 0.5–2.5 ms |
| 舵机 B（俯仰） | PA11 | TIM1_CH4 |
| 调试串口 | PA9 TX / PA10 RX | USART1 |
| 视觉串口 | PA2 TX / PA3 RX | USART2 |
| OLED | PB8 SCL / PB9 SDA | 软件 I2C |
| 蜂鸣器 | PB12 | GPIO |
| 按键 | 红：KEY1–11 = PA0, PA1, PA4, PA5, PA6, PA7, PB10, PB11, PB13, PB14, PB15；绿：只有 KEY2 = PA1 | EXTI |

## 红色云台按键（`RedServo/STM32/User/KEY/key.c`）

| 按键 | 作用 |
|---|---|
| KEY1 (PA0) | `Problem_Flag` 在 0→4 之间切换，选题；校准模式下记录当前点 |
| KEY2 (PA1) | 急停 / 继续 |
| KEY3 (PA4) | 进入 / 退出校准模式（`Problem_Flag = 9`） |
| KEY9 (PB13) | 校准模式下记录当前点 |
| KEY5 (PA6) / KEY6 (PA7) / KEY7 (PB10) / KEY11 (PB15) | 校准模式下微调舵机 A/B 的 PWM（在 `System_Calibration()` 里轮询） |
| KEY8 (PB11) | 直接执行 Problem1（回原点） |

## 视觉 → STM32 协议

OpenMV 从 UART3 发 `"a,b,c,d,e,f,g,h\r\n"`，共 8 个整数。STM32 在 `User/USART/usart_2.c` 里用 `sscanf` 解析到 `K210_data[8]`，
再由 `App.c` 的 `Update_Parameters()` 取用：

| 下标 | 含义 |
|---|---|
| 0–3 | 屏幕左 / 上 / 右 / 下边缘对应的舵机 PWM（`left_PWM, up_PWM, right_PWM, down_PWM`） |
| 4–7 | 屏幕左 / 上 / 右 / 下边缘在摄像头画面里的像素坐标 |

STM32 用这两组数建立"像素 → 舵机 PWM"的线性映射。数据存在 OpenMV 上，没存的时候发默认值 `1610,1258,1273,1580,83,2,260,210`。

## 我的改动

- **舵机标定**：中位改成 `pwm_A = 1503, pwm_B = 1471`，行程限幅改成 700–2300（原来是 500–2500 / 800–2200），按自己的云台重新测的
- **插值**：`Yuntaiz_AB_Move_2` 取 A、B 两轴里差值大的那个作为步数（再 ×3），两轴按比例同步走。原来的插值把矩形当成正方形处理，走斜边会跑偏
- **PID**：`Yuntai_PID` 改成增量式，`Kp = −0.3`；低通滤波和死区试过，现在是注释掉的状态
- **OpenMV**：重写了红色激光和 A4 黑框的识别（`thisone.py`），加了一组红色激光阈值，胶带上的红点也能勉强认出来

## 已知问题

- 沿框走的时候还是会跑出矩形，速度也慢（`我的改动说明.txt` 里记了几个改进方向：加边界限幅、减少插值点、输出加平滑）
- `Yuntai_PID` 里保存历史误差用的是 `L_x_err = filtered_x_err`，但滤波那段已经注释掉了，`filtered_x_err` 一直是 0，所以 D 项算出来是错的。
  要用不带滤波的版本，应该改成保存 `x_err`；另外 `L_L_x_err` 要在 `L_x_err` 更新**之前**赋值
