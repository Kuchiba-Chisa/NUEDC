# 校赛 D2 题：两轮自平衡循线小车（MSPM0G3507，2025 年 5 月）

提交包名为"D2-通院15-刘厚麟"。`作品资料/` 里有报告 PDF、原理图、作品照片，以及"要求 1–5"的演示视频，题面以报告为准。

## 版本

| 目录 | 日期 | 说明 |
|---|---|---|
| `final_校赛提交版_05-04` | 05-04 | **校赛提交的版本**，CCS 20.1 编译通过 |
| `history_07-15_改过的版本` | 07-15 | 7 月拿来改别的东西时动了 `empty.syscfg`，把灰度引脚删了，`gw/gw_sens.c` 因此**编译不过**。只作参考 |
| `777_编码器测速测试` | 05-03 | 同一套框架拆出来的编码器测速测试 |

## 硬件

- MSPM0G3507 LaunchPad
- 两轮平衡底盘：编码器电机 ×2 + 双路驱动
- JY61P 陀螺仪（取 Roll 做平衡，Yaw 做转向）
- MaixCam / K230：识别黑线中心点，串口 + DMA 发 2 字节坐标
- 蓝牙串口（BLE）、OLED（硬件 I2C）、LED、蜂鸣器、按键 ×4

## 引脚（来自 `empty.syscfg`）

| 功能 | 引脚 |
|---|---|
| 电机 PWM | TIMA0：PA8（CC0，SysConfig 自动分配）、PA9、PB20、PB24 |
| 编码器 L1 | PA15 / PA16 |
| 编码器 L2 | PB8 / PB9 |
| JY61P（UART "TLY"） | PA25 RX / PA26 TX |
| 视觉（UART "MAIX"） | PA22 RX / PA21 TX |
| 蓝牙（UART "BLE"） | PB7 RX / PB6 TX |
| OLED（I2C） | PB2 SCL / PB3 SDA |
| LED 红 / 橙 / 绿 | PB18 / PA7 / PA2 |
| 蜂鸣器 | PB19 |
| 按键 | PA12、PA13、PA14、PA23 |

## 按键（`Hardware/encoder.c` 的 `GROUP1_IRQHandler`）

| 按键 | 功能 |
|---|---|
| PA14 | `mode` 在 0→1→2→3 之间循环（选要求几） |
| PA12 | 启动 / 停止：同时切换 `stand_flag`（是否开平衡）和 `stop_flag`（是否开始跑） |
| PA23 | 陀螺仪 Yaw 校零 |

## 控制结构（`Hardware/control.c`，10 ms 一次）

```
Motor_L = Balance + Velocity + Chasu + Turn
Motor_R = Balance + Velocity − Chasu − Turn
```

| 环 | 输入 | 参数（×100） |
|---|---|---|
| `Balance` 直立 PD | Roll、Gyro_x | `Kp=27000, Kd=2600`，机械中值 `Middle_angle` |
| `Velocity` 速度 PI | 两轮编码器均值 | `Kp=4300, Ki=21`，目标 30（mode 3 是 50） |
| `Turn` 转向 PID | Yaw 与目标的差 | `Kp=3000, Kd=100, Ki=10` |
| `Chasu_PID` 差速 | 视觉线中心分成 9 段，偏差 −7…+7 | — |

`State()` 用 `state_num` 记经过了几次"线 → 无线"的切换，`mode 1/2/3` 在不同的 `state_num` 上切到 IMU 定向走直线。
从 `state_num % 4` 的判断看，路线应该是"有线段 + 无线直行段"交替绕圈（推测，具体路线以报告为准）。

## 调平衡的顺序

1. 只开 `Balance`：先确定 Kp 的正负（车倒向哪边，轮子就要往哪边转），再加大到出现低频大幅振荡，然后加 Kd 压住。
2. 加 `Velocity`：小车会慢慢往一个方向跑，调 `Middle_angle` 抵消；Ki 一般取 Kp/200 左右。
3. 最后加 `Turn` 和视觉差速。
