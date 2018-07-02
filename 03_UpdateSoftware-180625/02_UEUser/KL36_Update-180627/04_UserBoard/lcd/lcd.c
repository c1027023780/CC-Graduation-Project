
#include "gpio.h"
#include "spi.h"
#include "lcd.h"

uint_32 mypow(uint_8 m,uint_8 n);
void LCD_WR_DATA8(uint_8 da); //��������-8λ����
void LCD_WR_DATA(uint_16 da);
void LCD_WR_REG(uint_8 da);
void LCD_WR_REG_DATA(uint_8 reg,uint_16 da);
void Address_set(uint_16 x1,uint_16 y1,uint_16 x2,uint_16 y2);
//extern uint_8 asc2_1608[];
//const static uint_8 Letter[];
//const static uint_8  nAsciiDot[];
//const static struct typFNT_GB16  GB_16[];

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

////=====================================================================
////�������ƣ�LCD_ShowChar
////�������أ���
////����˵����x,y:��ʾ�ַ�����(240*320),ch:Ҫ��ʾ���ַ�,
////       mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)��
////       color:�ַ���ɫ��ʹ�ñ��ļ��󻭱���ɫ�궨�壩
////���ܸ�Ҫ����ָ��λ����ʾһ��ָ����ɫ���ַ�,mode=1���ı䱳��ɫ��ʾ�ַ���mode=0 ����Ϊ����ʾ�ַ�
////       �ַ���СΪ16*8
////=====================================================================
//void LCD_ShowChar(uint_16 x,uint_16 y,uint_8 ch,
//	 uint_8 mode,uint_16 color)    //��ʾһ��ָ����ɫ�ַ�
//{
//    uint_8 temp;
//    uint_8 pos,t;
//	uint_16 x0=x;
//	uint_16 colortemp=color;
//    if(x>LCD_W-16||y>LCD_H-16)return;
//	//���ô���
//    ch=ch-' ';//�õ�ƫ�ƺ��ֵ
//	Address_set(x,y,x+8-1,y+16-1);      //���ù��λ��
//	if(!mode) //�ǵ��ӷ�ʽ
//	{
//		for(pos=0;pos<16;pos++)
//		{
//			temp=asc2_1608[(uint_16)ch*16+pos];		 //����1608����
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
//	}else//���ӷ�ʽ
//	{
//		for(pos=0;pos<16;pos++)
//		{
//		    temp=asc2_1608[(uint_16)ch*16+pos];		 //����1608����
//			for(t=0;t<8;t++)
//		    {
//		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos,color);//��һ����
//		        temp>>=1;
//		    }
//		}
//	}
//}
//
////=====================================================================
////�������ƣ�LCD_ShowNum
////�������أ���
////����˵����x,y:�������(240*320),len:������ʾ�Ŀ��,num:��ֵ(0~4294967295),color:��ɫ
////���ܸ�Ҫ����ָ��λ����ָ����ɫ��ʾ���֣�lenΪ��ʾ��ȣ��������������λ��ʱ����λ��ʧ������ȴ�������
////       λ��ʱ���ո������Էǵ��ӷ�ʽ��ʾ��������ɫΪ��ɫ
////=====================================================================
//void LCD_ShowNum(uint_16 x,uint_16 y,uint_32 num,
//	 uint_8 len,uint_16 color)    //��ʾ���֣����̫����߲��ո�
//{
//	uint_8 t,temp;
//	uint_8 enshow=0;
//	num=(uint_16)num;
//	for(t=0;t<len;t++)
//	{
//		temp=(num/mypow(10,len-t-1))%10;   //�Ӹ�λ����λ
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
////�������ƣ�LCD_Show2Num��
////�������أ���
////����˵����x,y:�������(240*320),len:������ʾ�Ŀ��,num:��ֵ(0~65535),color:��ɫ
////���ܸ�Ҫ����ָ��λ����ָ����ɫ��ʾ���֣�lenΪ��ʾ��ȣ��������������λ��ʱ����λ��ʧ������ȴ�������
////       λ��ʱ��'0'�������Էǵ��ӷ�ʽ��ʾ��������ɫΪ��ɫ
////=====================================================================
//void LCD_Show2Num(uint_16 x,uint_16 y,uint_16 num,
//	 uint_8 len,uint_16 color)    //��ʾ���֣����̫����߲�0
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
//�������ƣ�LCD_ShowCharactor
//�������أ���
//����˵����x,y:�������(240*320),index���������ֿ��е�����,fontcolor:������ɫ,
//       groundbackcolor:������ɫ��p:�ֿ���ʼ��ַ
//���ܸ�Ҫ����ָ��λ����ʾһ������(32*32��С),ʹ��16*16����Ϊ32*32
//=====================================================================
void LCD_ShowHugeCharactor(uint_16  x,uint_16 y,uint_16 fontcolor,
	 uint_16 groundbackcolor,const uint_8 *p)    //��ʾ32*32����
{
	uint_8 i,j,s;
	const uint_8 *temp=p;
    Address_set(x,y,x+31,y+31); //��������
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
////�������ƣ�LCD_Show32Charactor
////�������أ���
////����˵����x,y:�������(240*320),index���������ֿ��е�����,fontcolor:������ɫ,
////       groundbackcolor:������ɫ��p:�ֿ���ʼ��ַ
////���ܸ�Ҫ����ָ��λ����ʾһ������(32*32��С),������Ҫ���д��ֿ���ȡ
////=====================================================================
//void LCD_Show32Charactor(uint_16  x,uint_16 y,uint_8 index,uint_16 fontcolor,
//	 uint_16 groundbackcolor,const uint_8 *p)    //��ʾ32*32����
//{
//	uint_8 i,j;
//	const uint_8 *temp;
//	temp = p;
//    Address_set(x,y,x+31,y+31); //��������
//	temp+=index*128;
//	for(j=0;j<128;j++)
//	{
//		for(i=0;i<8;i++)
//		{
//		 	//if((*temp&(1<<i))!=0)   //��ģ�ӵ�λ����λ��ʾ
//			if((*temp&(0x80>>i))!=0)  //��ģ�Ӹ�λ����λ��ʾ
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
////�������ƣ�LCD_Show16Charactor
////�������أ���
////����˵����x,y:�������(240*320),index���������ֿ��е�����,fontcolor:������ɫ,
////       groundbackcolor:������ɫ��p:�ֿ���ʼ��ַ
////���ܸ�Ҫ����ָ��λ����ʾһ������(32*32��С),������Ҫ���д��ֿ���ȡ
////=====================================================================
//void LCD_Show16Charactor(uint_16  x,uint_16 y,uint_8 index,uint_16 fontcolor,
//	 uint_16 groundbackcolor,const uint_8 *p)    //��ʾ16*16����
//{
//	uint_8 i,j;
//	const uint_8 *temp;
//	temp = p;
//    Address_set(x,y,x+15,y+15); //��������
//	temp+=index*32;     //ÿ����ռ32���ֽ�
//	for(j=0;j<32;j++)
//	{
//		for(i=0;i<8;i++)
//		{   //if((*temp&(1<<i))!=0)   //��ģ�ӵ�λ����λ��ʾ
//			if((*temp&(0x80>>i))!=0)  //��ģ�Ӹ�λ����λ��ʾ
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
////�������ƣ�LCD_ShowLetter
////�������أ���
////����˵����x,y:�������(240*320),index���ַ����ֿ��е�����,fontcolor:������ɫ,
////       groundbackcolor:������ɫ
////���ܸ�Ҫ����ָ��λ����ʾһ����ĸ(32*32��С)
////=====================================================================
//void LCD_ShowLetter(uint_16 x,uint_16 y,uint_8 index,uint_16 fontcolor,
//	 uint_16 groundbackcolor)    //��ʾ32*32Ӣ���ַ�
//{
//	uint_8 i,j;
//	const uint_8 *temp=Letter;  //Letter���ֿ�������
//    Address_set(x,y,x+15,y+31); //��������
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
////�������ƣ�LCD_ShowStringA
////�������أ���
////����˵����x,y:�������(240*320),p:�ַ�����ʼ��ַ,color:������ɫ
////���ܸ�Ҫ����ָ��λ����ʾ�ַ���,�Էǵ��ӷ�ʽ��ʾ
////=====================================================================
//void LCD_ShowStringA(uint_16 x,uint_16 y,const uint_8 *p ,
//		uint_16 color)    //��ʾ�ַ���
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
////�������ƣ�LCD_ShowStringB
////�������أ���
////����˵����x,y:�������(240*320),p:�ַ�����ʼ��ַ,color:������ɫ
////���ܸ�Ҫ����ָ��λ����ʾ�ַ������Ե��ӷ�ʽ��ʾ
////=====================================================================
//void LCD_ShowStringB(uint_16 x,uint_16 y,const uint_8 *p ,
//		uint_16 color)    //��ʾ�ַ���
//{
//    while(*p!='\0')
//    {
//        if(x>LCD_W-16){x=0;y+=16;}
//        if(y>LCD_H-16){y=x=0;LCD_Clear(RED);}
//        LCD_ShowChar(x,y,*p,1,color);  //�Ե��ӷ�ʽ��ʾ
//        x+=8;
//        p++;
//    }
//}
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
	}
}

