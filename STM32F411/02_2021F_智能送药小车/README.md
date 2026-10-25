# 2021 F 题 智能送药小车（STM32F411，2025 年 7 月赛前练习）

依据：OLED 提示 "Take your medical" / "NO Medicine" / "medical taken"，药品检测、去程转弯、回程反向转弯的流程，
注释"基础 1/2/3"，以及双车之间用串口发 `'b'`、`'f'` 通信。这些都和 2021 F 题对得上。

## 版本

| 目录 | 日期 | 说明 |
|---|---|---|
| `car2_7.13_night` | 07-23 | **流程最完整的版本**。上电检测药品（`XL` 引脚）后才能开始任务，四题齐全 |
| `final_7.14_night_shi` | 07-24 | 时间最晚的版本。`app.c` 的四题逻辑和 car2 基本一样（第 4 题多了停车识别），但 `main.c` 被改成了**速度环测试**：药品检测被注释掉，`startflag` 永远是 0，任务 2–4 进不去。要跑完整流程，把 `main.c` 里 `if(HAL_GPIO_ReadPin(GPIOB,XL_Pin)==1)` 那段取消注释 |
| `history/411Uart_K210数字识别` | 05-04 | K210 数字识别：接收 1–9 的投票数，取票数最多和次多的两个数字 |
| `history/7.14_night_yuntai_循迹底盘` | 08-01 | 国赛期间拿这个底盘改成只循迹的版本，给 2025 E 题试方案用 |

## 硬件

- STM32F411CEU6
- 编码器电机 ×2 + 双路驱动（TIM3 四路 PWM）
- 感为 8 路灰度，I2C 版（地址 0x4C，`GW_digital_eight()` 一次读 8 路数字量）
- JY61P（USART2，DMA + 空闲中断）
- K210 / K230 数字识别（USART1 收发）
- 药品检测：光电/微动开关接 `XL`（PB5），有药时为 1
- 红/绿 LED、蜂鸣器、OLED（软件 I2C）、按键 ×5

## 引脚（来自 `clean.ioc`）

| 功能 | 引脚 | 外设 |
|---|---|---|
| 电机 PWM | PA6, PA7, PB0, PB1 | TIM3_CH1–4 |
| 编码器 左 / 右 | PB6/PB7、PA0/PA1 | TIM4、TIM5 编码器模式 |
| 节拍 | — | TIM2，1 ms：读编码器、计时，2 s 后置 `lookflag` |
| JY61P | PA2 TX / PA3 RX | USART2 |
| 视觉 / 另一辆车 | PA15 TX / PB3 RX | USART1 |
| 备用串口 | PA11 TX / PA12 RX | USART6 |
| 灰度 | PB8 SCL / PB9 SDA | 软件 I2C（`User/GW_gpio/GWKJ.c`）。`.ioc` 里这两个脚配成了硬件 I2C1，实际没用 |
| OLED | PA4 SCL / PA5 SDA | 软件 I2C |
| 药品检测 XL | PB5 | GPIO 输入 |
| LED 红 / 绿 | PB12 / PB13 | GPIO，低电平亮 |
| 蜂鸣器 | PB10 | GPIO |
| 激光测距 | PB2 XSHUT，PC14 SCL / PC15 SDA | GPIO 软件 I2C（ceju_scl / ceju_sda） |
| 按键 KEY0–4 | PA10, PA9, PA8, PB15, PB14 | EXTI |

## 按键

| 按键 | Flag | 任务 |
|---|---|---|
| KEY1 | 1 | 基础 1：近端病房（第一个路口按视觉结果左/右转） |
| KEY2 | 2 | 基础 2：中端病房（第一个路口直走，第二个路口按视觉结果转） |
| KEY3 | 3 | 基础 3：远端病房（前两个路口直走，停车摆头识别） |
| KEY4 | 4 | 第 4 题（发挥部分，双车） |
| KEY0 | 0 | 停止 / 灰度校准 |

## 流程（`MDK-ARM/app.c`，以 `problem1` 为例）

1. 灰度循迹：`CalculateOffset` → `ComputePID` → `AdjustMotorSpeed`
2. 8 路里压线 ≤ 2 路判定为路口 → 按视觉结果 `Uart_RxBuffer_2`（`'L'`/`'R'`）转弯，`crossflag` 记走到了第几个路口
3. 8 路全压线 → 到达病房：停车、红灯亮，等 `XL` 变 0（药被取走）
4. 用 JY61P 原地掉头 180°，按去程的反方向转回去（去程左转、回程就右转）
5. 回到药房：绿灯亮，`startflag=0`

## 通信

- 发视觉 / 副车：`'b'` 表示开始，`'f'` 表示停车识别（任务 2/3/4 到识别点时发）
- 收视觉：单字符 `'L'` / `'R'`
- K210 数字识别（history 版）：`"n1,n2,...,n9\r\n"`，9 个数字各自的票数

## 注意

- 源文件编码混用：`main.c`、`key.c` 是 UTF-8，`app.c`、`moto.c`、`pwm.c`、`encoder.c` 是 GBK。Keil 打开 UTF-8 的文件中文会乱码，只影响注释。
- 转弯用的是 `Set_PWM + delay_ms` 开环转，换了电机、电压或地面要重新调延时。
