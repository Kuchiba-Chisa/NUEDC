import sensor, time, image
import utime
from machine import UART
import pyb

# 设定LED引脚（OpenMV板上LED控制）
led_r = pyb.LED(1)  # 红色LED
led_g = pyb.LED(2)  # 绿色LED
led_b = pyb.LED(3)  # 蓝色LED

# 设置UART
uart = UART(3, 115200)  # UART2通信，波特率115200

# 初始化传感器
sensor.reset()
sensor.set_pixformat(sensor.RGB565)  # 设置颜色格式
sensor.set_framesize(sensor.QVGA)  # 设置分辨率
sensor.set_auto_gain(False)  # 禁用自动增益并设置增益
sensor.set_auto_whitebal(False)  # 禁用自动白平衡
sensor.set_vflip(True)  # 设置垂直翻转
sensor.set_hmirror(False)  # 不水平翻转
sensor.skip_frames(time=900)  # 等待图像传感器稳定

# LCD显示初始化

flag = 0

# LED状态切换函数
def led(num):
    if num == 0:
        led_r.off()
        led_g.off()
        led_b.off()
    elif num == 1:
        led_r.off()
        led_g.on()
        led_b.off()
    elif num == 2:
        led_r.off()
        led_g.off()
        led_b.on()

# 串口接收数据处理
def accept_uart():
    global flag, uart
    if uart.any():  # 判断是否有数据可读
        led(1)
        read_data = uart.read()  # 读取数据
        if read_data:
            try:
                data_str = read_data.decode("utf-8")  # 将字节数据转换为字符串
                segments = data_str.split(",")  # 按逗号分割字符串
                result = {}  # 创建字典存储解析结果
                for segment in segments:  # 遍历每个键值对并解析
                    key, value = segment.split(":")  # 按冒号分隔键值对
                    result[key.strip()] = value.strip()  # 去除空格并保存到字典中
                print(result)  # 打印解析结果
                led(2)
                flag = int(result['id'])
                print('收到', flag)
                return 1
            except:
                pass
    return 0

# 识别激光确定位置
def pointPosition():
    global sensor, uart
    flag0 = 0
    while flag0 == 0:
        print('识别激光确定位置中')
        flag0 = accept_uart()
        img = sensor.snapshot()  # 获取图片
        red_x = 0
        red_y = 0
        green_x = 0
        green_y = 0

        # 寻找红色激光
        for blobs in img.find_blobs([(50, 100, 10, 127, -20, 127)], pixels_threshold=10, area_threshold=10, merge=True):
            red_x = blobs.cx()
            red_y = blobs.cy()
            img.draw_cross(blobs.cx(), blobs.cy(), color=(0, 0, 255), size=8)

        # 寻找绿色激光
        for blobs in img.find_blobs([(30, 90, -50, -10, 0, 50)], pixels_threshold=10, area_threshold=10, merge=True):
            green_x = blobs.cx()
            green_y = blobs.cy()
            img.draw_cross(blobs.cx(), blobs.cy(), color=(255, 0, 0), size=8)

        utime.sleep_ms(20)
        if red_x * red_y * green_x * green_y != 0:
            str2 = str(red_x) + ',' + str(red_y) + ',' + str(green_x) + ',' + str(green_y) + ',0,0,0,0\r\n'
            utime.sleep_ms(50)
            uart.write(str2)  # 发送数据
            print(str2 + ' 发送完毕')
def Fing_Write():
    global sensor
    flag0 = 0
    while flag0 == 0:
        img = sensor.snapshot()
        for blobs in img.find_blobs([(60, 100, -17, 127, -6, 127)], pixels_threshold=15000, area_threshold=15000, merge=True):
            if abs(blobs.w() - blobs.h()) > 50:
                continue
            img.draw_rectangle(blobs[0:4])
            str2 = str(blobs.cx()) + ',' + str(blobs.cy()) + ',0,0,0,0,0,0\r\n'
            uart.write(str2)  # 发送数据
            print(str2 + ' 发送完毕')
            img.draw_cross(blobs.cx(), blobs.cy(), color=255, size=8)
            flag0 = 1


while True:
    img = sensor.snapshot()  # 获取图片
    flag = 2

    if flag == 1:
        flag = 0
        Fing_Write()
    if flag == 2:
        flag = 0
        pointPosition()
