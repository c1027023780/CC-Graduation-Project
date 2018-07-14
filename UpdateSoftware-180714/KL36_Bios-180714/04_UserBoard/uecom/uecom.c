//===================================================================
//文件名称：uecom.c
//功能概要：UE驱动构件头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2018-05-23   V2.0
//备        注：适用于M6220
//===================================================================
#include "uecom.h"

#define AT_reBuf_length 200
static uint_8 AT_reBuf[200];            //存放UE的AT指令的返回结果
static uint_8 AT_reLength = 0;         //AT指令返回的数据长度
static uint_8 AT_haveCommand=0;//标志当前是否有AT指令发送
static uint_8 isGPRSdata = 0;
static uint_8 ATSendFlag;                 //标记一个AT指令已发送
static uint_8 frameHead[2] = "V!"; //帧头
static uint_8 frameTail[2] = "S$";     //帧尾
static uint_8 IMSI[15]={0};                //存储初始化后读取到的IMSI号
static uint_8 IMEI[15]={0};                //存储初始化后读取到的IMEI号

//======================内部函数声明====================================
//在函数定义处有详细说明
uint_8 uecom_sendCmd(uint_8 *cmd,uint_16 maxDelayMs,uint_16 maxTimes);
void uint_to_str(uint_32 ch,char *str);
void frameEncode(uint_8 *imsi,uint_8 *data,uint_16 dlen,uint_8 *frame,uint_16 *framelen);
uint_8 frameDecode(uint_8 *frame,uint_8 *cdata,uint_16 *len);
uint_16 crc16(uint_8 *ptr,uint_16 len);
uint_8 uecom_internal_getIMEI(uint_8 *dest);
uint_8 uecom_internal_getIMSI(uint_8 *dest);
uint_8 uecom_recv(uint_8 ch,uint_16 *dataLen, uint_8 *data);
//======================内部函数声明结束==================================


//========================变量声明======================================
	//函数中使用到的AT指令
	char AT[]   = "AT\r\n";                                          //测试与GPRS模组通讯的指令
	char CGSN[] = "AT+CGSN\r\n";                            //获取设备的IMEI号指令
	char CGDCONT[] =                                             //将上网类型设置为net，可以上外网
			"AT+CGDCONT=1, \"IP\", \"CMMTM\"\r\n";
	char CGACT_1[] ="AT+CGACT=1,1\r\n";                  //激活PDP，并向基站请求分配ip地址
//	char CGACT_0[] ="AT+CGACT=0\r\n";                //关闭PDP
	char CIFSR[] ="AT+CIFSR\r\n";                    //获取分配到的IP地址
	char CIPCLOSE[] ="AT+CIPCLOSE\r\n";                    //关闭TCP
	char CIPSEND[] ="AT+CIPSEND\r\n";                    //发送TCP数据
	char CIMI[] = "AT+CIMI\r\n";                              //获取设备的IMSI号指令
	char ATE_0[] = "ATE0\r\n";                                 //关闭回显
	char ATE_1[] = "ATE1\r\n";                                 //打开回显
	char CCED_1[] = "AT+CCED=0,1\r\n";            //获取当前小区信息
	char CSQ[] = "AT+CSQ\r\n";                             //获得信号强度
    //与http相关的
	char HTTPINIT[] = "AT+HTTPINIT\r\n";
	char HTTPSEND_0[] = "AT+HTTPSEND=0\r\n";
	char HTTPREAD[] = "AT+HTTPREAD?\r\n";  //?是问号，在KDS下变形了
	//与GNSS相关的AT指令
	char GPSACT_0[] = "AT+GPSACT=0\r\n";
	char GPSACT_1[] = "AT+GPSACT=1\r\n";
	char GPSPOS[] = "AT+GPSPOS\r\n";
	char GPSRMC[] = "AT+GPSRMC\r\n";
	char GPSRESTART_1[] = "AT+GPSRESTART=1\r\n";
	char GPSRESTART_2[] = "AT+GPSRESTART=2\r\n";
	char GPSRESTART_3[] = "AT+GPSRESTART=3\r\n";

	//AT命令是否执行成功的标志
	char OK[] = "OK";
	char ERROR[] = "ERROR";
//========================变量声明结束====================================

//=====================================================================
//函数名称：uecom_power
//函数返回：无
//参数说明：state:通信模组电源控制命令。可取值为（使用宏常数）：
//         UECOM_OFF：关闭通信模组电源；
//         UECOM_ON：打开通信模组电源；
//         UECOM_REBOOT：重启通信模组（先关闭，延时，再开启）。
//功能概要：控制通信模组供电状态
//修改日期：【20180610】
//=====================================================================
void uecom_power(uint_8 state)
{
	switch(state)
	{
	case UECOM_OFF:
		gpio_init(POWER_CONTROL,1,0);break;
	case UECOM_ON:
		gpio_init(POWER_CONTROL,1,1);break;
	case UECOM_REBOOT:
		gpio_init(POWER_CONTROL,1,0);
		uecom_delay_ms(2000);
		gpio_init(POWER_CONTROL,1,1);break;
	}
}

