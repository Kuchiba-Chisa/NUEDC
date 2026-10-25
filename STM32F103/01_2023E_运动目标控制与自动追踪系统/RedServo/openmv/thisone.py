import sensor
import time
import image
import gc
import utime  # 用于延时
import math   # 用于计算距离和插值
from machine import Pin
from pyb import UART
area2=0
coner=[[0,0] for _ in range(4)]


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
sensor.skip_frames(time=1000)  # 跳过一些帧来确保相机稳定

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



def sort_rectangle_corners(corners):
    """
    将矩形顶点以左上角为第一个点，按顺时针方向排序。

    参数:
    corners: 矩形四个顶点坐标，格式为 [(x0, y0), (x1, y1), (x2, y2), (x3, y3)]

    返回:
    排序后的顶点坐标，格式为 [(左上), (右上), (右下), (左下)]
    """
    # 计算中心点
    center_x = sum(p[0] for p in corners) / 4
    center_y = sum(p[1] for p in corners) / 4

    # 按顶点与中心点的角度排序
    def sort_key(p):
        return math.atan2(p[1] - center_y, p[0] - center_x)

    # 按逆时针方向排序
    sorted_corners = sorted(corners, key=sort_key)

    # 找到左上角的点（x + y 最小的点）
    top_left = min(sorted_corners, key=lambda p: p[0] + p[1])

    # 将左上角点作为第一个点，重新排序
    index = sorted_corners.index(top_left)
    return sorted_corners[index:] + sorted_corners[:index]



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
    global x
    global y
    while flag0 == 0:
        print('识别激光确定位置中')
        sensor.set_framesize(sensor.QVGA)
        sensor.skip_frames(time=300)
        img = sensor.snapshot()  # 获取图片
        # QVGA 下的 ROI
        #roiQ = (19, 20, 254, 250)

        # 调整为 VGA 下的 ROI
        roiVGA = (38, 40, 508, 500)  # 将 QVGA 的 ROI 坐标和尺寸乘以 2
        for blobs in img.find_blobs([(67, 90, 79, 15, -23, 69),(75, 12, 49, 11, 5, 53)], roi=roiVGA, x_stride=1, y_stride=1,
                                      pixels_threshold=10, area_threshold=10, merge=True, margin=2):
            print(blobs)
            img.draw_cross(blobs.cx(), blobs.cy(), color=255, size=8)

            x = blobs.cx()
            y = blobs.cy()

            flag0 = 1



