#include "dev_lcd.h"

#include "driver_LCD_ILI9481.h"
#include "font.h"
//#include "flowerpic.h"


#include "gpio.h"
#include "delay.h"

LCDTypedef g_lcd;

// 读取lcd id
uint16_t fsmc_lcd_read_id()
{
    uint16_t  id = 0;
    FSMC_LCD->REG = 0xB9;
    FSMC_LCD->RAM = 0XFF;
    FSMC_LCD->RAM= 0X83;
    FSMC_LCD->RAM = 0X69;

    FSMC_LCD->REG = 0XBF;
    id=FSMC_LCD->RAM;	//1dummy read
    id=FSMC_LCD->RAM;	//2读到0X02
    id=FSMC_LCD->RAM;   	//3读取04
    id=FSMC_LCD->RAM;   	//4读取94
    id<<=8;
    id|=FSMC_LCD->RAM;   //5读取81
    printf("LCD ID:%X",id);//将LCD ID打印到lcd_id数组
    printf("ADD:%x", FSMC_LCD_BASE_ADDR);
    return id;
}
// 复位
void dev_lcd_reset(void)
{
    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port,LCD_RESET_Pin,1);
    HAL_Delay(50);
    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port,LCD_RESET_Pin,0);
    HAL_Delay(50);
    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port,LCD_RESET_Pin,1);
    HAL_Delay(50);
}
// 初始化
void dev_lcd_init(void)
{
    g_lcd.dir = LCD_DIR;
    if(fsmc_lcd_read_id() == LCD_ILI9481_ID) {
        driver_LCD_ILI9481_init(&g_lcd);
        if(g_lcd.dir) {
            g_lcd.now_height = g_lcd.width;
            g_lcd.now_width = g_lcd.height;
        } else {
            g_lcd.now_height = g_lcd.height;
            g_lcd.now_width = g_lcd.width;
        }

    }
    dev_lcd_clear(BLACK);
}

// 设置画点
void dev_lcd_set_cursor(uint16_t x,uint16_t y)
{
    // 竖屏
    if(g_lcd.dir) {
        g_lcd.lcd_8080.set_set_cursor(g_lcd.height-x,y);
    } else { // 横屏
        g_lcd.lcd_8080.set_set_cursor(y,x);
    }
}

// 设置窗口
void dev_lcd_set_windows(uint16_t start_x,uint16_t start_y,uint16_t end_x,uint16_t end_y)
{
    // 竖屏
    if(g_lcd.dir) {
        // 原生分辨率
        g_lcd.lcd_8080.set_set_windows(g_lcd.height - end_x,start_y, g_lcd.height -start_x, end_y);
    } else { // 横屏
        g_lcd.lcd_8080.set_set_windows(start_y,start_x, end_y, end_x);
    }
}
// 填充颜色
void dev_lcd_write_color(uint16_t rgb_value)
{
    g_lcd.lcd_8080.write_color(rgb_value);
}
// 清屏
void dev_lcd_clear(uint16_t rgb_value)
{

    uint32_t all_pix = g_lcd.now_width * g_lcd.now_height;
    uint32_t i = 0;

    dev_lcd_set_windows(0,0,g_lcd.now_width,g_lcd.now_height);
    for(i = 0; i<all_pix; i++)
    {
        dev_lcd_write_color(rgb_value);
    }
}




/*******************************************************************************
//快速画点  modify YZ
//x,y:坐标
//color:颜色
*******************************************************************************/
void dev_lcd_draw_point(uint16_t x,uint16_t y,uint16_t color)
{

    dev_lcd_set_cursor(x,y);
    dev_lcd_write_color(color);
}

/*******************************************************************************
//在指定位置显示一个字符   YZ
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
//解释：
//1、字符宽度(size/2)＝高度(size)一半。
//2、字符取模为竖向取模，即每列占几个字节，最后不够完整字节数的占一字节。
//3、字符所占空间为：每列所占字节＊列数。
//csize=(size/8+((size%8)?1:0))*(size/2)
//乘号*前为计算每列所占字节数，乘号*后为列数（字符高度一半）
*******************************************************************************/
void dev_lcd_show_char(uint16_t x,uint16_t y,uint16_t num,uint8_t size,uint8_t mode,uint16_t  font_color, uint16_t  back_color)
{
    uint8_t temp,t1,t;
    uint16_t  y0=y;
    uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
    num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
    for(t=0; t<csize; t++)
    {
        if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
        else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
        else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
        else return;								//没有的字库
        for(t1=0; t1<8; t1++)
        {
            if(temp&0x80) {
                dev_lcd_draw_point(x,y,font_color);
            } else {
                if(mode) {
                    dev_lcd_draw_point(x,y,back_color);
                }

            }
            temp<<=1;
            y++;
            if(y>=g_lcd.height)return;		//超区域了
            if((y-y0)==size)
            {
                y=y0;
                x++;
                if(x>=g_lcd.width)return;	//超区域了
                break;
            }
        }
    }
}

