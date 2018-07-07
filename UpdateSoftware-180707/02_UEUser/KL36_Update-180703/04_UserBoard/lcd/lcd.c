
#include "gpio.h"
#include "spi.h"
#include "lcd.h"

uint_32 mypow(uint_8 m,uint_8 n);
void LCD_WR_DATA8(uint_8 da); //��������-8λ����
void LCD_WR_DATA(uint_16 da);
void LCD_WR_REG(uint_8 da);
void LCD_WR_REG_DATA(uint_8 reg,uint_16 da);
void Address_set(uint_16 x1,uint_16 y1,uint_16 x2,uint_16 y2);

//=====================================================================
//�������ƣ�Lcd_Init��
//�������أ���
//����˵������
//���ܸ�Ҫ��LCD��ʼ��
//=====================================================================
void LCD_Init(void)     //LCD��ʼ��
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
//�������ƣ�LCD_Clear
//�������أ���
//����˵����background:������ɫ��ʹ�ñ��ļ��󻭱���ɫ�궨�壩
//���ܸ�Ҫ��LCD����
//=====================================================================
void LCD_Clear(uint_16 background)  //LCD����
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
//�������ƣ�LCD_DrawPoint
//�������أ���
//����˵����x,y:������(240*320),
//       color:��ɫ��ʹ�ñ��ļ��󻭱���ɫ�궨�壩
//���ܸ�Ҫ����ָ��λ�û�һ����
//=====================================================================
void LCD_DrawPoint(uint_16 x,uint_16 y,uint_16 color)    //����
{
	Address_set(x,y,x,y);//���ù��λ��
	LCD_WR_DATA(color);
}

//=====================================================================
//�������ƣ�LCD_DrawPoint_big
//�������أ���
//����˵����x,y:������(240*320)
//       color:��ɫ��ʹ�ñ��ļ��󻭱���ɫ�궨�壩
//���ܸ�Ҫ����ָ��λ�û�һ�����,���ķ�Χʵ����һ����x-1,y-1Ϊ���Ͻ⣬x+1,y+1Ϊ���½ǵ�С����
//       ����9���㣨3*3���Ĵ�㡣
//=====================================================================
void LCD_DrawPoint_big(uint_16 x,uint_16 y,uint_16 color) //��һ�����
{
	LCD_DrawSurface(x-1,y-1,x+1,y+1,color);
}