//=====================================================================
//函数名称：uecom_init
//函数返回： 0：成功；1：与UE模组串口通讯失败；2：获取SIM卡的IMSI号失败；
//参数说明： 无
//功能概要：uecom模组初始化。（1）建立与通讯模组的串口通讯；（2）获取SIM卡的IMSI号。
//修改日期：【20180706】WB；
//=====================================================================
uint_8 uecom_init(void)
{
	//（1）变量声明和赋初值
	uint_16 i;
	uint_8 ret_val ;
	uint_8 flag;
	//（2）使用串口与通信模组通信，首先初始化串口
	uart_init(UART_UE, 115200);   //初始化UE模组串口，波特率为115200
	uart_enable_re_int(UART_UE);  //打开UE模组串口中断
	//（3）测试GPRS模组是否正在运行。若不运行，则重启
	if(uecom_sendCmd((uint_8 *)AT,500,3))
	{
		uecom_power(UECOM_REBOOT);  //给UE模组重新供电
		uecom_delay_ms(8000);
	}
	if(uecom_sendCmd((uint_8 *)AT,500,3))
		goto uecom_init_err1;     //UE模组通讯失败
	//（4）关闭回显
	if(uecom_sendCmd((uint_8 *)ATE_0,500,2))
		goto uecom_init_err1;     //关闭回显失败
	//（5）查询模组的IMEI,并将IMEI号放在dest数组的前15个字节
	if(uecom_internal_getIMEI(IMEI))
		goto uecom_init_err1;     //IMEI查询失败
	//（6）查询模组的IMSI,并将IMSI号放在dest数组的15-29下标的空间中
	if(uecom_internal_getIMSI(IMSI))
		goto uecom_init_err2;      //IMSI查询失败
	//至此，没有失败退出，成功！
	ret_val = 0;
	goto uecom_init_exit;       //成功，ret_val=0
	//错误退出
uecom_init_err1:
	ret_val = 1;                       //与UE模组串口通讯失败
	goto uecom_init_exit;
uecom_init_err2:
	ret_val = 2;                      //获取SIM卡的IMSI号失败
	goto uecom_init_exit;
	//退出处
uecom_init_exit:
    return ret_val;
}

//=====================================================================
//函数名称： uecom_ linkBase
//函数返回： 0：成功建立TCP连接；1：连接不上铁塔；
//参数说明：无
//功能概要：与网络运营商的基站（铁塔）建立连接
//修改日期：【20180706】WB；
//=====================================================================
uint_8 uecom_linkBase(void)
{
	//（1）定义并初始化使用到的变量
	uint_8 ret = 1;
	//（2）设置联网方式
    if(uecom_sendCmd(CGDCONT,3000,2))
        goto uecom_linkBase_exit;
    //（3）附着核心网
    if(uecom_sendCmd(CGACT_1,3000,2))
        goto uecom_linkBase_exit;
    //（4）至此，执行成功
    ret = 0;
    uecom_linkBase_exit:
    return ret;
}

//=====================================================================
//函数名称：uecom_linkCS
//函数返回： 0：成功建立TCP连接；1：建立TCP连接失败
//参数说明： IP:待连接服务器的IP地址； port:待连接服务器的端口号
//功能概要：与指定的服务器和端口建立TCP连接。
//修改日期：【20180706】WB；
//=====================================================================
uint_8 uecom_linkCS(uint_8 *ip,uint_8* port)
{
	//（1）变量声明和赋初值
	uint_8 i;
	uint_8 flag;
	uint_8 ret_val;
	uint_8 CIPSTART[50] = "";				//建立TCP连接的地址和端口
	uint_8 loc;
	//（2）组建AT指令
	strcat((char *)CIPSTART,"AT+CIPSTART=\"TCP\",\"");
	loc = strlen(CIPSTART);
	for(i=0; i<15; i++)
	{
		if(ip[i] == 0)
		{
			break;
		}
		CIPSTART[loc+i] = ip[i];
	}
	strcat((char *)CIPSTART,"\",");
	loc = strlen(CIPSTART);
	for(i=0; i<5; i++)
	{
		if(port[i] == 0)
		{
			break;
		}
		CIPSTART[loc+i] = port[i];
	}
	strcat((char *)CIPSTART,"\r\n");
	//（3）建立TCP连接
	uecom_sendCmd(CIPCLOSE,2000,1);//首先关闭TCP
	if(uecom_sendCmd(CIPSTART,6000,2))
	{
		uecom_sendCmd(CIPCLOSE,2000,1);
		if(uecom_sendCmd(CIPSTART,6000,1))
		{
			ret_val = 2;
			goto uecom_config_exit;
		}
	}
	//（4）至此，执行成功
	ret_val = 0;
uecom_config_exit:
	return ret_val;
}

