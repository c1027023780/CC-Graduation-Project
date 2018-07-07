
#include "gpio.h"
#include "spi.h"
#include "lcd.h"

uint_32 mypow(uint_8 m,uint_8 n);
void LCD_WR_DATA8(uint_8 da); //发送数据-8位参数
void LCD_WR_DATA(uint_16 da);
void LCD_WR_REG(uint_8 da);
void LCD_WR_REG_DATA(uint_8 reg,uint_16 da);
void Address_set(uint_16 x1,uint_16 y1,uint_16 x2,uint_16 y2);

//=====================================================================
//函数名称：Lcd_Init。
//函数返回：无
//参数说明：无
//功能概要：LCD初始化
//=====================================================================
void LCD_Init(void)     //LCD初始化
{
	SPI_init(SPI_0,1,6000,0,0);
	gpio_init(LCD_RS,1,0);
	gpio_init(LCD_DC,1,0);
	LCD_WR_REG_DATA(0,0);

	gpio_set(LCD_RS,0);
	Delay_ms(20);
	gpio_set(LCD_RS,1);
	Delay_ms(20);

	//************* Start Initial Sequence **********//
	LCD_WR_REG(0xCF);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0xD9);
	LCD_WR_DATA8(0X30);

	LCD_WR_REG(0xED);
	LCD_WR_DATA8(0x64);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0X12);
	LCD_WR_DATA8(0X81);

	LCD_WR_REG(0xE8);
	LCD_WR_DATA8(0x85);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x78);

	LCD_WR_REG(0xCB);
	LCD_WR_DATA8(0x39);
	LCD_WR_DATA8(0x2C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x34);
	LCD_WR_DATA8(0x02);

	LCD_WR_REG(0xF7);
	LCD_WR_DATA8(0x20);

	LCD_WR_REG(0xEA);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0xC0);    //Power control
	LCD_WR_DATA8(0x21);   //VRH[5:0]

	LCD_WR_REG(0xC1);    //Power control
	LCD_WR_DATA8(0x12);   //SAP[2:0];BT[3:0]

	LCD_WR_REG(0xC5);    //VCM control
	LCD_WR_DATA8(0x32);
	LCD_WR_DATA8(0x3C);

	LCD_WR_REG(0xC7);    //VCM control2
	LCD_WR_DATA8(0XC1);

	LCD_WR_REG(0x36);    // Memory Access Control
	LCD_WR_DATA8(0x08);

	LCD_WR_REG(0x3A);
	LCD_WR_DATA8(0x55);

	LCD_WR_REG(0xB1);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x18);

	LCD_WR_REG(0xB6);    // Display Function Control
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0xA2);



	LCD_WR_REG(0xF2);    // 3Gamma Function Disable
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0x26);    //Gamma curve selected
	LCD_WR_DATA8(0x01);

	LCD_WR_REG(0xE0);    //Set Gamma
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x20);
	LCD_WR_DATA8(0x1E);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x12);
	LCD_WR_DATA8(0x0B);
	LCD_WR_DATA8(0x50);
	LCD_WR_DATA8(0XBA);
	LCD_WR_DATA8(0x44);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x14);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x23);
	LCD_WR_DATA8(0x21);
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0XE1);    //Set Gamma
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x19);
	LCD_WR_DATA8(0x19);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x12);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x2D);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x3F);
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x25);
	LCD_WR_DATA8(0x2D);
	LCD_WR_DATA8(0x0F);

	LCD_WR_REG(0x11);    //Exit Sleep
	Delay_ms(120);
	LCD_WR_REG(0x29);    //Display on
}

//=====================================================================
//函数名称：LCD_Clear
//函数返回：无
//参数说明：background:背景颜色（使用本文件后画笔颜色宏定义）
//功能概要：LCD清屏
//=====================================================================
void LCD_Clear(uint_16 background)  //LCD清屏
{
	uint_16 i,j;
	Address_set(0,0,LCD_W-1,LCD_H-1);
    for(i=0;i<LCD_W;i++)
	 {
	  for (j=0;j<LCD_H;j++)
	   	{
        	LCD_WR_DATA(background);
	    }
	  }
}

//=====================================================================
//函数名称：LCD_DrawPoint
//函数返回：无
//参数说明：x,y:点坐标(240*320),
//       color:颜色（使用本文件后画笔颜色宏定义）
//功能概要：在指定位置画一个点
//=====================================================================
void LCD_DrawPoint(uint_16 x,uint_16 y,uint_16 color)    //画点
{
	Address_set(x,y,x,y);//设置光标位置
	LCD_WR_DATA(color);
}

//=====================================================================
//函数名称：LCD_DrawPoint_big
//函数返回：无
//参数说明：x,y:点坐标(240*320)
//       color:颜色（使用本文件后画笔颜色宏定义）
//功能概要：在指定位置画一个大点,大点的范围实际是一个以x-1,y-1为左上解，x+1,y+1为右下角的小矩形
//       包含9个点（3*3）的大点。
//=====================================================================
void LCD_DrawPoint_big(uint_16 x,uint_16 y,uint_16 color) //画一个大点
{
	LCD_DrawSurface(x-1,y-1,x+1,y+1,color);
}

