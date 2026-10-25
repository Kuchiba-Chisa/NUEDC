import sensor, image, time ,math

# 初始化摄像头
sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=1000)
sensor.set_vflip(True)  # 视角垂直翻转
sensor.set_hmirror(False)  # 视角水平翻转

# 全局变量
clock = time.clock()
flag_find_first_rect = False
flag_find_second_rect = False
first_rect_corners = [[0, 0] for _ in range(4)]  # 内矩形四个顶点
second_rect_corners = [[0, 0] for _ in range(4)]  # 外矩形四个顶点
flagofzcx = 0
area2=0
coner=[[0,0] for _ in range(4)]
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


while(True):
    clock.tick()
    img = sensor.snapshot()
    img = img.gaussian(1)  # 高斯模糊，减少噪声

    # 找内矩形
    if flag_find_first_rect == False:
        for rect in img.find_rects(threshold=5000, x_gradient=20, y_gradient=20):  # 降低阈值和梯度
            if rect:
                area1 = rect.magnitude()
                if area1 > 50000 and area1 < 90000:  # 内矩形面积较小
                    flag_find_first_rect = True
                    area2=area1
                    first_rect_corners = sort_rectangle_corners(rect.corners())  # 排序顶点
                    coner=first_rect_corners
                    print("内矩形面积:", area1)
                    for i, corner in enumerate(coner):
                        print(f"顶点 {i}: ({corner[0]}, {corner[1]})")

    # 找外矩形
    if flag_find_first_rect == True:
        if flag_find_second_rect == False:
            for rect in img.find_rects(threshold=40000, x_gradient=40, y_gradient=40):  # 提高阈值和梯度
                if rect:
                    area = rect.magnitude()
                    print(area2)
                    if area > (area2+5000) and area < 150000:  # 外矩形面积较大
                        flag_find_second_rect = True
                        second_rect_corners = sort_rectangle_corners(rect.corners())  # 排序顶点
                        print("外矩形面积:", area)

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
        x0 = (first_rect_corners[2][0] + second_rect_corners[2][0]) // 2
        y0 = (first_rect_corners[2][1] + second_rect_corners[2][1]) // 2

        x1 = (first_rect_corners[3][0] + second_rect_corners[3][0]) // 2
        y1 = (first_rect_corners[3][1] + second_rect_corners[3][1]) // 2

        x2 = (first_rect_corners[0][0] + second_rect_corners[0][0]) // 2
        y2 = (first_rect_corners[0][1] + second_rect_corners[0][1]) // 2

        x3 = (first_rect_corners[1][0] + second_rect_corners[1][0]) // 2
        y3 = (first_rect_corners[1][1] + second_rect_corners[1][1]) // 2

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
