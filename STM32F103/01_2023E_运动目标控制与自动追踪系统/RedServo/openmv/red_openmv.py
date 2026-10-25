import sensor
import time
import image
import gc
from machine import Pin
from pyb import UART


# 设置LED
led_r = Pin('P0', Pin.OUT)  # 红色LED
led_g = Pin('P1', Pin.OUT)  # 绿色LED
led_b = Pin('P2', Pin.OUT)  # 蓝色LED

flag=0
# UART初始化
uart = UART(3, 115200, timeout_char=1000)

# 摄像头初始化
sensor.reset()
sensor.set_pixformat(sensor.RGB565)  # 设置图片格式
sensor.set_framesize(sensor.QVGA)  # 设置图片分辨率
sensor.set_auto_gain(False)  # 关闭自动增益
sensor.set_auto_whitebal(False)  # 关闭自动白平衡
sensor.set_vflip(True)  # 反转图像
sensor.set_hmirror(False)  # 不水平翻转
sensor.skip_frames(time=900)  # 跳过一些帧来确保相机稳定

# LCD初始化

# 定义要写入的内容
origin_str = "left_PWM:1500,up_PWM:1500,right_PWM:1500,down_PWM:1500,K210_left:83,K210_up:2,K210_right:260,K210_down:210"

# 打开文件并写入内容
try:
    with open("/flash/test.txt", "w") as f:
        f.write(origin_str)
    print("文件创建成功，内容已写入。")
except Exception as e:
    print("文件写入失败:", e)

# 读取文件以验证内容
try:
    with open("/flash/test.txt", "r") as f:
        read_data = f.read()
        print("读取的文件内容:", read_data)
        if read_data == origin_str:
            print("验证成功：文件内容与写入内容一致。")
        else:
            print("验证失败：文件内容与写入内容不一致。")
except Exception as e:
    print("文件读取失败:", e)

#检测最小矩形
def find_min_rectangle(rects):
    """
    检测最小矩形，返回最小矩形的相关信息。

    参数:
    rects: 一个包含多个矩形对象的列表

    返回:
    min_rect: 面积最小的矩形对象
    """
    if not rects:
        return None  # 如果没有矩形检测到，返回None

    min_area = float('inf')  # 初始化最小面积为正无穷
    min_rect = None  # 初始化最小矩形

    for rect in rects:
        # 计算当前矩形的面积
        area = rect.w() * rect.h()

        # 如果当前矩形的面积比最小面积小，更新最小矩形
        if area < min_area:
            min_area = area
            min_rect = rect

    return min_rect

#检测最大矩形
def find_max_rectangle(rects):
    """
    检测最大矩形，返回最大矩形的相关信息。

    参数:
    rects: 一个包含多个矩形对象的列表

    返回:
    max_rect: 面积最小的矩形对象
    """
    if not rects:
        return None  # 如果没有矩形检测到，返回None

    max_area = float('inf')  # 初始化最小面积为正无穷
    max_rect = None  # 初始化最小矩形

    for rect in rects:
        # 计算当前矩形的面积
        area = rect.w() * rect.h()

        # 如果当前矩形的面积比最小面积小，更新最小矩形
        if area > max_area:
            max_area = area
            max_rect = rect

    return max_rect

# 发送初始化数据
def sendSaveData():
    print('初始化')
    try:
        with open("/flash/test.txt", "r") as f:
            data_str = f.read()
            print('文件内容:', data_str)
            if not data_str:
                print('文件内容为空')

            segments = data_str.split(",")  # 按逗号分割字符串
            if not segments:
                 print('分割后的数据为空')

            result = {}  # 创建字典存储解析结果
            for segment in segments:
                if ':' not in segment:
                    print(f'无效的键值对: {segment}')

                key, value = segment.split(":")  # 按冒号分隔键值对
                result[key.strip()] = value.strip()  # 去除空格并保存到字典中

            send_str = result['left_PWM'] + ',' + result['up_PWM'] + ',' + result['right_PWM'] + ',' + \
                       result['down_PWM'] + ',' + result['K210_left'] + ',' + result['K210_up'] + ',' + \
                       result['K210_right'] + ',' + result['K210_down'] + '\r\n'
            uart.write(send_str)  # 发送数据
            print('发送初始化数据: ' + send_str)
    except:
        print('文件读取错误，发送默认数据')
        uart.write('1610,1258,1273,1580,83,2,260,210\r\n')  # 发送默认数据