//=====================================================================
//函数名称：Draw_Circle
//函数返回：无
//参数说明：x,y:中心点坐标(240*320),
//       r:圆的半径,
//       color:颜色（使用本文件后画笔颜色宏定义）
//功能概要：在指定位置画一个指定大小的圆
//=====================================================================
void LCD_DrawCircle(uint_16 x,uint_16 y,uint_8 r,uint_16 color)    //画圆
{
	int a,b;
	int di;
	a=0;b=r;
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x-b,y-a,color);             //3
		LCD_DrawPoint(x+b,y-a,color);             //0
		LCD_DrawPoint(x-a,y+b,color);             //1
		LCD_DrawPoint(x-b,y-a,color);             //7
		LCD_DrawPoint(x-a,y-b,color);             //2
		LCD_DrawPoint(x+b,y+a,color);             //4
		LCD_DrawPoint(x+a,y-b,color);             //5
		LCD_DrawPoint(x+a,y+b,color);             //6
		LCD_DrawPoint(x-b,y+a,color);
		a++;
		//使用Bresenham算法画圆
		if(di<0)di +=4*a+6;
		else
		{
			di+=10+4*(a-b);
			b--;
		}
		LCD_DrawPoint(x+a,y+b,color);
	}
}

//=====================================================================
//函数名称：LCD_DrawLine
//函数返回：无
//参数说明：x1,y1:起点坐标(240*320),x2,y2:终点坐标(240*320),
//       color:颜色（使用本文件后画笔颜色宏定义）
//功能概要：在指定两点间画一条指定颜色的直线
//=====================================================================
void LCD_DrawLine(uint_16 x1,uint_16 y1,
	 uint_16 x2, uint_16 y2,uint_16 color)    //画线
{
	uint_16 t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;

	delta_x=x2-x1; //计算坐标增量
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向
	else if(delta_x==0)incx=0;//垂直线
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if(delta_y==0)incy=0;//水平线
	else{incy=-1;delta_y=-delta_y;}
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
	else distance=delta_y;
	for(t=0;t<=distance+1;t++ )//画线输出
	{
		LCD_DrawPoint(uRow,uCol,color);//画点
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

//=====================================================================
//函数名称：LCD_DrawRectangle
//函数返回：无
//参数说明：x1,y1:左上角点坐标(240*320),x2,y2:右下角点坐标(240*320),
//       color:颜色（使用本文件后画笔颜色宏定义）
//功能概要：在指定左上角与右下角画一个指定线条颜色的矩形
//=====================================================================
void LCD_DrawRectangle(uint_16 x1, uint_16 y1,
     uint_16 x2, uint_16 y2,uint_16 color)    //画矩形
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}

//=====================================================================
//函数名称：LCD_DrawSurface
//函数返回：无
//参数说明：x1,y1:起点坐标(240*320),x2,y2:终点坐标(240*320),
//       color:颜色（使用本文件后画笔颜色宏定义）
//功能概要：指定颜色填充矩形区域，要确保x2>x1,y2>y1否则程序不能正常执行
//=====================================================================
void LCD_DrawSurface(uint_16 x1,uint_16 y1,
     uint_16 x2,uint_16 y2,uint_16 color)    //填充矩形区域
{
	uint_16 i,j;
	Address_set(x1,y1,x2,y2);      //设置光标位置
	for(i=y1;i<=y2;i++)
	{
		for(j=x1;j<=x2;j++)LCD_WR_DATA(color);//画点
	}
}

//=====================================================================
//函数名称：LCD_ShowCharactor
//函数返回：无
//参数说明：x,y:起点坐标(240*320),index：汉字在字库中的索引,fontcolor:字体颜色,
//       groundbackcolor:背景颜色，p:字库起始地址
//功能概要：在指定位置显示一个汉字(16*16大小),汉字需要自行从字库提取
//=====================================================================
void LCD_ShowCharactor(uint_16  x,uint_16 y,uint_16 fontcolor,
	 uint_16 groundbackcolor,const uint_8 *p)    //显示16*16汉字
{
	uint_8 i,j;
	const uint_8 *temp=p;
    Address_set(x,y,x+15,y+15); //设置区域
//	temp+=index*32;
	for(j=0;j<32;j++)
	{
		for(i=0;i<8;i++)
		{
		 	if((*temp&(1<<i))!=0)
			{
				LCD_WR_DATA(fontcolor);
			}
			else
			{
				LCD_WR_DATA(groundbackcolor);
			}

		}
		temp++;
	 }
}

//=====================================================================
//函数名称：LCD_ShowString
//函数返回：无
//参数说明：x,y:起点坐标(240*320),p:字符串起始地址（可以包含中文，但应包含在汉字字模中）
//       color:颜色（使用本文件后画笔颜色宏定义）
//功能概要：在指定位置显示字符串，可指定显示画笔颜色和背景颜色。
//                   中文大小为（16*16）,英文大小为（8*16）
//=====================================================================
void LCD_ShowString(uint_16  x,uint_16 y,uint_16 fontcolor,
	 uint_16 groundbackcolor,const uint_8 *p)
{
	uint_8 i,j,k;
	const uint_8 *temp;
	k = 0;
	while(*p != 0)
	{
		__disable_irq();
		if(*p>=0x20 && *p<=0x7e)
		{
			temp=nAsciiDot;
		    Address_set(x+8*k,y,x+7+8*k,y+15); //设置区域
			temp+=(*p-0x20)*16;
			for(j=0;j<16;j++)
			{
				for(i=0;i<8;i++)
				{
				 	if((*temp&(1<<i))!=0)
					{
						LCD_WR_DATA(fontcolor);
					}
					else
					{
						LCD_WR_DATA(groundbackcolor);
					}
				}
				temp++;
			 }
			k++;
		}
		else if(*p > 0x7e)
		{
             for(i=0;i<ChineseCharacterLength;i++)
             {
            	 uint_8 a = *p;
            	 uint_8 b = *(p+1);
            	 uint_8 c = GB_16[i].Index[0];
            	 uint_8 d = GB_16[i].Index[1];
                 if( (a == c) && (b == d))
                 {
                	 LCD_ShowCharactor(x+8*k,y,fontcolor,groundbackcolor,GB_16[i].Msk);
                	 break;
                 }
             }
             k+=1;
		}
		p++;
		__enable_irq();
	}
}

//=====================================================================
//函数名称：aotu_panel
//函数返回：无
//参数说明：x,y:左上坐标(240*320),x1,y1:右下角坐标(240*320)
//功能概要：在指定位置显示一个凹凸按钮
//=====================================================================
void aotu_panel(uint_16 x,uint_16 y,uint_16 x1,uint_16 y1,uint_8 s) //凹凸按钮
{
   uint_8 i,n;
   uint_16 color;
   color = GRAY;
   LCD_DrawSurface(x,y,x1,y1,color);
   n=2;
   color = BLACK;
   LCD_DrawLine(x,y,x1,y,color);
   LCD_DrawLine(x,y,x,y1,color);
   color = BLACK;
   LCD_DrawLine(x1,y,x1,y1,color);
   LCD_DrawLine(x,y1,x1,y1,color);



   for(i=1;i<n;i++)
   {


   color=(s==0?BLACK:WHITE);  //


   LCD_DrawLine(x+i,y+i,x1-i,y+i,color);
   LCD_DrawLine(x+i,y+i,x+i,y1-i,color);

   color=(s==0?WHITE:BLACK);

   LCD_DrawLine(x1-i,y+i,x1-i,y1-i,color);
   LCD_DrawLine(x+i,y1-i,x1-i,y1-i,color);
   };
};

//=====================================================================
//内部函数
//=====================================================================

//=====================================================================
//函数名称：mypow
//函数返回：m^n
//参数说明：m底数,n指数
//功能概要：计算m^n
//=====================================================================
uint_32 mypow(uint_8 m,uint_8 n)
{
	uint_32 result=1;
	while(n--)result*=m;
	return result;
}
void LCD_WR_DATA8(uint_8 da) //发送数据-8位参数
{
	gpio_set(LCD_DC,1);
	SPI_send1(0,da);
}
 void LCD_WR_DATA(uint_16 da)
{
	gpio_set(LCD_DC,1);
    SPI_send1(0,da>>8);
    SPI_send1(0,da);
}
void LCD_WR_REG(uint_8 da)
{
	gpio_set(LCD_DC,0);
	SPI_send1(0,da);
}
//void LCD_WR_REG_DATA(uint_16 reg,uint_16 da)
 void LCD_WR_REG_DATA(uint_8 reg,uint_16 da)
{
    LCD_WR_REG(reg);
	LCD_WR_DATA(da);
}
void Address_set(uint_16 x1,uint_16 y1,uint_16 x2,uint_16 y2)
{
	LCD_WR_REG(0x2a);      //写行地址命令
	LCD_WR_DATA8(x1>>8);   //写起始行地址
	LCD_WR_DATA8(x1);
	LCD_WR_DATA8(x2>>8);   //写结束行地址
	LCD_WR_DATA8(x2);

	LCD_WR_REG(0x2b);     //写列地址命令
	LCD_WR_DATA8(y1>>8);  //写列起始地址
	LCD_WR_DATA8(y1);
	LCD_WR_DATA8(y2>>8);  //写结束列地址
	LCD_WR_DATA8(y2);

	LCD_WR_REG(0x2C);     //写存储器命令
}







