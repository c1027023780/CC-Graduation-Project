
#include "gpio.h"
#include "spi.h"
#include "lcd.h"

uint_32 mypow(uint_8 m,uint_8 n);
void LCD_WR_DATA8(uint_8 da); //发送数据-8位参数
void LCD_WR_DATA(uint_16 da);
void LCD_WR_REG(uint_8 da);
void LCD_WR_REG_DATA(uint_8 reg,uint_16 da);
void Address_set(uint_16 x1,uint_16 y1,uint_16 x2,uint_16 y2);
//extern uint_8 asc2_1608[];
//const static uint_8 Letter[];
//const static uint_8  nAsciiDot[];
//const static struct typFNT_GB16  GB_16[];

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

////=====================================================================
////函数名称：LCD_ShowChar
////函数返回：无
////参数说明：x,y:显示字符坐标(240*320),ch:要显示的字符,
////       mode:叠加方式(1)还是非叠加方式(0)，
////       color:字符颜色（使用本文件后画笔颜色宏定义）
////功能概要：在指定位置显示一个指定颜色的字符,mode=1不改变背景色显示字符，mode=0 背景为黑显示字符
////       字符大小为16*8
////=====================================================================
//void LCD_ShowChar(uint_16 x,uint_16 y,uint_8 ch,
//	 uint_8 mode,uint_16 color)    //显示一个指定颜色字符
//{
//    uint_8 temp;
//    uint_8 pos,t;
//	uint_16 x0=x;
//	uint_16 colortemp=color;
//    if(x>LCD_W-16||y>LCD_H-16)return;
//	//设置窗口
//    ch=ch-' ';//得到偏移后的值
//	Address_set(x,y,x+8-1,y+16-1);      //设置光标位置
//	if(!mode) //非叠加方式
//	{
//		for(pos=0;pos<16;pos++)
//		{
//			temp=asc2_1608[(uint_16)ch*16+pos];		 //调用1608字体
//			for(t=0;t<8;t++)
//		    {
//		        if(temp&0x01)color=colortemp;
//				else color=0x0000;
//				LCD_WR_DATA(color);
//				temp>>=1;
//				x++;
//		    }
//			x=x0;
//			y++;
//		}
//	}else//叠加方式
//	{
//		for(pos=0;pos<16;pos++)
//		{
//		    temp=asc2_1608[(uint_16)ch*16+pos];		 //调用1608字体
//			for(t=0;t<8;t++)
//		    {
//		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos,color);//画一个点
//		        temp>>=1;
//		    }
//		}
//	}
//}
//
////=====================================================================
////函数名称：LCD_ShowNum
////函数返回：无
////参数说明：x,y:起点坐标(240*320),len:数字显示的宽度,num:数值(0~4294967295),color:颜色
////功能概要：在指定位置用指定颜色显示数字，len为显示宽度，当宽度少于数字位数时，高位丢失，当宽度大于数字
////       位数时补空格，数字以非叠加方式显示，即背景色为黑色
////=====================================================================
//void LCD_ShowNum(uint_16 x,uint_16 y,uint_32 num,
//	 uint_8 len,uint_16 color)    //显示数字，宽度太大左边补空格
//{
//	uint_8 t,temp;
//	uint_8 enshow=0;
//	num=(uint_16)num;
//	for(t=0;t<len;t++)
//	{
//		temp=(num/mypow(10,len-t-1))%10;   //从高位到低位
//		if(enshow==0&&t<(len-1))
//		{
//			if(temp==0)
//			{
//				LCD_ShowChar(x+8*t,y,' ',0,color);
//				continue;
//			}else enshow=1;
//
//		}
//	 	LCD_ShowChar(x+8*t,y,temp+48,0,color);
//	}
//}
//
////=====================================================================
////函数名称：LCD_Show2Num。
////函数返回：无
////参数说明：x,y:起点坐标(240*320),len:数字显示的宽度,num:数值(0~65535),color:颜色
////功能概要：在指定位置用指定颜色显示数字，len为显示宽度，当宽度少于数字位数时，高位丢失，当宽度大于数字
////       位数时补'0'，数字以非叠加方式显示，即背景色为黑色
////=====================================================================
//void LCD_Show2Num(uint_16 x,uint_16 y,uint_16 num,
//	 uint_8 len,uint_16 color)    //显示数字，宽度太大左边补0
//{
//	uint_8 t,temp;
//	for(t=0;t<len;t++)
//	{
//		temp=(num/mypow(10,len-t-1))%10;
//	 	LCD_ShowChar(x+8*t,y,temp+'0',0,color);
//	}
//}
//
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
//函数名称：LCD_ShowCharactor
//函数返回：无
//参数说明：x,y:起点坐标(240*320),index：汉字在字库中的索引,fontcolor:字体颜色,
//       groundbackcolor:背景颜色，p:字库起始地址
//功能概要：在指定位置显示一个汉字(32*32大小),使用16*16扩大为32*32
//=====================================================================
void LCD_ShowHugeCharactor(uint_16  x,uint_16 y,uint_16 fontcolor,
	 uint_16 groundbackcolor,const uint_8 *p)    //显示32*32汉字
{
	uint_8 i,j,s;
	const uint_8 *temp=p;
    Address_set(x,y,x+31,y+31); //设置区域
//	temp+=index*32;
	for(i=0;i<16;i++)
	{
		for(j=0;j<2;j++)
		{
			for(s=0;s<8;s++)
			{
			 	if((*temp&(1<<s))!=0)
				{
					LCD_WR_DATA(fontcolor);
					LCD_WR_DATA(fontcolor);
				}
				else
				{
					LCD_WR_DATA(groundbackcolor);
					LCD_WR_DATA(groundbackcolor);
				}
			}
			temp++;
		}
		temp -= 2;
		for(j=0;j<2;j++)
		{
			for(s=0;s<8;s++)
			{
			 	if((*temp&(1<<s))!=0)
				{
					LCD_WR_DATA(fontcolor);
					LCD_WR_DATA(fontcolor);
				}
				else
				{
					LCD_WR_DATA(groundbackcolor);
					LCD_WR_DATA(groundbackcolor);
				}
			}

			temp++;
		}
	}
}
////=====================================================================
////函数名称：LCD_Show32Charactor
////函数返回：无
////参数说明：x,y:起点坐标(240*320),index：汉字在字库中的索引,fontcolor:字体颜色,
////       groundbackcolor:背景颜色，p:字库起始地址
////功能概要：在指定位置显示一个汉字(32*32大小),汉字需要自行从字库提取
////=====================================================================
//void LCD_Show32Charactor(uint_16  x,uint_16 y,uint_8 index,uint_16 fontcolor,
//	 uint_16 groundbackcolor,const uint_8 *p)    //显示32*32汉字
//{
//	uint_8 i,j;
//	const uint_8 *temp;
//	temp = p;
//    Address_set(x,y,x+31,y+31); //设置区域
//	temp+=index*128;
//	for(j=0;j<128;j++)
//	{
//		for(i=0;i<8;i++)
//		{
//		 	//if((*temp&(1<<i))!=0)   //字模从低位到高位显示
//			if((*temp&(0x80>>i))!=0)  //字模从高位到低位显示
//			{
//				LCD_WR_DATA(fontcolor);
//			}
//			else
//			{
//				LCD_WR_DATA(groundbackcolor);
//			}
//
//		}
//		temp++;
//	 }
//}
////=====================================================================
////函数名称：LCD_Show16Charactor
////函数返回：无
////参数说明：x,y:起点坐标(240*320),index：汉字在字库中的索引,fontcolor:字体颜色,
////       groundbackcolor:背景颜色，p:字库起始地址
////功能概要：在指定位置显示一个汉字(32*32大小),汉字需要自行从字库提取
////=====================================================================
//void LCD_Show16Charactor(uint_16  x,uint_16 y,uint_8 index,uint_16 fontcolor,
//	 uint_16 groundbackcolor,const uint_8 *p)    //显示16*16汉字
//{
//	uint_8 i,j;
//	const uint_8 *temp;
//	temp = p;
//    Address_set(x,y,x+15,y+15); //设置区域
//	temp+=index*32;     //每个字占32个字节
//	for(j=0;j<32;j++)
//	{
//		for(i=0;i<8;i++)
//		{   //if((*temp&(1<<i))!=0)   //字模从低位到高位显示
//			if((*temp&(0x80>>i))!=0)  //字模从高位到低位显示
//			{
//				LCD_WR_DATA(fontcolor);
//			}
//			else
//			{
//				LCD_WR_DATA(groundbackcolor);
//			}
//
//		}
//		temp++;
//	 }
//}
////=====================================================================
////函数名称：LCD_ShowLetter
////函数返回：无
////参数说明：x,y:起点坐标(240*320),index：字符在字库中的索引,fontcolor:字体颜色,
////       groundbackcolor:背景颜色
////功能概要：在指定位置显示一个字母(32*32大小)
////=====================================================================
//void LCD_ShowLetter(uint_16 x,uint_16 y,uint_8 index,uint_16 fontcolor,
//	 uint_16 groundbackcolor)    //显示32*32英文字符
//{
//	uint_8 i,j;
//	const uint_8 *temp=Letter;  //Letter是字库数组名
//    Address_set(x,y,x+15,y+31); //设置区域
//    index = index-'A';
//    if (index >= 26)
//    	index = index-6;
//	temp+=index*64;
//	for(j=0;j<64;j++)
//	{
//		for(i=0;i<8;i++)
//		{
//		 	if((*temp&(1<<i))!=0)
//			{
//				LCD_WR_DATA(fontcolor);
//			}
//			else
//			{
//				LCD_WR_DATA(groundbackcolor);
//			}
//
//		}
//		temp++;
//	 }
//}
////=====================================================================
////函数名称：LCD_ShowStringA
////函数返回：无
////参数说明：x,y:起点坐标(240*320),p:字符串起始地址,color:字体颜色
////功能概要：在指定位置显示字符串,以非叠加方式显示
////=====================================================================
//void LCD_ShowStringA(uint_16 x,uint_16 y,const uint_8 *p ,
//		uint_16 color)    //显示字符串
//{
//    while(*p!='\0')
//    {
//        if(x>LCD_W-16){x=0;y+=16;}
//        if(y>LCD_H-16){y=x=0;LCD_Clear(RED);}
//        LCD_ShowChar(x,y,*p,0,color);
//        x+=8;
//        p++;
//    }
//}
//
////=====================================================================
////函数名称：LCD_ShowStringB
////函数返回：无
////参数说明：x,y:起点坐标(240*320),p:字符串起始地址,color:字体颜色
////功能概要：在指定位置显示字符串，以叠加方式显示
////=====================================================================
//void LCD_ShowStringB(uint_16 x,uint_16 y,const uint_8 *p ,
//		uint_16 color)    //显示字符串
//{
//    while(*p!='\0')
//    {
//        if(x>LCD_W-16){x=0;y+=16;}
//        if(y>LCD_H-16){y=x=0;LCD_Clear(RED);}
//        LCD_ShowChar(x,y,*p,1,color);  //以叠加方式显示
//        x+=8;
//        p++;
//    }
//}
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
	}
}