//=====================================================================
//函数名称：uecom_send
//函数返回：  0：发送成功；1：发送失败；2：返回超时（可能未正确开启发送模式）
//参数说明： data:待发送数据缓存区，传入参数
//		           length:待发送数据的长度
//功能概要：将数据通过已经建立的TCP/UDP通道发送出去。最多500个字节。
//修改日期：【20180706】WB；
//=====================================================================
uint_8 uecom_send(uint_16 length, uint_8 *data)
{
	//（1）变量声明，赋初值
	uint_8 ch,flag,k,i,l;
	uint_8 ret_val;
	char dataLen[5]="";
	char CIPSEND[20] = "";    //设置发送的字节数
	char buf[50];
	uint_8 frame[524];
	uint_16 frameLen;
    //（2）组帧
	frameEncode(IMSI,data, length,frame,&frameLen);
	//（3）将需要发送的数据长度发给UE模组，并开启发送模式
	uint_to_str(frameLen,dataLen);        //将待发送数据的长度转换为字符串格式
	strcat(CIPSEND,"AT+CIPSEND=");		//获取启动发送模式的AT指令字符串
	strcat(CIPSEND, dataLen);
	strcat(CIPSEND, "\r\n");
	uart_send_string(UART_UE, CIPSEND);	//指定发送的数据字节个数，并进入发送模式
	//（4）等待发送模式成功开启
	if(uecom_sendCmd("wait",3000,1))
	{
			ret_val = 1;
			goto uecom_send_exit;          //发送失败
	}
	AT_haveCommand=1;  //置位AT命令标志
    AT_reLength = 0;
    //（5）开始发送数据
	uart_sendN(UART_UE,frameLen,frame);    //发送数据
	//（6）等待返回发送成功
	if(uecom_sendCmd("SEND",6000,1))
	{
		ret_val = 2;
		goto uecom_send_exit;         //发送失败
	}
	ret_val = 0;
	uecom_send_exit:
	return ret_val;
}

//====================================================================
//函数名称：uecom_interrupt
//函数返回：无
//参数说明：ch：串口中断接收到的数据；length：接收到的网络数据长度；
//                recvData：存储接收到的网络数据
//功能概要：本函数需要放在串口中断中。并需要传入串口中断接收到的数据。
//  本构件的所有功能实现均依赖该api。本api实现的功能：
//  （1）接收网络发送来的数据;（2）构件内部使用本api与模块进行数据交互
//修改日期：【20180309】WB；
//=====================================================================
void uecom_interrupt(uint_8 ch,uint_16 *length,uint_8 recvData[])
{
	//保存数据到缓冲区中
	if(AT_haveCommand)// && !isGPRSdata)
	{
		AT_reBuf[AT_reLength] = ch;    //存储接收到的数据到缓冲区AT_reBuf中
		AT_reLength = (AT_reLength+1)%AT_reBuf_length;//接收到的数据下标递增
		AT_reBuf[AT_reLength] = 0;
	}
    uecom_recv(ch,length,recvData); //处理GPRS接收到的数据
}

//====================================================================
//函数名称：uecom_baseLocation
//函数返回：0：获取基站信息成功；1：获取信号强度失败；2：获取基站号失败
//参数说明：retData：存储返回的信息,最少分配20个字节。
//          信息组成：信号强度(1个字节)+基站号（19个字节）
//功能概要：获取与基站相关的信息：信号强度和基站号
//修改日期：【20180309】WB；
//=====================================================================
uint_8 uecom_baseInfo (uint_8 retData [20])
{
	//（1）变量声明和赋初值
	uint_8 i,j,*p,ret,ret_val;
	//（2）获取信号强度，最多获取三次，间隔1秒
	ret_val = 1;           //设置错误返回为1
	for(j=0;j<3;j++)
	{
		//（2.1）向模组发送获取信号强度的指令
		if( uecom_sendCmd((uint_8 *)CSQ,500,3))
			goto uecom_baseInfo_exit;
		//（2.2）从接收到的数据中解析出信号强度，转为uint_8格式，并赋给*signalPower
		i = strstr(AT_reBuf,"+CSQ: ")-(char*)AT_reBuf;
		i+=5;
		retData[0]= 0;
		while(AT_reBuf[i]!=','&&i<AT_reLength)
		{
			if(AT_reBuf[i]>='0' && AT_reBuf[i]<='9')
				retData[0] = retData[0]*10 + (AT_reBuf[i]-'0');
			i++;
		}
		if(retData[0]!=0)
	    {
			retData[0] = (uint_16)100*(retData[0])/31;
			break;
		}
		uecom_delay_ms(1000);
	}
	if(j==3)goto uecom_baseInfo_exit;  //若3次均失败，则认为获取信号强度失败
    //（3）获得基站号
	ret = 2;           //设置错误返回为2
	//（3.1）向模组发送获取基站号的指令
	if(uecom_sendCmd(CCED_1,500,3))
		goto uecom_baseInfo_exit;
    //（3.2）从接收到的数据中解析出基站号，并存入data数组中
    p = strstr(AT_reBuf,"+CCED:");   //获得字符':'所在的位置，其后为所需的数据。
    p += 6;
    j = 1;
    i = 0;
    //取第5个逗号之前的数据并存入data数组中
    while(i<5)
    {
        if(*p == ',')
            i++;
        retData[j++] = *p;
        p++;
    }
    if(j<=0)  //未能成功解析到数据
		goto uecom_baseInfo_exit;   //退出函数
    //最后一个逗号删除，并设置为字符串结束符
    retData[j-1]=0;
    //（4）运行至此，说明执行成功
    ret = 0;          //返回值设为0
    uecom_baseInfo_exit:
    return ret;
}