# 串口接收
def accept_uart():
    global flag
    global led_r, led_g, led_b
    while True:
        img = sensor.snapshot()  # 获取图片

        if uart.any():  # 判断是否有数据可读
            led_g.value(1)
            read_data = uart.read()  # 读取数据
            print("接收到的数据:", read_data)  # 打印接收到的原始数据
            if read_data:
                try:
                    data_str = read_data.decode("utf-8")  # 将字节数据转换为字符串
                    print("解码后的数据:", data_str)  # 打印解码后的数据
                    segments = data_str.split(",")  # 按逗号分割字符串
                    result = {}  # 创建字典存储解析结果
                    for segment in segments:  # 遍历每个键值对并解析
                        key, value = segment.split(":")  # 按冒号分隔键值对
                        result[key.strip()] = value.strip()  # 去除空格并保存到字典中
                    print('解析结果并处理完成')
                    print(result)  # 打印解析结果
                    led_b.value(1)
                    flag = int(result['id'])
                    print('收到flag:', flag)

                    if flag == 1:
                        sendSaveData()  # 发送初始化数据
                    elif flag == 5:
                        SaveData(data_str)  # 保存数据

                    break
                except Exception as e:
                    print(f"接收数据失败: {e}")


def SaveData(data_str):
    try:
        with open("/flash/test.txt", "w") as f:
            f.write(data_str)
            f.flush()  # 确保数据写入文件
        print('Save OK')
    except Exception as e:
        print(f"保存文件失败: {e}")

    try:
        with open("/flash/test.txt", "r") as f:
            read_data = f.read()
            print("SaveData保存的文件内容:", read_data)
    except Exception as e:
        print(f"读取文件失败: {e}")

def pointPosition(num):
    global sensor, uart, lcd
    flag0 = 0
    while flag0 == 0:
        print('识别激光确定位置中')
        sensor.set_framesize(sensor.QVGA)
        sensor.skip_frames(time=300)
        img = sensor.snapshot()  # 获取图片
        roiQ = (19, 0, 254, 220)
        for blobs in img.find_blobs([(67, 90, 79, 15, -23, 69)], roi=roiQ, x_stride=1, y_stride=1,
                                      pixels_threshold=10, area_threshold=10, merge=True, margin=2):
            print(blobs)
            img.draw_cross(blobs.cx(), blobs.cy(), color=255, size=8)
            if num == 2:
                leftup = (blobs.cx(), blobs.cy())
            elif num == 3:
                rightdown = (blobs.cx(), blobs.cy())

            if flag == 2:
                str2 = str(blobs.cx()) + ',' + str(blobs.cy()) + ',0,0,0,0,0,0\r\n'
            elif flag == 3:
                str2 = '0,0,' + str(blobs.cx()) + ',' + str(blobs.cy()) + ',0,0,0,0\r\n'

            uart.write(str2)  # 发送数据
            print(str2 + '发送完毕')
            flag0 = 1


# 识别激光确定位置
def pointPosition_pid():
    global sensor, uart, lcd
    flag0 = 0
    flag=3
    global x
    global y
    while flag0 == 0:
        print('识别激光确定位置中')
        sensor.set_framesize(sensor.QVGA)
        sensor.skip_frames(time=300)
        img = sensor.snapshot()  # 获取图片
        roiQ = (19, 20, 254, 250)
        for blobs in img.find_blobs([(67, 90, 79, 15, -23, 69)], roi=roiQ, x_stride=1, y_stride=1,
                                      pixels_threshold=10, area_threshold=10, merge=True, margin=2):
            print(blobs)
            img.draw_cross(blobs.cx(), blobs.cy(), color=255, size=8)

            x = blobs.cx()
            y = blobs.cy()

            flag0 = 1



import utime  # 用于延时
import math   # 用于计算距离和插值

