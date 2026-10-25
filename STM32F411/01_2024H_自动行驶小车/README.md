# 2024 H 题 自动行驶小车（STM32F411，2025 年 3–5 月赛前练习）

依据：四个任务分别是 A→B 直线、B→C 半圆循迹、走对角线、绕 4 圈，转角 39° / 143°。这些和 2024 H 题的场地（两个半圆弧 + ABCD 四个点）一致。
工程名叫 `clean`，后面的送药小车、测量题也是在这套底盘代码上改出来的。

## 版本

| 目录 | 日期 | 说明 |
|---|---|---|
| `final_4.21` | 04-21 | **最终版**，四个任务都有 |
| `history/4.03_灰度编码器初版` | 04-04 | 刚接上感为灰度和编码器。`User/APP/app.c` 里的 Problem1–4 是从 2023 E 题开源代码（DuRuofu）搬来的框架，没实际用 |
| `history/4.16_陀螺仪走直线` | 04-16 | 加 JY61P，用航向角 PID 走直线 |
| `history/4.20_灰度串级PID` | 04-20 | 灰度位置环做外环、角度环做内环的尝试 |

## 硬件

- STM32F411CEU6
- 编码器电机 ×2 + 双路驱动
- 感为 8 路灰度，I2C 版（软件 I2C）
- JY61P 陀螺仪
- OLED、按键 ×5

## 引脚（来自 `clean.ioc`）

| 功能 | 引脚 | 外设 |
|---|---|---|
| 电机 PWM | PA8, PA9, PA10, PA11 | TIM1_CH1–4 |
| 编码器 | PB6/PB7、PA0/PA1 | TIM4、TIM5 |
| JY61P | PA2 TX / PA3 RX | USART2 |
| 调试 / 视觉 | PA15 TX / PB3 RX | USART1 |
| 灰度 | PB8 SCL / PB9 SDA | 软件 I2C |
| OLED | PA4 SCL / PA5 SDA | 软件 I2C |
| 按键 KEY0–4 | PA12, PA6, PA7, PB0, PB1 | EXTI |

> 送药小车把 PWM 挪到了 TIM3（PA6/PA7/PB0/PB1），按键也跟着换了引脚，两套代码不能混用。

## 任务（`Core/Src/main.c`，KEY1–4 选 Flag 1–4）

| Flag | 任务 | 实现 |
|---|---|---|
| 1 | 基础循迹 | 灰度偏差 + 航向角双 PID，有丢线后原地找线的逻辑 |
| 2 | A→B→C→D→A | 用 IMU 锁 0° 直行，直到灰度检测到线 → 循迹走半圆 → 全白后停 → 用 IMU 锁 183° 直行 → 再循迹半圆 |
| 3 | 走对角线 | 同 2，直行角度换成 39° 和 143.5° |
| 4 | 按 3 的路线绕 4 圈 | `count < 4` 循环 |

关键函数在 `MDK-ARM/pid.c`：
- `CalculateOffset(BIT)`：8 路灰度加权得到偏差
- `ComputePID(offset)`：灰度位置 PID
- `CalculateOffset_IMU(target)`：`Yaw − target`。**没有处理 ±180° 跳变**，目标角接近 180°（比如任务 2 的 183°）时要注意，
  正确做法是把差值折回 (−180°, 180°]
- `angle_PID_Controller(err, dt)`：航向角 PID
- `AdjustMotorSpeed(out)`：`BASE_SPEED ± out` 差速

## 调参

- 39° / 143.5° / 183° 是在自己的场地上实测出来的，场地摆放不一样就要重新测。
- JY61P 上电时 Yaw 不一定是 0，`IMU_Init()` 里发了 `FF AA 76 00 00`（Z 轴角度归零）。
- `ComputePID` 的积分项没有限幅，长时间偏线会积分饱和。
