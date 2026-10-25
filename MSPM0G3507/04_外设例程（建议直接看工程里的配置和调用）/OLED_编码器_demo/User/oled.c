/*
 * oled.c
 *
 *  Created on: 2024年1月11日
 *      Author: junying
 */

#include "oled.h"

#define OLED_CMD    0   // 写命令
#define OLED_DATA   1   // 写数据

#include "oledfont.h"

u8 OLED_GRAM[144][8];  // OLED显存数组（144x64像素，每8行一个字节）

#define I2C_TARGET_ADDRESS   0x3C  // OLED的I2C地址

// 颜色反转函数
void OLED_ColorTurn(u8 i)
{
    if(i == 0)
    {
        OLED_WR_Byte(0xA6, OLED_CMD); // 正常显示
    }
    if(i == 1)
    {
        OLED_WR_Byte(0xA7, OLED_CMD); // 反色显示
    }
}

// 屏幕旋转180度
void OLED_DisplayTurn(u8 i)
{
    if(i == 0)
    {
        OLED_WR_Byte(0xC8, OLED_CMD); // 正常显示（COM扫描方向）
        OLED_WR_Byte(0xA1, OLED_CMD); // 正常显示（SEG方向）
    }
    if(i == 1)
    {
        OLED_WR_Byte(0xC0, OLED_CMD); // 反转显示（COM扫描方向）
        OLED_WR_Byte(0xA0, OLED_CMD); // 反转显示（SEG方向）
    }
}

// 向OLED写入一个字节
// mode: 数据/命令标志 0=命令, 1=数据
void OLED_WR_Byte(u8 dat, u8 mode)
{
    if(mode)
        I2C_Write_Bytes(I2C_TARGET_ADDRESS, 0x40, &dat, 1); // 写数据
    else
        I2C_Write_Bytes(I2C_TARGET_ADDRESS, 0x00, &dat, 1); // 写命令
}

// 开启OLED显示
void OLED_DisPlay_On(void)
{
    OLED_WR_Byte(0x8D, OLED_CMD); // 电荷泵使能
    OLED_WR_Byte(0x14, OLED_CMD); // 开启电荷泵
    OLED_WR_Byte(0xAF, OLED_CMD); // 开启显示
}

// 关闭OLED显示
void OLED_DisPlay_Off(void)
{
    OLED_WR_Byte(0x8D, OLED_CMD); // 电荷泵使能
    OLED_WR_Byte(0x10, OLED_CMD); // 关闭电荷泵
    OLED_WR_Byte(0xAE, OLED_CMD); // 关闭显示
}

// 更新显存到OLED
void OLED_Refresh(void)
{
    u8 i, n;
    u8 buf[128]; // 临时缓冲区

    for(i = 0; i < 8; i++) // 遍历8页（每页8行）
    {
        OLED_WR_Byte(0xB0 + i, OLED_CMD); // 设置页地址
        OLED_WR_Byte(0x00, OLED_CMD);     // 设置列地址低4位
        OLED_WR_Byte(0x10, OLED_CMD);     // 设置列地址高4位

        // 将当前页的128列数据拷贝到缓冲区
        for(n = 0; n < 128; n++)
            buf[n] = OLED_GRAM[n][i];

        // 通过I2C发送整页数据
        I2C_Write_Bytes(I2C_TARGET_ADDRESS, 0x40, buf, 128);
    }
}

// 清屏函数
void OLED_Clear(void)
{
    u8 i, n;
    for(i = 0; i < 8; i++)      // 遍历8页
    {
        for(n = 0; n < 128; n++) // 遍历128列
        {
            OLED_GRAM[n][i] = 0; // 清空显存
        }
    }
    OLED_Refresh(); // 更新到屏幕
}

// 画点函数
// x:0~127, y:0~63, t:1=点亮 0=熄灭
void OLED_DrawPoint(u8 x, u8 y, u8 t)
{
    u8 i, m, n;
    i = y / 8;      // 计算所在页（每页8行）
    m = y % 8;      // 计算页内行偏移
    n = 1 << m;     // 生成掩码
    if(t) {
        OLED_GRAM[x][i] |= n;  // 置位点亮
    } else {
        OLED_GRAM[x][i] &= ~n; // 清零熄灭
    }
}