//====================================================================
//函数名称：uecom_ modelInfo
//函数返回：0：获取模组信息成功；1：获取模组信息失败
//参数说明：retData：存储返回的信息,最少分配40个字节。
//          信息组成：IMSI (20个字节)+IMEI（20个字节）
//功能概要：获得需要与模块相关的信息，包括：IMEI号，IMSI号
//修改日期：【20180309】WB；
//=====================================================================
uint_8 uecom_modelInfo (uint_8 retData[40])
{
	int i;
	//（1）从本构件的全局变量中取出IMEI并赋值
	for(i=0;i<15;i++)
		retData[i]=IMEI[i];
	retData[i] = 0;    //结束符
	//（2）从本构件的全局变量中取出IMSI并赋值
	for(i=20;i<35;i++)
		retData[i]=IMSI[i-20];
	retData[i] = 0;   //结束符
	return 0;
}

//=====================================================================
//函数名称：uecom_ gnssSwitch
//函数返回：0：操作GNSS成功；1：操作GNSS失败
//参数说明：state：设置GNSS的开关状态。0：关闭；1：热启动；2：温启动；3：冷启动；
//          建议默认使用冷启动。
//功能概要：设置GNSS的状态。开启或关闭GNSS，并设定开启方式。
//修改日期：【20180707】WB；
//=====================================================================
uint_8 uecom_gnssSwitch (uint_8 state)
{
	//（1）定义使用到的变量
	uint_8  ret;
	ret = 1;    //默认返回失败
	//（2）若不是关闭GNSS，则首先开启电源
    if(state != 0)
	{
	    uecom_sendCmd(GPSACT_1,1000,2);
		uecom_delay_ms(500);    //延时500ms开启GNSS模组
	}
	//（3）根据传入值操作GNSS
	switch(state)
	{
	case 0:
    	if(uecom_sendCmd(GPSACT_0,1000,2))
    		goto uecom_gnssSwitch_exit;break;
	case 1:
    	if(uecom_sendCmd(GPSRESTART_1,1000,2))
    		goto uecom_gnssSwitch_exit;break;
	case 2:
    	if(uecom_sendCmd(GPSRESTART_2,1000,2))
    		goto uecom_gnssSwitch_exit;break;
	case 3:
    	if(uecom_sendCmd(GPSRESTART_3,1000,2))
    		goto uecom_gnssSwitch_exit;break;
	default:
		break;
	}
	//至此，本函数运行完成
	ret = 0;
	uecom_gnssSwitch_exit:
	return ret;
}