//=====================================================================
//函数名称：LCD_ShowHugeString
//函数返回：无
//参数说明：x,y:起点坐标(240*320),p:字符串起始地址（可以包含中文，但应包含在汉字字模中）
//       color:颜色（使用本文件后画笔颜色宏定义）
//功能概要：在指定位置显示字符串，可指定显示画笔颜色和背景颜色。
//                   中文大小为（32*32）,英文大小为（16*32）
//=====================================================================
void LCD_ShowHugeString(uint_16  x,uint_16 y,uint_16 fontcolor,
	 uint_16 groundbackcolor,const uint_8 *p)
{
	uint_8 i,j,k,s;
	const uint_8 *temp;
	k = 0;
	while(*p != 0)
	{
		if(*p>=0x20 && *p<=0x7e)
		{
			temp=nAsciiDot;
		    Address_set(x+16*k,y,x+15+16*k,y+31); //设置区域
			temp+=(*p-0x20)*16;
			for(j=0;j<8;j++)
			{
				for(s=0;s<2;s++)
				{
					for(i=0;i<8;i++)
					{
						if((*temp&(1<<i))!=0)
						{
							LCD_WR_DATA(fontcolor);
							LCD_WR_DATA(fontcolor);
						}
						else
						{
							LCD_WR_DATA(groundbackcolor);
							LCD_WR_DATA(groundbackcolor);
						}
					}
					temp++;
				}
	            temp-=2;
				for(s=0;s<2;s++)
				{
					for(i=0;i<8;i++)
					{
						if((*temp&(1<<i))!=0)
						{
							LCD_WR_DATA(fontcolor);
							LCD_WR_DATA(fontcolor);
						}
						else
						{
							LCD_WR_DATA(groundbackcolor);
							LCD_WR_DATA(groundbackcolor);
						}
					}
					temp++;
				}
			 }
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
                	 LCD_ShowHugeCharactor(x+16*k,y,fontcolor,groundbackcolor,GB_16[i].Msk);
                	 break;
                 }
             }
		}
		p++;
        k++;
	}
}
//=====================================================================
//函数名称：showimageA
//函数返回：无
//参数说明：x,y:起点坐标(240*320),width:图片宽度，height:图片高度，p:图片数组地址
//功能概要：在指定位置显示图片
//       未对图片超出显示窗口进行检查
//=====================================================================
void showimageA(uint_16 x,uint_16 y,uint_16 width,
	 uint_16 height,const uint_8 * p) //显示width*height大小的图片
{
  	int i,j,k;
	//LCD_Clear(WHITE); //清屏

	Address_set(x,y,x+width-1,y+height-1);	 //显示窗口设置

	for(i=0;i<width*height;i++)              //将数据写入显存
			 {
			  	 LCD_WR_DATA8(p[i*2+1]);     //高位在前
				 LCD_WR_DATA8(p[i*2]);       //低位在后
			 }
}
//=====================================================================
//函数名称：showimage1
//函数返回：无
//参数说明：x,y:起点坐标(240*320),p:图片数组地址
//功能概要：在指定位置显示图片，图片的高度与宽度在图片数组中,图片数组前8字含义见结构HEADCOLOR。
//       未对图片超出显示窗口进行检查
//=====================================================================
void showimage1(uint_16 x,uint_16 y,const uint_8 *p)
{
	HEADCOLOR headData;

	  headData.scan = (uint_8)(*(p+0));
	  headData.gray= (uint_8)(*(p+1));
	  headData.w= (uint_16)(*(p+2));
	  headData.h= (uint_16)(*(p+4));
	  headData.is565= (uint_8)(*(p+6));
	  headData.rgb= (uint_8)(*(p+7));

	  showimageA(x,y,headData.w,headData.h,p+8);
}
//=====================================================================
//函数名称：showimageB
//函数返回：无
//参数说明：x,y:起点坐标(240*320),width,height图片宽高，p:图片数组地址
//       per:放大倍数,为整数
//功能概要：在指定位置，以放大倍数显示图片，有对图片超出显示窗口进行检查
//=====================================================================
void showimageB(uint_16 x,uint_16 y,uint_16 width,
		 uint_16 height,const uint_8 *p,uint_8 per) //显示放大per倍图片
{
  	int i,j,k1,k2,m,n,w,h;

  	        w = x+width*per-1;
  	        h = y+height*per-1;
  	        if(w>=LCD_W) w=LCD_W-1;      //如果显示窗口大于屏幕则设置为屏幕宽度
  	        if(h>=LCD_H) h=LCD_H-1;

			Address_set(x,y,w,h);     //设置显示窗口


		    for(i=0,n=0;i<height;i++)    //将图像数据填入窗口
		    {
			   for(k1=0;k1<per;k1++,n++)
			   {
			      if(n<=h-y)
				  {

				    for(j=0,m=0;j<width;j++)
				     {
		    	       for(k2=0;k2<per;k2++,m++)
		    	         {
                            if(m<=w-x)
					          {
						        LCD_WR_DATA8( p[(i*width+j)*2+1]);
		    	                LCD_WR_DATA8( p[(i*width+j)*2] );
					           }
		    	          }
                     }
				  }
		       }

           }
}