// 画线函数（Bresenham算法）
// x1,y1:起点坐标, x2,y2:终点坐标, mode:0=擦除 1=绘制
void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode)
{
    int xerr = 0, yerr = 0;
    int delta_x = x2 - x1;
    int delta_y = y2 - y1;
    int distance = (delta_x > delta_y) ? delta_x : delta_y;
    int incx = (delta_x > 0) ? 1 : ((delta_x == 0) ? 0 : -1);
    int incy = (delta_y > 0) ? 1 : ((delta_y == 0) ? 0 : -1);

    for(int t = 0; t <= distance; t++) {
        OLED_DrawPoint(x1, y1, mode);
        xerr += delta_x;
        yerr += delta_y;
        if(xerr > distance) {
            xerr -= distance;
            x1 += incx;
        }
        if(yerr > distance) {
            yerr -= distance;
            y1 += incy;
        }
    }
}

// 画圆函数（中点圆算法）
// x,y:圆心坐标, r:半径
void OLED_DrawCircle(u8 x, u8 y, u8 r)
{
    int a = 0, b = r;
    while(2 * b * b >= r * r) {
        // 绘制8个对称点
        OLED_DrawPoint(x + a, y - b, 1);
        OLED_DrawPoint(x - a, y - b, 1);
        OLED_DrawPoint(x - a, y + b, 1);
        OLED_DrawPoint(x + a, y + b, 1);
        OLED_DrawPoint(x + b, y + a, 1);
        OLED_DrawPoint(x + b, y - a, 1);
        OLED_DrawPoint(x - b, y - a, 1);
        OLED_DrawPoint(x - b, y + a, 1);

        a++;
        if((a * a + b * b - r * r) > 0) {
            b--;
            a--;
        }
    }
}

// 显示单个字符
// x,y:坐标, chr:字符, size1:字体大小, mode:0=正常 1=反色
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size1, u8 mode)
{
    u8 temp, chr1 = chr - ' '; // 计算字库偏移
    u8 x0 = x, y0 = y;
    u8 size2 = (size1 == 8) ? 6 : (size1 / 8 + ((size1 % 8) ? 1 : 0)) * (size1 / 2);

    for(u8 i = 0; i < size2; i++) {
        // 选择字库
        if(size1 == 8) temp = asc2_0806[chr1][i];
        else if(size1 == 12) temp = asc2_1206[chr1][i];
        else if(size1 == 16) temp = asc2_1608[chr1][i];
        else if(size1 == 24) temp = asc2_2412[chr1][i];
        else return;

        // 逐位绘制
        for(u8 m = 0; m < 8; m++) {
            if(temp & 0x01) OLED_DrawPoint(x, y, mode);
            else OLED_DrawPoint(x, y, !mode);
            temp >>= 1;
            y++;
        }
        x++;
        if((size1 != 8) && ((x - x0) == size1 / 2)) {
            x = x0;
            y0 += 8;
        }
        y = y0;
    }
}

// 显示字符串
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 size1, u8 mode)
{
    while((*chr >= ' ') && (*chr <= '~')) {
        OLED_ShowChar(x, y, *chr, size1, mode);
        x += (size1 == 8) ? 6 : (size1 / 2);
        chr++;
    }
}

// 计算m的n次方
u32 OLED_Pow(u8 m, u8 n)
{
    u32 result = 1;
    while(n--) result *= m;
    return result;
}

// 显示数字
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size1, u8 mode)
{
    u8 t, temp;
    u8 m = (size1 == 8) ? 2 : 0;
    for(t = 0; t < len; t++) {
        temp = (num / OLED_Pow(10, len - t - 1)) % 10;
        OLED_ShowChar(x + (size1 / 2 + m) * t, y, temp + '0', size1, mode);
    }
}

// 显示汉字
void OLED_ShowChinese(u8 x, u8 y, u8 num, u8 size1, u8 mode)
{
    u8 temp, x0 = x, y0 = y;
    u16 size3 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * size1;

    for(u16 i = 0; i < size3; i++) {
        // 选择字库
        if(size1 == 16) temp = Hzk1[num][i];
        else if(size1 == 24) temp = Hzk2[num][i];
        else if(size1 == 32) temp = Hzk3[num][i];
        else if(size1 == 64) temp = Hzk4[num][i];
        else return;

        // 逐位绘制
        for(u8 m = 0; m < 8; m++) {
            if(temp & 0x01) OLED_DrawPoint(x, y, mode);
            else OLED_DrawPoint(x, y, !mode);
            temp >>= 1;
            y++;
        }
        x++;
        if((x - x0) == size1) {
            x = x0;
            y0 += 8;
        }
        y = y0;
    }
}