//====================================================================
//函数名称：uecom_ gnssGetInfo
//函数返回：0：获取信息成功；1：与通信模组通信失败；2：获取UTC时间失败；
//                 3：获取定位信息失败；4：获取速度信息失败；5：获取海拔信息失败
//参数说明：data：存储获得的GNSS相关信息。采用结构体的方式，共包含4个成员：
//          time（15个字节的uint_8数组）,例如:“20180706155132”表示2018年7月6日15:51:32
//          latitude（double类型），纬度信息；longitude（double），经度信息；
//          speed（double类型），速度，单位为：米每秒。
//          attitude（double类型），海拔高度，单位为：米
//功能概要：获得与GNSS定位相关的信息。
//=====================================================================
uint_8 uecom_gnssGetInfo (UecomGnssInfo *data)     // UecomGnssInfo为存储gnss信息的结构体数据
{
	//（1）定义本函数使用的变量
	uint_8 i,j,ret,index;
	double a,b,c,d;
	//（2）发送AT指令获得最小定位信息。最多尝试3次
	ret = 1;        //设置默认返回1
	if(uecom_sendCmd(GPSRMC,500,3))
		goto uecom_gnssGetInfo_exit;
	//（3）从通信模组返回数据中解析出UTC时间
	ret = 2;      //设置默认返回2
	//（3.1）判断是否有正确返回
    if(strstr((char*)AT_reBuf,"20000101000000"))
    	goto uecom_gnssGetInfo_exit;
    //（3.2）至此，有正确时间。解析出该时间，并存入结构体的time变量中
    i=0;    j=0;
    while(i<14 && j<30)
    {
    	if(AT_reBuf[j]>='0' &&  AT_reBuf[j] <= '9')
    		data->time[i++] = AT_reBuf[j];
    	j++;
    }
    data->time[i]=0;
    //（4）判断位置信息是否为有效数据。
    //备注：若通过此判断，则说明位置，速度，海拔等信息可靠。否则表示不可靠
    //          UTC时间不需要通过此判断也会可靠，因此其通过其他方法判断（比较初值）。
    ret = 3;
	i = strstr(AT_reBuf,"+GPSRMC:")-(char*)AT_reBuf;
	i+=24;
    if(AT_reBuf[i]!='A')
    	goto uecom_gnssGetInfo_exit;
    //至此，获得的定位等信息为有效信息
    //（5）从通信模组返回数据中解析出位置信息，并存入传入的结构体
    //（5.1）提取纬度信息，并存入结构体的latitude成员中
    i += 2;
    a = AT_reBuf[i++] - '0';
    a  = a*10 + AT_reBuf[i++] - '0';
	b = AT_reBuf[i++] - '0';
	while(AT_reBuf[i]!='.')
	{
		b = b*10 + AT_reBuf[i++] - '0';
	}
	i ++;
	c = 0.1;
	b = b + (AT_reBuf[i++] - '0')*c;
	while(AT_reBuf[i]!=',')
	{
		c *= 0.1;
		b = b + (AT_reBuf[i++] - '0')*c;
	}
	i++;
	if(AT_reBuf[i] == 'N')
		d = 1;
	else
		d = -1;
	data->latitude = (a + b/60)*d;
	//（5.2）提取经度信息，并存入结构体的longitude成员中
	i += 2;
    a = AT_reBuf[i++] - '0';
    a  = a*10 + AT_reBuf[i++] - '0';
    a  = a*10 + AT_reBuf[i++] - '0';
	b = AT_reBuf[i++] - '0';
	while(AT_reBuf[i]!='.')
	{
		b = b*10 + AT_reBuf[i++] - '0';
	}
	i ++;
	c = 0.1;
	b = b + (AT_reBuf[i++] - '0')*c;
	while(AT_reBuf[i]!=',')
	{
		c *= 0.1;
		b = b + (AT_reBuf[i++] - '0')*c;
	}
	i++;
	if(AT_reBuf[i] == 'E')
		d = 1;
	else
		d = -1;
	data->longitude = (a + b/60)*d;
	//（6）从通信模组返回数据中解析出速度信息，并存入传入的结构体
    if(strstr(AT_reBuf,"E,") != NULL)
    	 i = strstr(AT_reBuf,"E,")-(char*)AT_reBuf;
    else
    	 i = strstr(AT_reBuf,"W,")-(char*)AT_reBuf;
    i+=2;
    a = 0;
	while(AT_reBuf[i]!='.')
	{
		a = a*10 + AT_reBuf[i++] - '0';
	}
	i++;
	b = 0.1;
	a = a + (AT_reBuf[i++] - '0')*b;
	while(AT_reBuf[i]!=',')
	{
		b *= 0.1;
		a = a + (AT_reBuf[i++] - '0')*b;
	}
	data->speed = a*0.5144444;
	//（7）从通信模组返回数据中解析出海拔信息，并存入传入的结构体
    i = strstr(AT_reBuf,"OK")-(char*)AT_reBuf;
   while(AT_reBuf[i]!=',')
   	i--;
   i += 2;
   a = 0;
	while(AT_reBuf[i]!='.')
	{
		a = a*10 + AT_reBuf[i++] - '0';
	}
	i++;
	b = 0.1;
	a = a + (AT_reBuf[i++] - '0')*b;
	while(AT_reBuf[i]!='\r')
	{
		b *= 0.1;
		a = a + (AT_reBuf[i++] - '0')*b;
	}
	data->attitude = a;
	//至此，数据解析完成
    ret = 0;
    uecom_gnssGetInfo_exit:
    return ret;
}

//====================================================================
//函数名称：http_get
//函数返回：0：获得get请求成功；1：初始化http失败；2：传递url参数失败；3：设置网络失败；
//                4：开启网络失败；5：建立连接失败；6：发送请求失败；7：获得返回失败；
//参数说明：ip:目标服务器地址；port :目标地址；url:get请求的内容。result:get请求返回的结果，
//                数组长度由预计返回的长度（用户应已知返回内容）*1.5来决定。
//功能概要：发起http的get请求，并将返回结果存储在result中
//修改日期：【2018-03-09】WB；
//=====================================================================
uint_8 uecom_httpGet (uint_8 ip[],uint_8 port[],uint_8 url[],uint_8 result[])
{
	uint_8 ret;
	uint_8 allUrl[500] = "AT+HTTPPARA=\"URL\",\"";
	uint_16 len = 0 , i;
	uint_8 *p;
	//（1）初始化http
	if(uecom_sendCmd(HTTPINIT,2000,2))
	{
		ret = 1;
		goto http_get_exit;
	}
	//（2）传递参数
	strcat(allUrl,url);
	strcat(allUrl,"\"\r\n");
	if(uecom_sendCmd(allUrl,2000,2))
	{
		ret = 2;
		goto http_get_exit;
	}
	//（3）设置网络
	if(uecom_sendCmd(CGDCONT,2000,2))
	{
		ret = 3;
		goto http_get_exit;
	}
	//（4）开启网络
	if(uecom_sendCmd(CGACT_1,5000,3))
	{
		ret = 4;
		goto http_get_exit;
	}
	//（5）建立连接
	if(uecom_linkCS(ip, port))
	{
		ret = 5;
		goto http_get_exit;
	}
	//（6）发送请求
	if(uecom_sendCmd(HTTPSEND_0,5000,3))
	{
		ret = 6;
		goto http_get_exit;
	}
	uecom_delay_ms(500);
	//（7）获得返回
	if(uecom_sendCmd(HTTPREAD,8000,3))
	{
		ret = 7;
		goto http_get_exit;
	}
	p = strstr(AT_reBuf,"+HTTPREAD:");
	p+=10;
	while(*p>='0'&&*p<='9')
	{
		len = len*10+*p-'0';
		p++;
	}
	p+=2;
	for(i=0;i<len;i++)
		result[i] = *(p+i);
	result[len] = 0;
	//至此运行成功
	ret = 0;
	http_get_exit:
	return ret;
}