//=====================================================================
//�������ƣ�Draw_Circle
//�������أ���
//����˵����x,y:���ĵ�����(240*320),
//       r:Բ�İ뾶,
//       color:��ɫ��ʹ�ñ��ļ��󻭱���ɫ�궨�壩
//���ܸ�Ҫ����ָ��λ�û�һ��ָ����С��Բ
//=====================================================================
void LCD_DrawCircle(uint_16 x,uint_16 y,uint_8 r,uint_16 color)    //��Բ
{
	int a,b;
	int di;
	a=0;b=r;
	di=3-(r<<1);             //�ж��¸���λ�õı�־
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
		//ʹ��Bresenham�㷨��Բ
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
//�������ƣ�LCD_DrawLine
//�������أ���
//����˵����x1,y1:�������(240*320),x2,y2:�յ�����(240*320),
//       color:��ɫ��ʹ�ñ��ļ��󻭱���ɫ�궨�壩
//���ܸ�Ҫ����ָ������仭һ��ָ����ɫ��ֱ��
//=====================================================================
void LCD_DrawLine(uint_16 x1,uint_16 y1,
	 uint_16 x2, uint_16 y2,uint_16 color)    //����
{
	uint_16 t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;

	delta_x=x2-x1; //������������
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	if(delta_x>0)incx=1; //���õ�������
	else if(delta_x==0)incx=0;//��ֱ��
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if(delta_y==0)incy=0;//ˮƽ��
	else{incy=-1;delta_y=-delta_y;}
	if( delta_x>delta_y)distance=delta_x; //ѡȡ��������������
	else distance=delta_y;
	for(t=0;t<=distance+1;t++ )//�������
	{
		LCD_DrawPoint(uRow,uCol,color);//����
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
//�������ƣ�LCD_DrawRectangle
//�������أ���
//����˵����x1,y1:���Ͻǵ�����(240*320),x2,y2:���½ǵ�����(240*320),
//       color:��ɫ��ʹ�ñ��ļ��󻭱���ɫ�궨�壩
//���ܸ�Ҫ����ָ�����Ͻ������½ǻ�һ��ָ��������ɫ�ľ���
//=====================================================================
void LCD_DrawRectangle(uint_16 x1, uint_16 y1,
     uint_16 x2, uint_16 y2,uint_16 color)    //������
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}

//=====================================================================
//�������ƣ�LCD_DrawSurface
//�������أ���
//����˵����x1,y1:�������(240*320),x2,y2:�յ�����(240*320),
//       color:��ɫ��ʹ�ñ��ļ��󻭱���ɫ�궨�壩
//���ܸ�Ҫ��ָ����ɫ����������Ҫȷ��x2>x1,y2>y1�������������ִ��
//=====================================================================
void LCD_DrawSurface(uint_16 x1,uint_16 y1,
     uint_16 x2,uint_16 y2,uint_16 color)    //����������
{
	uint_16 i,j;
	Address_set(x1,y1,x2,y2);      //���ù��λ��
	for(i=y1;i<=y2;i++)
	{
		for(j=x1;j<=x2;j++)LCD_WR_DATA(color);//����
	}
}

//=====================================================================
//�������ƣ�LCD_ShowCharactor
//�������أ���
//����˵����x,y:�������(240*320),index���������ֿ��е�����,fontcolor:������ɫ,
//       groundbackcolor:������ɫ��p:�ֿ���ʼ��ַ
//���ܸ�Ҫ����ָ��λ����ʾһ������(16*16��С),������Ҫ���д��ֿ���ȡ
//=====================================================================
void LCD_ShowCharactor(uint_16  x,uint_16 y,uint_16 fontcolor,
	 uint_16 groundbackcolor,const uint_8 *p)    //��ʾ16*16����
{
	uint_8 i,j;
	const uint_8 *temp=p;
    Address_set(x,y,x+15,y+15); //��������
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
//�������ƣ�LCD_ShowString
//�������أ���
//����˵����x,y:�������(240*320),p:�ַ�����ʼ��ַ�����԰������ģ���Ӧ�����ں�����ģ�У�
//       color:��ɫ��ʹ�ñ��ļ��󻭱���ɫ�궨�壩
//���ܸ�Ҫ����ָ��λ����ʾ�ַ�������ָ����ʾ������ɫ�ͱ�����ɫ��
//                   ���Ĵ�СΪ��16*16��,Ӣ�Ĵ�СΪ��8*16��
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
		    Address_set(x+8*k,y,x+7+8*k,y+15); //��������
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
//�������ƣ�aotu_panel
//�������أ���
//����˵����x,y:��������(240*320),x1,y1:���½�����(240*320)
//���ܸ�Ҫ����ָ��λ����ʾһ����͹��ť
//=====================================================================
void aotu_panel(uint_16 x,uint_16 y,uint_16 x1,uint_16 y1,uint_8 s) //��͹��ť
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
//�ڲ�����
//=====================================================================

//=====================================================================
//�������ƣ�mypow
//�������أ�m^n
//����˵����m����,nָ��
//���ܸ�Ҫ������m^n
//=====================================================================
uint_32 mypow(uint_8 m,uint_8 n)
{
	uint_32 result=1;
	while(n--)result*=m;
	return result;
}
void LCD_WR_DATA8(uint_8 da) //��������-8λ����
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
	LCD_WR_REG(0x2a);      //д�е�ַ����
	LCD_WR_DATA8(x1>>8);   //д��ʼ�е�ַ
	LCD_WR_DATA8(x1);
	LCD_WR_DATA8(x2>>8);   //д�����е�ַ
	LCD_WR_DATA8(x2);

	LCD_WR_REG(0x2b);     //д�е�ַ����
	LCD_WR_DATA8(y1>>8);  //д����ʼ��ַ
	LCD_WR_DATA8(y1);
	LCD_WR_DATA8(y2>>8);  //д�����е�ַ
	LCD_WR_DATA8(y2);

	LCD_WR_REG(0x2C);     //д�洢������
}







