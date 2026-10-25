# 2025 E 题 简易自行瞄准装置 —— 步进云台部分（STM32F411）

> 题目对应是推测：代码里没有写题号。依据是赛期（7.27–8.02）、两轴云台 + 激光 + 视觉找靶心、画圆功能。
> 小车部分用 TI MSPM0G3507 做，见 [MSPM0G3507/01_2025E_简易自行瞄准装置_小车](../../MSPM0G3507/01_2025E_简易自行瞄准装置_小车)。

## 版本

| 目录 | 日期 | 说明 |
|---|---|---|
| `final_存一版_v4` | 08-02 20:16 | **最终版**。USART1 收视觉板单字符命令切换任务，USART6 收靶心坐标 |
| `history/存一版_v6_回调注释版` | 08-02 14:54 | 串口回调被注释掉的版本，任务分支多一个 `problem_flag==6` 复位 |
| `history/7.28_画圆` | 07-29 | 加入 `DrawCircle` / `DrawPrecisionRectangle`，用步进画圆、画矩形 |
| `history/7.27_v1_总线舵机+步进+灰度` | 07-28 | 早期方案：总线舵机云台 + 步进 + 感为无 MCU 灰度。**缺 `No_Mcu_Ganv_Grayscale_Sensor_Config.h`，编译不过**（原始备份里也没有，可以从感为官方例程补） |

## 硬件

- 主控：STM32F411CEU6，HSE 25 MHz
- 云台：两个 42 步进电机 + 脉冲/方向/使能驱动器（X 轴 = 电机 A，Y 轴 = 电机 B）
- 视觉：MaixCam / K230，识别靶心和激光点，串口发坐标
- 激光笔：GPIO 直接控制
- 串口屏（`page.控件.txt="..."` + `0xFF 0xFF 0xFF` 格式，淘晶驰 / USART HMI 一类）：显示坐标
- JY61P 陀螺仪（初始化了，最终版没用到 Yaw）
- 按键 ×5（外部中断）

## 引脚（来自 `7.21.ioc`）

| 功能 | 引脚 | 外设 |
|---|---|---|
| 电机 A 脉冲 | PA6 | TIM3_CH1（PSC 99，ARR 19999，改 ARR 调速） |
| 电机 A 方向 / 使能 | PB12 / PB13 | GPIO（DIR1 / EN1） |
| 电机 B 脉冲 | PB6 | TIM4_CH1 |
| 电机 B 方向 / 使能 | PB4 / PB5 | GPIO（DIR2 / EN2） |
| 激光 | PB9 | GPIO（`light_Pin`，高电平亮） |
| 视觉板命令 / 串口屏 | PA15 TX / PB3 RX | USART1（收任务命令；发串口屏指令） |
| JY61P / 小车通信 | PA2 TX / PA3 RX | USART2（DMA + 空闲中断收 JY61P；收 `'1'` 置 `turn_flag`） |
| 靶心坐标 | PA11 TX / PA12 RX | USART6，9600 |
| 计时 | — | TIM2，100 MHz / 10 / 10000 = 1 kHz，`index_ms` 每 1 ms 加 1，用来控制激光亮多久 |
| 按键 KEY0–4 | PA10, PA9, PA8, PB15, PB14 | EXTI |
| OLED | PA4 / PA5 | 软件 I2C |

## 通信协议

- **USART1 收**：单字符 `'1'`–`'6'`，设置 `problem_flag` 选择任务（`main.c` 里的 `HAL_UART_RxCpltCallback`）
- **USART1 发**：上电发 `'a'`；任务 2/3 开始时发 `'1'`；每收到一帧坐标就转发给串口屏（`feedback.t6.txt="x"` + `0xFF 0xFF 0xFF`）
- **USART6 收**：`"x,y\r\n"`，靶心在画面里的像素坐标 → `target_X / target_Y`（`User/USART/usart_3.c`）
- **USART2 收**：JY61P 数据帧；单字符 `'1'` 置 `turn_flag=1`（任务 5 用）

## 任务逻辑（`Core/Src/main.c`）

| problem_flag | 行为 |
|---|---|
主循环每 100 ms 跑一次。激光上电就是亮的（`light_Pin=1`），下面的"关激光"是到时间后拉低。

| problem_flag | 行为 |
|---|---|
| 1 | 用 PID 把激光对准靶心（`Stepper_Control`），对准后开 TIM2 计时，`index_ms ≥ 1500` 关激光 |
| 2 | 先让 X 轴**逆时针**转（同时 Y 轴慢转）找靶，`target_X==0` 表示视觉还没看到；找到后 `Stepper_Control_UntilReached` 闭环对准，1 s 关激光，2 s 停 PWM |
| 3 | 同 2，X 轴**顺时针**找靶 |
| 4 | 同 1 |
| 5 | `turn_flag==1` 时 X 轴持续转（配合小车转弯）。这里是 `while` 死循环，进了就出不来，要复位 |
| 6 | 复位：停计时、清标志、重新初始化电机 |

## 瞄准控制（`User/Stepper_Motor/Stepper_Motor.c`）

- 误差 = 激光点在画面里的固定坐标 − 靶心坐标。代码里激光点写死成 `(222, 202)`，换了摄像头或者重新装了激光要重新标定。
- X / Y 各一个位置式 PID：`Kp=0.9, Ki=0, Kd=1.7`，输出限幅 ±200，作为步进转速（rpm）。
- 死区 ±2 像素，进了死区就停电机，防止抖。
- `Stepper_Control_UntilReached*`：阻塞到误差进死区才返回，后缀 `_x` / `_y` 是单轴，`_fast` 用另一组更软的 PID。
- `Motor_Move_A/B(dir, rpm)`：脉冲频率 = `rpm × steps_per_rev / 60`（`steps_per_rev = 1000`，和驱动器细分要对上），
  ARR = `SystemCoreClock / 100 / 频率 − 1`，CCR = ARR / 2。`rpm` 传 0 会除零，停机要调 `Stepper_Motor_Stop_A/B`。

## 使用

1. 先测出激光点在摄像头画面里的像素坐标，填到 `Stepper_Control` 的 `222 / 202`。
2. 调 PID：先只开 X 轴、`Kd=0`，从小往大加 `Kp` 到开始振荡，再加 `Kd` 压住超调。
3. 视觉端按上面的协议发数据。不接视觉时，可以用串口助手先给 USART1 发 `1` 进任务 1，再给 USART6（9600）发 `200,150\r\n` 测试。