//=====================================================================
//===================以下为内部函数=======================================
//=====================================================================

//=====================================================================
//函数名称：uecom_sendCmd
//函数返回：0：指令发送成功;1：指令发送失败
//参数说明：cmd：需要发送的AT指令的首地址;buf[]:传地址，带回AT指令返回结果,最多需要200
//                   个字节
//功能概要：发送AT指令并获取返回信息。最多发送三次命令，每次命令最多等待3秒。
//内部调用：strTrim
//修改日期：2017.06.30，WYH；2017.08.11,QHJ,CC；；2017.09.25，WB；
//=====================================================================
uint_8 uecom_sendCmd(uint_8 *cmd,uint_16 maxDelayMs,uint_16 maxTimes)
{
	//中断方式实现
    //（1）变量声明和赋初值
	uint_8 ret;
	uint_16 i,j,k,count;
	uint_8 wait[30]="OK";
	AT_haveCommand=1;  //置位AT命令标志
	uart_enable_re_int(UART_UE);//开启中断
	//决定等待的字符串，默认为"OK"
	if(!strcmp(cmd,"SEND"))
		strcpy(wait,"SEND");
	else if(strstr(cmd,"AT+CIPSTART")!=NULL)
		strcpy(wait,"CONNECT OK");
	else if(strstr(cmd,"wait")!=NULL)
		strcpy(wait,">");
	uecom_delay_ms(10);//防止串口AT命令发送过于频繁
	j = maxDelayMs/10;
	for(count=0;count<maxTimes;count++)
	{
		if(strcmp(cmd,"SEND") && strcmp(cmd,"wait") )//“SEND”用于判断发送成功
		{
			AT_reLength = 0;   //重新开始接收数据
			AT_reBuf[0]=0;
//			uart_send_string(UART_2,cmd);
			uart_send_string(UART_UE,cmd); //通过串口发送AT指令
		}
		//等待maxDelayMs毫秒
		for(i=0; i<j;i++)
		{
	        uecom_delay_ms(10);
	        if(strstr(AT_reBuf,wait))
	        {
	        	ret = 0;
	        	goto uecom_sendCmd_exit;
	        }
	        if(strstr(AT_reBuf,ERROR))
	        {
	        	uecom_delay_ms(100);//缓冲100毫秒
	        	uart_send_string(UART_UE,AT); //通过串口发送AT指令
	        	uecom_delay_ms(100);//缓冲100毫秒
	        	AT_reLength = 0;   //重新开始接收数据
	    		AT_reBuf[0]=0;
                break;
	        }
		}
		//至此，说明未接收到正确返回。
    	j*=2;//若失败，等待时间加倍
	    AT_reLength = 0;
		AT_reBuf[0]=0;
	}
	ret = 1;
	uecom_sendCmd_exit:
	AT_haveCommand=0;
	return ret;
}

//==================================================================
//函数名称：uecom_internal_getIMEI
//函数返回： 0：获取IMEI成功；1：获取IMEI失败；2：解析IMEI号失败
//参数说明：dest：存放返回的IMEI号，15位
//功能概要：获取设备IMEI号
//内部调用：uecom_sendCmd
//修改日期：2017.08.11,QHJ,CC
//==================================================================
uint_8 uecom_internal_getIMEI(uint_8 *dest)
{
	//（1）变量声明和赋初值
	uint_8 ret_val;
	uint_16 i,k;

	//（2）获取IMEI号
	dest[15] = 0;				      //字符串的结尾符
	if(uecom_sendCmd((uint_8 *)CGSN,500,2))
	{
		uecom_sendCmd((uint_8 *)AT,200,2);
		if(uecom_sendCmd((uint_8 *)CGSN,500,2))
		{
			ret_val = 1;
			goto uecom_internal_getIMEI_exit;      //获取IMEI号失败
		}
	}
	//（3）将IMEI号存储在dest数组中
	//定位IMEI的起始位置
	i = strstr(AT_reBuf,"+CGSN: ")-(char*)AT_reBuf;
	i += 7;
	k = 0;
	while ( (AT_reBuf[i] != 'O' || AT_reBuf[i+1] != 'K') && i  <  AT_reBuf_length && k<15)
	{
		if( (AT_reBuf[i] >= '0' && AT_reBuf[i] <= '9'))
		{
			dest[k++] = AT_reBuf[i++];
		}
		else
		{
			i++;
		}
	}
	if(k < 15)    //设定IMEI的最小长度为15
	{
			ret_val = 2;
			goto uecom_internal_getIMEI_exit;      //解析IMEI号失败
	}
	ret_val = 0;
	//退出处
uecom_internal_getIMEI_exit:
	return ret_val;
}