void showimageC(uint_16 x,uint_16 y,uint_16 width,
		 uint_16 height,const uint_8 *p,uint_8 per) //显示放大per倍图片
{
  	int i,j,k,w,h;

  	        w = x+width*per-1;
  	        h = y+height*per-1;
  	        if(w>=LCD_W) w=LCD_W-1;      //如果显示窗口大于屏幕则设置为屏幕宽度
  	        if(h>=LCD_H) h=LCD_H-1;

			Address_set(x,y,w,h);     //设置显示窗口
			j=0;
		    for(i=0;i<width*height;i++)    //将图像数据填入窗口
		    {
		    	if(i%width==0 && i!=0 )    //
		    	  	{
		    	    	 i=i-width;
		    	    	 j++;
		    	   	}
		    	 if(j>=per)
		    	   {
		    		  i=i+width;
		    		  j=0;
		    	   }
                if ( ( (i%width)*per<=LCD_W-per ) && ((i/width)*per<=LCD_H))
                   {
		    	     for(k=0;k<per;k++)
		    	        { LCD_WR_DATA8(p[i*2+1]);
		    	          LCD_WR_DATA8(p[i*2]);
		    	        }
                    }

			 }

}
//=====================================================================
//函数名称：showimage2
//函数返回：无
//参数说明：x,y:起点坐标(240*320),p:图片数组地址, per:放大倍数,为整数
//功能概要：在指定位置显示图片，图片的高度与宽度在图片数组中,图片数组前8字含义见结构HEADCOLOR。
//       以放大per倍显示图片，有对图片超出显示窗口进行检查
//=====================================================================
void showimage2(uint_16 x,uint_16 y,const uint_8 *p,uint_8 per)
{
	HEADCOLOR headData;

	  headData.scan = (uint_8)(*(p+0));
	  headData.gray= (uint_8)(*(p+1));
	  headData.w= (uint_16)(*(p+2));
	  headData.h= (uint_16)(*(p+4));
	  headData.is565= (uint_8)(*(p+6));
	  headData.rgb= (uint_8)(*(p+7));

	  showimageB(x,y,headData.w,headData.h,p+8,per);
}
//=====================================================================
//函数名称：buttonUp
//函数返回：无
//参数说明：x,y:左上坐标(240*320),x1,y1:右下角坐标(240*320)
//功能概要：在指定位置显示一个凸起的按钮
//=====================================================================
void buttonUp(int x,int y,int x1,int y1)    //凸起的按钮
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
   color = WHITE; /*white*/
   LCD_DrawLine(x+i,y+i,x1-i,y+i,color);
   LCD_DrawLine(x+i,y+i,x+i,y1-i,color);
   color = BLACK;
   LCD_DrawLine(x1-i,y+i,x1-i,y1-i,color);
   LCD_DrawLine(x+i,y1-i,x1-i,y1-i,color);
   };
};

//=====================================================================
//函数名称：buttonDown
//函数返回：无
//参数说明：x,y:左上坐标(240*320),x1,y1:右下角坐标(240*320)
//功能概要：在指定位置显示一个凹下的按钮
//=====================================================================
void buttonDown(int x,int y,int x1,int y1)    //凹下的按钮
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
   color = BLACK; /*white*/
   LCD_DrawLine(x+i,y+i,x1-i,y+i,color);
   LCD_DrawLine(x+i,y+i,x+i,y1-i,color);
   color = WHITE;
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
    //LCD_DC=1;
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