//=====================================================================
//�������ƣ�LCD_ShowHugeString
//�������أ���
//����˵����x,y:�������(240*320),p:�ַ�����ʼ��ַ�����԰������ģ���Ӧ�����ں�����ģ�У�
//       color:��ɫ��ʹ�ñ��ļ��󻭱���ɫ�궨�壩
//���ܸ�Ҫ����ָ��λ����ʾ�ַ�������ָ����ʾ������ɫ�ͱ�����ɫ��
//                   ���Ĵ�СΪ��32*32��,Ӣ�Ĵ�СΪ��16*32��
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
		    Address_set(x+16*k,y,x+15+16*k,y+31); //��������
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
//�������ƣ�showimageA
//�������أ���
//����˵����x,y:�������(240*320),width:ͼƬ��ȣ�height:ͼƬ�߶ȣ�p:ͼƬ�����ַ
//���ܸ�Ҫ����ָ��λ����ʾͼƬ
//       δ��ͼƬ������ʾ���ڽ��м��
//=====================================================================
void showimageA(uint_16 x,uint_16 y,uint_16 width,
	 uint_16 height,const uint_8 * p) //��ʾwidth*height��С��ͼƬ
{
  	int i,j,k;
	//LCD_Clear(WHITE); //����

	Address_set(x,y,x+width-1,y+height-1);	 //��ʾ��������

	for(i=0;i<width*height;i++)              //������д���Դ�
			 {
			  	 LCD_WR_DATA8(p[i*2+1]);     //��λ��ǰ
				 LCD_WR_DATA8(p[i*2]);       //��λ�ں�
			 }
}
//=====================================================================
//�������ƣ�showimage1
//�������أ���
//����˵����x,y:�������(240*320),p:ͼƬ�����ַ
//���ܸ�Ҫ����ָ��λ����ʾͼƬ��ͼƬ�ĸ߶�������ͼƬ������,ͼƬ����ǰ8�ֺ�����ṹHEADCOLOR��
//       δ��ͼƬ������ʾ���ڽ��м��
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
//�������ƣ�showimageB
//�������أ���
//����˵����x,y:�������(240*320),width,heightͼƬ��ߣ�p:ͼƬ�����ַ
//       per:�Ŵ���,Ϊ����
//���ܸ�Ҫ����ָ��λ�ã��ԷŴ�����ʾͼƬ���ж�ͼƬ������ʾ���ڽ��м��
//=====================================================================
void showimageB(uint_16 x,uint_16 y,uint_16 width,
		 uint_16 height,const uint_8 *p,uint_8 per) //��ʾ�Ŵ�per��ͼƬ
{
  	int i,j,k1,k2,m,n,w,h;

  	        w = x+width*per-1;
  	        h = y+height*per-1;
  	        if(w>=LCD_W) w=LCD_W-1;      //�����ʾ���ڴ�����Ļ������Ϊ��Ļ���
  	        if(h>=LCD_H) h=LCD_H-1;

			Address_set(x,y,w,h);     //������ʾ����


		    for(i=0,n=0;i<height;i++)    //��ͼ���������봰��
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
		 uint_16 height,const uint_8 *p,uint_8 per) //��ʾ�Ŵ�per��ͼƬ
{
  	int i,j,k,w,h;

  	        w = x+width*per-1;
  	        h = y+height*per-1;
  	        if(w>=LCD_W) w=LCD_W-1;      //�����ʾ���ڴ�����Ļ������Ϊ��Ļ���
  	        if(h>=LCD_H) h=LCD_H-1;

			Address_set(x,y,w,h);     //������ʾ����
			j=0;
		    for(i=0;i<width*height;i++)    //��ͼ���������봰��
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
//�������ƣ�showimage2
//�������أ���
//����˵����x,y:�������(240*320),p:ͼƬ�����ַ, per:�Ŵ���,Ϊ����
//���ܸ�Ҫ����ָ��λ����ʾͼƬ��ͼƬ�ĸ߶�������ͼƬ������,ͼƬ����ǰ8�ֺ�����ṹHEADCOLOR��
//       �ԷŴ�per����ʾͼƬ���ж�ͼƬ������ʾ���ڽ��м��
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
//�������ƣ�buttonUp
//�������أ���
//����˵����x,y:��������(240*320),x1,y1:���½�����(240*320)
//���ܸ�Ҫ����ָ��λ����ʾһ��͹��İ�ť
//=====================================================================
void buttonUp(int x,int y,int x1,int y1)    //͹��İ�ť
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
//�������ƣ�buttonDown
//�������أ���
//����˵����x,y:��������(240*320),x1,y1:���½�����(240*320)
//���ܸ�Ҫ����ָ��λ����ʾһ�����µİ�ť
//=====================================================================
void buttonDown(int x,int y,int x1,int y1)    //���µİ�ť
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