//==================================================================
//函数名称：uecom_internal_getIMSI
//函数返回：0：获取IMSI成功；1：获取IMSI失败
//参数说明：dest：存放返回的IMSI号，15位
//功能概要：获取设备IMSI号
//内部调用：uecom_sendCmd
//修改日期：2017.08.11,QHJ,CC
//==================================================================
uint_8 uecom_internal_getIMSI(uint_8 *dest)
{
	//（1）变量声明和赋初值
	uint_8 ret_val;
	int_16 i,k;

	dest[14] = 0;

	//（2）获取IMSI号
	if(uecom_sendCmd((uint_8 *)CIMI,5000,2))
	{
		uecom_sendCmd((uint_8 *)AT,200,2);
		if(uecom_sendCmd((uint_8 *)CIMI,5000,1))
		{
			ret_val = 1;
			goto uecom_internal_getIMSI_exit;     //获取IMSI号失败
		}
	}
	//（3）将IMSI号存储在dest数组中
	//定位IMSI的起始位置
	i = strstr(AT_reBuf,OK)-(char*)AT_reBuf;
	dest[15]=0;
	k=14;
	while(k>=0 && i>=0)
	{
		if( (AT_reBuf[i] >= '0' && AT_reBuf[i] <= '9'))
		{
			dest[k--] = AT_reBuf[i--];
		}
		else
		{
			i--;
		}
	}
	ret_val = 0;
	//退出处
uecom_internal_getIMSI_exit:
	return ret_val;
}


//======================================================================
//函数名称：uecom_default_delay_ms
//函数返回：无
//参数说明：无
//功能概要：延时 - 毫秒级
//======================================================================
void uecom_default_delay_ms(uint_16 mseconds)
{
  vuint_16 i;
  vuint_32 j;
  for(i = 0; i<mseconds; i++)
  {
    for(j = 0; j<4800; j++);
  }
}
//======================================================================
//函数名称：uint_to_str
//函数返回：无
//参数说明：ch:带转换的正整数		*str：存放转换后的字符串
//功能概要：将无符号整数转换为字符串
//======================================================================
void uint_to_str(uint_32 ch,char *str)
{
	int i,j,sign;
	char s[10];
	if((sign=ch)<0)//记录符号
		ch=-ch;//使n成为正数
	i=0;
	do{
	       s[i++]=ch%10+'0';//取下一个数字
	}
	while ((ch/=10)>0);//删除该数字
	if(sign<0)
	  s[i++]='-';
	for(j=i-1;j>=0;j--)//生成的数字是逆序的，所以要逆序输出
	       str[i-1-j]=s[j];
	str[i]=0;
}


//=====================================================================
//函数名称：frameEncode
//功能概要：实现待发送数据的组帧,将待发送数据加上帧头、帧长、帧尾以及校验信息
//       frame=帧头+IMSI+帧长+data+CRC校验码+帧尾
//参数说明：imsi：发送数据的设备的IMSI号
//       data:待组帧的数据头指针
//       dlen:待组帧的数据长度
//       frame:组帧之后的待发送数据帧
//       framelen:组帧后的数据长度
//函数返回：0：成功
//=====================================================================
void frameEncode(uint_8 *imsi,uint_8 *data,uint_16 dlen,uint_8 *frame,uint_16 *framelen)
{
	uint_16 crc,len;

	//组帧操作
	//帧头
	frame[0] = frameHead[0];
	frame[1] = frameHead[1];
	len = 2;
	//IMSI号
	strncpy(frame+2,imsi,15);
	len += 15;
    //帧长
	frame[len++] = dlen>>8;
	frame[len++] = dlen;
	//帧数据
	memcpy(frame+19,data,dlen);
	len += dlen;
	//CRC校验码
	crc = crc16(frame+2,dlen+17);
	frame[len++] = crc>>8;
	frame[len++] = crc;
	//帧尾
	frame[len++] = frameTail[0];
	frame[len++] = frameTail[1];
	frame[len] = 0;
	*framelen = len;
}

//=====================================================================
//函数名称：crc16
//功能概要：将数据进行16位的CRC校验，返回校验后的结果值
//参数说明：ptr:需要校验的数据缓存区
//                len:需要检验的数据长度
//函数返回：计算得到的校验值
//=====================================================================
uint_16 crc16(uint_8 *ptr,uint_16 len)
{
	uint_16 i,j,tmp,crc16;

	crc16 = 0xffff;
	for(i = 0;i < len;i++)
	{
		crc16 = ptr[i]^crc16;
		for(j = 0;j< 8;j++)
		{
			tmp = crc16 & 0x0001;
			crc16 = crc16>>1;
			if(tmp)
				crc16 = crc16^0xa001;
		}
	}
	return crc16;
}