def A4Ti(num):
    global sensor, lcd, leftup, rightdown
    # 全局变量
    clock = time.clock()
    flag_find_first_rect = False
    flag_find_second_rect = False
    first_rect_corners = [[0, 0] for _ in range(4)]  # 内矩形四个顶点
    second_rect_corners = [[0, 0] for _ in range(4)]  # 外矩形四个顶点
    flagofzcx = 0
    area2=0
    coner=[[0,0] for _ in range(4)]

    while(True):
        clock.tick()
        img = sensor.snapshot()
        img = img.gaussian(1)  # 高斯模糊，减少噪声
        if flag_find_first_rect == False:
            for rect in img.find_rects(threshold=40000, x_gradient=40, y_gradient=40):  # 提高阈值和梯度
                if rect:
                    area1 = rect.magnitude()
                    if area1 > 50000 and area1 < 150000:
                        flag_find_first_rect = True
                        area2=area1
                        first_rect_corners = sort_rectangle_corners(rect.corners())  # 排序顶点
                        coner=first_rect_corners
                        print("外矩形面积:", area1)
                        for i, corner in enumerate(coner):
                            print(f"顶点 {i}: ({corner[0]}, {corner[1]})")

        # 找内矩形
        if flag_find_first_rect == True:
            if flag_find_second_rect == False:
                roi=(coner[0][0]+7,coner[1][1]+7,coner[2][0]-coner[0][0]-14,coner[3][1]-coner[1][1]-14)
                img.draw_rectangle(roi, color=(255, 0, 0), thickness=2)
                for rect in img.find_rects(roi=roi,threshold=30000, x_gradient=20, y_gradient=20):
                    if rect:
                        area = rect.magnitude()
                        print(area2)
                        if area < (area2-5000) and area > 88000:
                            flag_find_second_rect = True
                            second_rect_corners = sort_rectangle_corners(rect.corners())  # 排序顶点
                            print("内矩形面积:", area)


        # 如果找到内矩形和外矩形
        if flag_find_first_rect == True and flag_find_second_rect == True:
            # 打印内矩形和外矩形的四个顶点坐标
            print("内矩形顶点坐标:")
            for i, corner in enumerate(coner):
                print(f"顶点 {i}: ({corner[0]}, {corner[1]})")

            print("外矩形顶点坐标:")
            for i, corner in enumerate(second_rect_corners):
                print(f"顶点 {i}: ({corner[0]}, {corner[1]})")

            # 计算内外矩形对应顶点的均值
            x0 = (first_rect_corners[1][0] + second_rect_corners[1][0]) // 2
            y0 = (first_rect_corners[1][1] + second_rect_corners[1][1]) // 2

            x1 = (first_rect_corners[0][0] + second_rect_corners[0][0]) // 2
            y1 = (first_rect_corners[0][1] + second_rect_corners[0][1]) // 2

            x2 = (first_rect_corners[3][0] + second_rect_corners[3][0]) // 2
            y2 = (first_rect_corners[3][1] + second_rect_corners[3][1]) // 2

            x3 = (first_rect_corners[2][0] + second_rect_corners[2][0]) // 2
            y3 = (first_rect_corners[2][1] + second_rect_corners[2][1]) // 2

            # 打印均值顶点坐标
            print("均值顶点坐标:")
            print(f"顶点 0: ({x0}, {y0})")
            print(f"顶点 1: ({x1}, {y1})")
            print(f"顶点 2: ({x2}, {y2})")
            print(f"顶点 3: ({x3}, {y3})")

            # 绘制内矩形
            img.draw_line(first_rect_corners[0][0], first_rect_corners[0][1], first_rect_corners[1][0], first_rect_corners[1][1], color=(255, 255, 255))
            img.draw_line(first_rect_corners[1][0], first_rect_corners[1][1], first_rect_corners[2][0], first_rect_corners[2][1], color=(255, 255, 255))
            img.draw_line(first_rect_corners[2][0], first_rect_corners[2][1], first_rect_corners[3][0], first_rect_corners[3][1], color=(255, 255, 255))
            img.draw_line(first_rect_corners[3][0], first_rect_corners[3][1], first_rect_corners[0][0], first_rect_corners[0][1], color=(255, 255, 255))
            for p in first_rect_corners:
                img.draw_circle(p[0], p[1], 3, color=(0, 255, 0))  # 绿色圆圈标记内矩形顶点

            # 绘制外矩形
            img.draw_line(second_rect_corners[0][0], second_rect_corners[0][1], second_rect_corners[1][0], second_rect_corners[1][1], color=(255, 255, 255))
            img.draw_line(second_rect_corners[1][0], second_rect_corners[1][1], second_rect_corners[2][0], second_rect_corners[2][1], color=(255, 255, 255))
            img.draw_line(second_rect_corners[2][0], second_rect_corners[2][1], second_rect_corners[3][0], second_rect_corners[3][1], color=(255, 255, 255))
            img.draw_line(second_rect_corners[3][0], second_rect_corners[3][1], second_rect_corners[0][0], second_rect_corners[0][1], color=(255, 255, 255))
            for p in second_rect_corners:
                img.draw_circle(p[0], p[1], 3, color=(255, 0, 0))  # 红色圆圈标记外矩形顶点

            # 绘制均值矩形
            img.draw_line(x0, y0, x1, y1, color=(0, 0, 255))  # 蓝色均值矩形
            img.draw_line(x1, y1, x2, y2, color=(0, 0, 255))
            img.draw_line(x2, y2, x3, y3, color=(0, 0, 255))
            img.draw_line(x3, y3, x0, y0, color=(0, 0, 255))
            for p in [(x0, y0), (x1, y1), (x2, y2), (x3, y3)]:
                img.draw_circle(p[0], p[1], 3, color=(0, 0, 255))  # 蓝色圆圈标记均值顶点
            flagofzcx+=1
            print(str(flagofzcx))
        if(flagofzcx == 10):
            break


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
            for p in corners:
                img.draw_circle(p[0], p[1], 5, color=(0, 255, 0))  # 绿色圆圈标记顶点

            # 计算矩形的周长
            perimeter = 0
            for i in range(4):
                x1_, y1_ = corners[i]
                x2_, y2_ = corners[(i + 1) % 4]
                perimeter += math.sqrt((x2_ - x1_) ** 2 + (y2_ - y1_) ** 2)

            # 计算等距点的间隔
            step = perimeter / 100
            edge_length_id = [0, 0, 0, 0]
            edge_length_sum = 0
            j = 0
            # 存储等距点的坐标
            current_distance = 0
            n = 0

            for i in range(4):
                if i == 0:
                    # 初始点
                    pointPosition_pid()
                    print('最新x和y值：' + str(x) + ',' + str(y))
                    x1_, y1_ = corners[0]
                    str3 = f"{int(x1_)},{int(y1_)},{int(x)},{int(y)},0,0,0,0\r\n"
                    uart.write(str3)  # 发送数据
                    print(str3 + '激光回到矩形边框初始点')
                    utime.sleep_ms(2000)  # 延时 2000ms

                x1_, y1_ = corners[i]
                x2_, y2_ = corners[(i + 1) % 4]
                edge_length = math.sqrt((x2_ - x1_) ** 2 + (y2_ - y1_) ** 2)
                edge_length_id.append(edge_length)
                while current_distance < perimeter:
                    while j < i:
                        edge_length_sum += edge_length_id[i + 3]
                        j += 1
                    if current_distance > edge_length_sum + edge_length:
                        break
                    # 计算当前点在当前边上的位置
                    print(str(edge_length_id))
                    print(str(edge_length_sum))
                    t = (current_distance - edge_length_sum) / edge_length
                    x_tar = x1_ + t * (x2_ - x1_)
                    y_tar = y1_ + t * (y2_ - y1_)
                    pointPosition_pid()
                    print('最新x和y值：' + str(x) + ',' + str(y))
                    # 生成 str3
                    str3 = f"{int(x_tar)},{int(y_tar)},{int(x)},{int(y)},0,0,0,0\r\n"
                    uart.write(str3)  # 发送数据
                    print(str3 + 'Ti4发送完毕')
                    # utime.sleep_ms(100)  # 延时 500ms
                    n += 1
                    current_distance += step

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