// 滚动显示（用于跑马灯效果）
void OLED_ScrollDisplay(u8 num, u8 space, u8 mode, u32 timeout)
{
    u32 Cnt = 0;
    timeout *= 144;
    for(u8 t = 0, m = 0; ; m++) {
        if(timeout && (++Cnt > timeout)) return;
        
        if(m == 0) OLED_ShowChinese(128, 24, t++, 16, mode);
        if(t == num) {
            for(u8 r = 0; r < 16 * space; r++) {
                // 左移一列
                for(u8 i = 1; i < 144; i++) {
                    for(u8 n = 0; n < 8; n++) {
                        OLED_GRAM[i-1][n] = OLED_GRAM[i][n];
                    }
                }
                OLED_Refresh();
            }
            t = 0;
        }
        if(m == 16) m = 0;
        
        // 正常左移
        for(u8 i = 1; i < 144; i++) {
            for(u8 n = 0; n < 8; n++) {
                OLED_GRAM[i-1][n] = OLED_GRAM[i][n];
            }
        }
        OLED_Refresh();
    }
}

// 显示图片
void OLED_ShowPicture(u8 x, u8 y, u8 sizex, u8 sizey, u8 BMP[], u8 mode)
{
    u16 j = 0;
    u8 x0 = x, y0 = y;
    sizey = sizey / 8 + ((sizey % 8) ? 1 : 0);
    
    for(u8 n = 0; n < sizey; n++) {
        for(u8 i = 0; i < sizex; i++) {
            u8 temp = BMP[j++];
            for(u8 m = 0; m < 8; m++) {
                if(temp & 0x01) OLED_DrawPoint(x, y, mode);
                else OLED_DrawPoint(x, y, !mode);
                temp >>= 1;
                y++;
            }
            x++;
            if((x - x0) == sizex) {
                x = x0;
                y0 += 8;
            }
            y = y0;
        }
    }
}

// OLED初始化
void OLED_Init(void)
{
    OLED_WR_Byte(0xAE, OLED_CMD); // 关闭显示
    OLED_WR_Byte(0x00, OLED_CMD); // 设置列地址低4位
    OLED_WR_Byte(0x10, OLED_CMD); // 设置列地址高4位
    OLED_WR_Byte(0x40, OLED_CMD); // 设置起始行地址
    OLED_WR_Byte(0x81, OLED_CMD); // 对比度设置
    OLED_WR_Byte(0xCF, OLED_CMD); // 亮度值(0-255)
    OLED_WR_Byte(0xA1, OLED_CMD); // 段重定向（水平翻转）
    OLED_WR_Byte(0xC8, OLED_CMD); // 行重定向（垂直翻转）
    OLED_WR_Byte(0xA6, OLED_CMD); // 正常显示（非反色）
    OLED_WR_Byte(0xA8, OLED_CMD); // 多路复用比例设置
    OLED_WR_Byte(0x3F, OLED_CMD); // 1/64占空比
    OLED_WR_Byte(0xD3, OLED_CMD); // 显示偏移
    OLED_WR_Byte(0x00, OLED_CMD); // 偏移量=0
    OLED_WR_Byte(0xD5, OLED_CMD); // 时钟分频
    OLED_WR_Byte(0x80, OLED_CMD); // 100Hz刷新率
    OLED_WR_Byte(0xD9, OLED_CMD); // 预充电周期
    OLED_WR_Byte(0xF1, OLED_CMD); // 15时钟预充电，1时钟放电
    OLED_WR_Byte(0xDA, OLED_CMD); // COM引脚配置
    OLED_WR_Byte(0x12, OLED_CMD); // 备用模式
    OLED_WR_Byte(0xDB, OLED_CMD); // VCOMH设置
    OLED_WR_Byte(0x30, OLED_CMD); // VCOMH=0.83*VCC
    OLED_WR_Byte(0x20, OLED_CMD); // 页地址模式
    OLED_WR_Byte(0x02, OLED_CMD); // 
    OLED_WR_Byte(0x8D, OLED_CMD); // 电荷泵设置
    OLED_WR_Byte(0x14, OLED_CMD); // 启用电荷泵
    OLED_Clear();                 // 清屏
    OLED_WR_Byte(0xAF, OLED_CMD); // 开启显示
}