//=====================================================================
//函数名称：uecom_recv
//函数返回：0：接收到了通过TCP/UDP发来的数据；1：正在组帧；2接收数据错误
//参数说明：ch:串口接收到的数据
//                   *dataLen:存储接收到的数据长度
//                   *data:将接收到的数据存放到该数组串中;
//功能概要：将接收到的数据进行解帧，并将有效数据存入data数组之中，由串口的uart中断调用。
//内部调用：uart_init，uart_disable_re_int,uart_re1，
//修改日期：2017.08.11,QHJ,CC；2017.09.25，WB；
//=====================================================================
uint_8 uecom_recv(uint_8 ch,uint_16 *dataLen, uint_8 *data)
{
	static uint_8 flag = 0;
	static uint_16 recvLength = 0;
	static uint_16 index = 0;
	static uint_16 length = 0;
	uint_8 ret_val;
	uint_16 i;
	uint_8 CRC[2];
	uint_16 crc;
	isGPRSdata = 0;
	switch(flag)
	{
	case 0:    if(ch=='+')flag=1; else flag = 0;break;
	case 1:    if(ch=='R')flag=2; else flag = 0;break;
	case 2:    if(ch=='E')flag=3; else flag = 0;break;
	case 3:    if(ch=='C')flag=4; else flag = 0;break;
	case 4:    if(ch=='E')flag=5; else flag = 0;break;
	case 5:    if(ch=='I')flag=6; else flag = 0;break;
	case 6:    if(ch=='V')flag=7; else flag = 0;break;
	case 7:    if(ch=='E')flag=8; else flag = 0;break;
	case 8:    if(ch==',')flag=9; else flag = 0;recvLength = 0;break;
	case 9:    if(ch==':')flag=10; else recvLength=recvLength*10+ch-'0';
	                 break;
	case 10:
		isGPRSdata = 1;
	    //组帧
		if(index>recvLength)   //超出最大值
		{
        	index = 0;
        	length = 0;
    		ret_val = 2;
    		flag = 0;//重新等待接收数据
    		isGPRSdata = 0;
    		goto uecom_recv_exit;
		}
		if((index == 0 && ch != frameHead[0]) ||
				(index == 1 && ch != frameHead[1]))//未接收到数据或者未遇到帧头
		{
	    	index = 0;
	    	length = 0;
			ret_val = 2;     //接收数据错误
			isGPRSdata = 0;
			goto uecom_recv_exit;
		}
		data[index++] = ch;//存入数据
		if(index == 19)//读取有效数据长度
		{
			length = ((uint_16)data[17]<<8) + data[18];
		}
	    if(length != 0 && index >= length+23)//接收到的数据达到一帧长度。23为帧头帧尾等长度
	    {
	    	//CRC校验
	    	crc = crc16(data+2,length+17);
	    	CRC[0] = (crc>>8)&0xff;
	    	CRC[1] = crc&0xff;

	    	if(data[index-2]!=frameTail[0] || data[index-1]!= frameTail[1]//未遇到帧尾
	    		||	CRC[0] != data[length+19] || CRC[1] != data[length+20])//CRC检验错误
	    	{
	        	index = 0;
	        	length = 0;
	    		ret_val = 2;
	    		flag = 0;//重新等待接收数据
	    		isGPRSdata = 0;
	    		goto uecom_recv_exit;
	    	}
	    	for(i=0;i<length;i++)
	    	{
	    		data[i] = data[i+19];//19为有效字节前的数据长度
	    	}
	    	*dataLen = length;
	    	index = 0;
	    	length = 0;
	    	ret_val = 0;
	    	flag = 0;//重新等待接收数据
	    	isGPRSdata = 0;
	    	goto uecom_recv_exit;
	    }
	    *dataLen = 0;//正在组帧
	    ret_val = 1;
		break;
	}
uecom_recv_exit:
    return ret_val;
}

//====备用函数API
//=====================================================================
//函数名称：uecom_getUEIP
//函数返回：0：获取基站分配UE的IP地址成功；1：获取基站分配UE的IP地址失败
//参数说明：ip：存放返回基站分配UE的IP地址，15位
//功能概要：获取基站分配UE的IP地址
//内部调用：uecom_sendCmd
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_getUEIP(uint_8 *ip)
{
	//（1）变量声明和赋初值
	uint_8 i, k;
	uint_8 ret_val;

	//（2）将ip数组中的内容清空
	for(i = 0; i<=15; i++)
	{
		ip[i] = 0;
	}
	//（3）获得模组分得的IP地址
    if(uecom_sendCmd((uint_8 *)CIFSR,200,1))
    {
    	uecom_sendCmd((uint_8 *)CGACT_1,500,2);
    	if(uecom_sendCmd((uint_8 *)CIFSR,200,1))
    	{
    		ret_val = 1;
    		goto uecom_getUEIP_exit;
    	}
    }
	//（4）读取获得的IP地址
    i = strstr(AT_reBuf,OK)-(char*)AT_reBuf;
    while(AT_reBuf[i]=='\r'||AT_reBuf[i]=='\n'  //定位i的位置
    		||(AT_reBuf[i]>='0'&&AT_reBuf[i]<='9')||AT_reBuf[i]=='.')i--;
	k = 0;   //记录接收到的ip地址的位数，防止返回OK，却没有ip地址的情况
	while(AT_reBuf[i] != 'O' && AT_reBuf[i+1] != 'K' && i <= AT_reBuf_length)
	{
		if( (AT_reBuf[i] >= '0' && AT_reBuf[i] <= '9') || (AT_reBuf[i] == '.') )
		{
			ip[k++] = AT_reBuf[i++];
		}
		else
		{
			i++;
		}
	}
	if(k == 0)
	{
		ret_val = 2;
		goto uecom_getUEIP_exit;//虽然返回OK，但是，并没有获取到IP地址
	}
	//至此，没有失败退出，成功！
	ret_val = 0;
	//退出处
uecom_getUEIP_exit:
	return ret_val;
}