def A4Ti(num):
    global sensor, lcd, leftup, rightdown
    sensor.set_framesize(sensor.QVGA)  # 降低分辨率

    # 定义黑色的颜色阈值（LAB 颜色空间）
    black_threshold = (0, 50, -128, 127, -128, 127)  # L: 0-50, A: -128-127, B: -128-127

    rects_list = []
    quit = 0
    print("开始检测最大黑色矩形")
    while quit == 0:
        img = sensor.snapshot()

        # 查找黑色区域
        for blob in img.find_blobs([black_threshold], pixels_threshold=100, area_threshold=100, merge=True):
            # 在黑色区域中查找矩形
            for r in img.find_rects(threshold=10000, roi=blob.rect()):
                rects = r.corners()
                cx = r.x() + r.w() // 2  # 矩形中心
                cy = r.y() + r.h() // 2
                print('cx,cy', cx , cy )

                # 获取矩形四个顶点的坐标
                x0, y0, x1, y1, x2, y2, x3, y3 = rects[3][0] , rects[3][1] , rects[2][0] , rects[2][1] , \
                                                   rects[1][0] , rects[1][1] , rects[0][0] , rects[0][1]

                # 计算矩形面积
                area = r.w() * r.h()

                # 将矩形信息保存到列表中
                rects_list.append({
                    'rect': r,
                    'area': area,
                    'corners': (x0, y0, x1, y1, x2, y2, x3, y3)
                })

        # 如果检测到矩形
        if rects_list:
            # 按面积从大到小排序
            rects_list.sort(key=lambda x: x['area'], reverse=True)

            # 选择面积最大的矩形
            max_rect = rects_list[0]
            r = max_rect['rect']
            x0, y0, x1, y1, x2, y2, x3, y3 = max_rect['corners']

            # 检查矩形是否规则
            if abs((x1 - x0) - (x2 - x3)) < 10:
                if num == 3:
                    quit = 1
                    str3 = str(x0) + ',' + str(y0) + ',' + str(x1) + ',' + str(y1) + ',' + \
                           str(x2) + ',' + str(y2) + ',' + str(x3) + ',' + str(y3) + '\r\n'
                    uart.write(str3)  # 发送数据
                    print(str3 + 'Ti3发送完毕')
                if num == 4:
                    quit = 1
                    # 获取矩形的四个顶点坐标
                    corners = [(x0, y0), (x1, y1), (x2, y2), (x3, y3)]

                    # 在图像上绘制最大矩形顶点
                    for p in r.corners():
                        img.draw_circle(p[0], p[1], 5, color=(0, 255, 0))  # 绿色圆圈标记顶点

                    # 计算矩形的周长
                    perimeter = 0
                    for i in range(4):
                        x1_, y1_ = corners[i]
                        x2_, y2_ = corners[(i + 1) % 4]
                        perimeter += math.sqrt((x2_ - x1_) ** 2 + (y2_ - y1_) ** 2)

                    # 计算等距点的间隔
                    step = perimeter / 50

                    # 存储等距点的坐标
                    way_x=[]
                    way_y=[]
                    current_distance = 0
                    n=0
                    for i in range(4):
                        x1_, y1_ = corners[i]
                        x2_, y2_ = corners[(i + 1) % 4]
                        edge_length = math.sqrt((x2_ - x1_) ** 2 + (y2_ - y1_) ** 2)
                        while current_distance < perimeter:
                            if current_distance > (i + 1) * edge_length:
                                break
                            # 计算当前点在当前边上的位置
                            t = (current_distance - i * edge_length) / edge_length
                            x_tar = x1_ + t * (x2_ - x1_)
                            y_tar = y1_ + t * (y2_ - y1_)
                            way_x.append(x_tar)
                            way_y.append(y_tar)
                            n+=1
                            current_distance += step

                    print('debug:'+str(way_x))
                    print('debug:'+str(way_y))

                    j = 0
                    while j < 50:
                        # 调用 pointPosition_pid() 获取最新的激光点坐标
                        pointPosition_pid()
                        # 获取最新的 x 和 y 值
                        print('最新x和y值：'+str(x)+','+str(y))
                        # 更新 str3，使用最新的 x 和 y 值
                        str3 = str(way_x[j]) + ',' + str(way_y[j]) + ',' + str(x) + ',' + str(y) + ',0,0,0,0\r\n'

                        uart.write(str3)  # 发送数据
                        print(str3 + 'Ti4发送完毕')
                        j += 1
                        utime.sleep_ms(200)  # 延时 500ms

                # 清空矩形列表
                rects_list = []

while True:
    led_r.value(1)
    img = sensor.snapshot()  # 获取图片
    accept_uart()  # 串口接收数据

    if flag == 2:
        pointPosition(2)
    if flag == 3:
        pointPosition(3)
    if flag == 4:
        A4Ti(3)
    if flag == 6:
        A4Ti(4)