void dev_lcd_show_string(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p,uint16_t color)
{
    uint8_t x0=x;
    width+=x;
    height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {
        if(x>=width) {
            x=x0;
            y+=size;
        }
        if(y>=height)break;//退出
        dev_lcd_show_char(x,y,*p,size,FONT_USE_BACK_COLOR,color,FONT_BACK_COLOR);
        x+=size/2;
        p++;
    }
}

//画线
//x1,y1:起点坐标
//x2,y2:终点坐标
void dev_lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
    uint16_t t;
    int xerr=0,yerr=0,delta_x,delta_y,distance;
    int incx,incy,uRow,uCol;
    delta_x=x2-x1; //计算坐标增量
    delta_y=y2-y1;
    uRow=x1;
    uCol=y1;
    if(delta_x>0)incx=1; //设置单步方向
    else if(delta_x==0)incx=0;//垂直线
    else {
        incx=-1;
        delta_x=-delta_x;
    }
    if(delta_y>0)incy=1;
    else if(delta_y==0)incy=0;//水平线
    else {
        incy=-1;
        delta_y=-delta_y;
    }
    if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
    else distance=delta_y;
    for(t=0; t<=distance+1; t++ ) //画线输出
    {
        dev_lcd_draw_point(uRow,uCol,color);//画点
        xerr+=delta_x ;
        yerr+=delta_y ;
        if(xerr>distance)
        {
            xerr-=distance;
            uRow+=incx;
        }
        if(yerr>distance)
        {
            yerr-=distance;
            uCol+=incy;
        }
    }
}

// 画矩形
void dev_lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
    dev_lcd_draw_line(x1,y1,x2,y1,color);
    dev_lcd_draw_line(x1,y1,x1,y2,color);
    dev_lcd_draw_line(x1,y2,x2,y2,color);
    dev_lcd_draw_line(x2,y1,x2,y2,color);
}

//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void dev_lcd_draw_circle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color)
{
    int a,b;
    int di;
    a=0;
    b=r;
    di=3-(r<<1);             //判断下个点位置的标志
    while(a<=b)
    {
        dev_lcd_draw_point(x0+a,y0-b,color);             //5
        dev_lcd_draw_point(x0+b,y0-a,color);             //0
        dev_lcd_draw_point(x0+b,y0+a,color);             //4
        dev_lcd_draw_point(x0+a,y0+b,color);             //6
        dev_lcd_draw_point(x0-a,y0+b,color);             //1
        dev_lcd_draw_point(x0-b,y0+a,color);
        dev_lcd_draw_point(x0-a,y0-b,color);             //2
        dev_lcd_draw_point(x0-b,y0-a,color);             //7
        a++;
        //使用Bresenham算法画圆
        if(di<0)di +=4*a+6;
        else
        {
            di+=10+4*(a-b);
            b--;
        }
    }
}


void dev_lcd_fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{
    uint16_t i,j;
    int xlen= ex-sx+1;
    for(i=sy; i<=ey; i++)
    {
        for(j=0; j<xlen; j++)
        {
            dev_lcd_set_cursor(sx+j,i);
            g_lcd.lcd_8080.write_color(color);
        }
    }
}

void dev_lcd_fast_fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{
    int xlen= ex-sx+1;
    int ylen = ey - sy +1;

    uint32_t all_pix = xlen * ylen;
    uint32_t i = 0;

    dev_lcd_set_windows(sx,sy,ex -1,ey -1);

    for(i = 0; i<all_pix; i++)
    {
        dev_lcd_write_color(color);
    }

}

//在指定区域内填充指定颜色块
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
//color:要填充的颜色
void dev_lcd_color_fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color)
{
    uint16_t height,width;
    uint16_t i,j;
    width=ex-sx+1; 			//得到填充的宽度
    height=ey-sy+1;			//高度
    for(i=0; i<height; i++)
    {
        dev_lcd_set_cursor(sx,sy+i);
        g_lcd.lcd_8080.write_gram_pre();
        for(j=0; j<width; j++)
        {
            g_lcd.lcd_8080.write_color(color[i*width+j]);//写入数据
        }
    }
}


void dev_lcd_test(void)
{
//	uint32_t pos,t;
//	uint16_t temp = 0;
//		int pix_w = 480;
//	int pix_h = 320;
//	
//	dev_lcd_set_windows(0,0,pix_w-1,pix_h-1); //OK 20171021  240-320图片

//	for (pos=0;pos<pix_w;pos++) // (pos=0;pos<lcddev.height;pos++)//
//	{
//		for(t=0;t< pix_h*2;t+=2) //(t=0;t<lcddev.width*2;t++) 
//		{
//			temp=gImage_1[pos* pix_h*2+t]*256+gImage_1[pos* pix_h*2+t+1];
//                            
//			g_lcd.lcd_8080.write_color(temp);          
//		}
//	}
	
}

