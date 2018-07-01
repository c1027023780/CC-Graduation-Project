//===================================================================
//文件名称：uecom.c
//功能概要：UE驱动构件头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2018-05-23   V2.0
//备        注：适用于M6220
//===================================================================
#include "uecom.h"

#define AT_reBuf_length 1100
static uint_8 AT_reBuf[1100];            //存放UE的AT指令的返回结果
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
	char AT[]   = "AT\r\n";                                          //测试与GPRS模块通讯的指令
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
//函数名称：uecom_switch
//函数返回： 0：操作成功；1：操作失败
//参数说明： state:UE模块供电状态。可取值为UECOM_OFF：关闭。
//        UECOM_ON：关闭。UECOM_REBOOT：重启。
//功能概要：控制通信模块供电状态
//内部调用：gpio_init，uecom_delay_ms
//修改日期：2017.09.25,WB,CC
//=====================================================================
uint_8 uecom_switch(uint_8 state)
{
	switch(state)
	{
	case UECOM_OFF:
		gpio_init(POWER_CONTROL,1,0);break;
	case UECOM_ON:
		gpio_init(POWER_CONTROL,1,1);
		uecom_delay_ms(4000);break;
	case UECOM_REBOOT:
		gpio_init(POWER_CONTROL,1,0);
		uecom_delay_ms(2000);
		gpio_init(POWER_CONTROL,1,1);
		uecom_delay_ms(4000);break;
	}
	return 0;
}

//=====================================================================
//函数名称：uecom_init
//函数返回： 0：成功；1：与UE模块串口通讯失败；3：关闭回显失败；4：IMEI查询失败；
//                    5：IMSI查询失败；11:建立TCP连接失败
//参数说明： dest：返回数据的地址，总长74字节，IMEI,15个字节，0~14；
//                    IMSI,15个字节，15~29；
//                    IP:管理服务器的IP地址
//                    PORT:管理服务器的端口号
//功能概要：uecom模块初始化。本函数在完成MCU与通讯模块的基本通讯功能之后，执行附着核心网
//        操作，并与传入的IP地址和端口号建立TCP连接。在执行本构件中的其他函数之前必须
//        调用本函数或者uecom_fast_init函数。
//内部调用：uecom_fast_init，uecom_config
//修改日期：2017.08.11,QHJ,CC；2017.09.25，WB；2018.05.23，WB；
//备        注：返回值不连续，是为了所有AHL版本的返回信息一致
//=====================================================================
uint_8 uecom_init(uint_8 *dest,uint_8 *IP,uint_8* PORT)
{
	uint_8 ret_val,flag;
	//（1）首先完成通信模块的开启和读取IMEI、IMSI和信号强度
	flag = uecom_fast_init();
	if(flag)
	{
		ret_val = flag;
		goto uecom_init_exit;
	}
	memcpy(dest,IMEI,15);      //将读取到的IMEI号存储到全局变量IMEI中
	memcpy(dest+15,IMSI,15); //将读取到的IMSI号存储到全局变量IMSI中。
	//（7）建立TCP连接
	flag = uecom_config(IP,PORT);
	if(flag)
	{
		ret_val = 10 + flag;      //建立TCP连接失败
		goto uecom_init_exit;  //建立TCP连接失败
	}
	//至此，没有失败退出，成功！
	ret_val = 0;
	//退出处
uecom_init_exit:
    return ret_val;
}

//=====================================================================
//函数名称：uecom_fast_init
//函数返回： 0：成功；1：与UE模块串口通讯失败；3：关闭回显失败；
//                    4：获取IMEI号失败；5：获取IMSI号失败；
//参数说明： 无
//功能概要：uecom模块快速初始化。本函数完成MCU与通讯模块的基本通讯功能，在执行本构件中的
//        其他函数之前必须调用本函数或者uecom_init函数。
//内部调用：uart_init，uecom_sendCmd
//修改日期：2017.09.28，WB；
//备        注：返回值不连续，是为了所有AHL版本的返回信息一致
//=====================================================================
uint_8 uecom_fast_init(void)
{
	//（1）变量声明和赋初值
	uint_16 i;
	uint_8 ret_val ;
	uint_8 flag;
	//（2）使用串口与通信模组通信，首先初始化串口
	uart_init(UART_UE, 115200);   //初始化UE模块串口，波特率为115200
	uart_enable_re_int(UART_UE);  //打开UE模块串口中断
	//（3）测试GPRS模块是否正在运行。若不运行，则重启
	if(uecom_sendCmd((uint_8 *)AT,100,4))
		uecom_switch(UECOM_REBOOT);  //给UE模块重新供电
	if(uecom_sendCmd((uint_8 *)AT,100,4))
		goto uecom_init_err1;     //UE模块通讯失败
	//（4）关闭回显
	if(uecom_sendCmd((uint_8 *)ATE_0,200,3))
		goto uecom_init_err3;     //关闭回显失败
	//（5）查询模块的IMEI,并将IMEI号放在dest数组的前15个字节
	if(uecom_internal_getIMEI(IMEI))
		goto uecom_init_err4;     //IMEI查询失败
	//（6）查询模块的IMSI,并将IMSI号放在dest数组的15-29下标的空间中
	if(uecom_internal_getIMSI(IMSI))
		goto uecom_init_err5;      //IMSI查询失败
	//至此，没有失败退出，成功！
	ret_val = 0;
	goto uecom_init_exit;      //成功，ret_val=0
	//错误退出
uecom_init_err1:
	ret_val = 1;                             //AT指令发送失败
	goto uecom_init_exit;
uecom_init_err2:
	ret_val = 2;                            //UE模块的波特率设置失败
	goto uecom_init_exit;
uecom_init_err3:
	ret_val = 3;                           //关闭回显失败
	goto uecom_init_exit;
uecom_init_err4:
	ret_val = 4;             //IMEI查询失败
	goto uecom_init_exit;
uecom_init_err5:
	ret_val = 5;              //IMSI查询失败
	goto uecom_init_exit;
	//退出处
uecom_init_exit:
    return ret_val;
}

//=====================================================================
//函数名称：uecom_deInit
//函数返回：无
//参数说明：无
//功能概要：UE反初始化。关闭串口中断，取消串口的引脚复用，并关闭UE模块的供电。
//内部调用：uart_enable_re_int，uecom_switch
//修改日期：2017.09.22,QHJ,CC；2017.09.25，WB；
//=====================================================================
void uecom_deInit(void)
{
	uart_disable_re_int(UART_UE);                   //关闭UE模块串口中断
	uecom_switch(UECOM_OFF);                   //关闭UE模块的供电
}

//=====================================================================
//函数名称：uecom_send
//函数返回：  0：发送成功；1：发送失败；2：返回超时（可能未正确开启发送模式）
//参数说明： data:待发送数据缓存区，传入参数
//		           length:待发送数据的长度
//功能概要：将数据通过已经建立的TCP/UDP通道发送出去
//内部调用：uint_to_str，uart_send_string，uecom_delay_ms，uart_sendN，
//       uart_init，uart_enable_re_int，uart_disable_re_int，
//       uart_re1，uart_send1，uart_enable_re_int
//修改日期：2017.08.11,QHJ,CC；2017.09.25，WB；
//=====================================================================
uint_8 uecom_send(uint_16 length, uint_8 *data)
{
	//（1）变量声明，赋初值
	uint_8 ch,flag,k,i,l;
	uint_8 ret_val;
	char dataLen[5]="";
	char CIPSEND[20] = "";    //设置发送的字节数
	char buf[50];
	uint_8 frame[1024];
	uint_16 frameLen;
    //（2）组帧
	frameEncode(IMSI,data, length,frame,&frameLen);
	//（3）将需要发送的数据长度发给UE模块，并开启发送模式
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

//==================================================================
//函数名称：uecom_getIMEI
//函数返回： 0：获取IMEI成功；1：获取IMEI失败
//参数说明：dest：存放返回的IMEI号，15位
//功能概要：获取设备IMEI号
//内部调用：uecom_sendCmd
//修改日期：2017.08.11,QHJ,CC
//==================================================================
uint_8 uecom_getIMEI(uint_8 *dest)
{
	int i;
	for(i=0;i<15&&IMEI[i];i++)
		dest[i]=IMEI[i];
	return 0;
}

//==================================================================
//函数名称：uecom_getIMSI
//函数返回：0：获取IMSI成功；1：获取IMSI失败
//参数说明：dest：存放返回的IMSI号，15位
//功能概要：获取设备IMSI号
//内部调用：uecom_sendCmd
//修改日期：2017.08.11,QHJ,CC
//==================================================================
uint_8 uecom_getIMSI(uint_8 *dest)
{
	int i;
	for(i=0;i<15&&IMEI[i];i++)
		dest[i]=IMSI[i];
	return 0;
}

//===================================================================
//函数名称：uecom_getFirmVer
//函数返回：0：获取固件版本号成功；1：获取固件版本号失败
//参数说明：firmVer：存放返回的固件版本号，25位
//功能概要：获取固件版本号，用于统一接口，并没有实现
//内部调用：无
//修改日期：2017.08.11,QHJ,CC
//==================================================================
uint_8 uecom_getFirmVer(uint_8 *firmVer)
{
	uint_8 i;
	for(i = 0; i<25; i++)
	{
		firmVer[i] = 0;
	}
	return 0;
}

//=====================================================================
//函数名称：uecom_getSignalPower
//函数返回：0：获取基站信号强度成功；1：获取基站信号强度失败
//参数说明：signalPower：存放返回的基站信号强度号，百分比格式
//功能概要：获取基站信号强度
//内部调用：uecom_sendCmd
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_getSignalPower(uint_8 *signalPower)
{
	//（1）变量声明和赋初值
	uint_8 i,j,ret_val;

	for(j=0;j<3;j++)
	{
		//（2）获取信号强度
		if( uecom_sendCmd((uint_8 *)CSQ,200,3))
		{
			ret_val = 1;
			goto uecom_getSignalPower_exit;
		}
		//（3）提取出信号强度，转为uint_8格式，并赋给*signalPower
		i = strstr(AT_reBuf,"+CSQ: ")-(char*)AT_reBuf;
		i+=5;
		*signalPower = 0;
		while(AT_reBuf[i]!=','&&i<AT_reLength)
		{
			if(AT_reBuf[i]>='0' && AT_reBuf[i]<='9')
				*signalPower = *signalPower*10 + (AT_reBuf[i]-'0');
			i++;
		}
		if(*signalPower!=0)
	    {
			*signalPower = (uint_16)(*signalPower)*100/31;
			break;
		}
		uecom_delay_ms(1000);
	}
	if(j==3)
		ret_val = 1;
	else
		ret_val = 0;
	//退出处
uecom_getSignalPower_exit:
	return ret_val;
}

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
	//（3）获得模块分得的IP地址
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

//=====================================================================
//函数名称：uecom_getBSNum
//函数返回：0：获取基站号成功；1：获取基站号失败
//参数说明：bsNum：存放返回的基站号，2位
//功能概要：仅用于统一接口
//内部调用：无
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_getBSNum(uint_32 *bsNum)
{
	*bsNum = 0;
	return 0;
}

//====================================================================
//函数名称：uecom_getCurCommunity
//函数返回：0：获取当前小区成功；1：获取当前小区失败
//参数说明：data：存放返回的小区描述信息。格式为:mcc(移动国家号码),mnc(移动网络号码),
//                   lac(位置区域码),cell id(小区号),bsic(基站标示码)。例如:460,000,5280,e467,32
//                   注：其中lac和cell id为十六进制表示。data数组最少占25个字节
//功能概要：获得当前的小区信息
//修改日期：【2018-03-09】增（WB）
//=====================================================================
uint_8 uecom_getCurCommunity(uint_8 *data)
{
    //（1）声明临时变量
	uint_8 i, j,*p,ret;
	//（2）发送获取小区信息命令（最多发三次）
	if(uecom_sendCmd(CCED_1,200,3))
	{
		ret = 1;              //返回值设为1
		goto uecom_getCurCommunity_exit;   //退出函数
	}
    //（3）解析返回的数组，并存入data数组中
    p = strstr(AT_reBuf,"+CCED:");   //获得字符':'所在的位置，其后为所需的数据。
    p += 6;
    j = 0;
    i = 0;
    //取第5个逗号之前的数据并存入data数组中
    while(i<5)
    {
        if(*p == ',')
            i++;
        data[j++] = *p;
        p++;
    }
    if(j<=0)  //未能成功解析到数据
    {
		ret = 2;              //返回值设为2
		goto uecom_getCurCommunity_exit;   //退出函数
    }
    //最后一个逗号删除，并设置为字符串结束符
    data[j-1]=0;
    ret = 0;          //返回值设为0
    //（4）退出函数
    uecom_getCurCommunity_exit:
    return ret;
}

//====================================================================
//函数名称：http_get
//函数返回：0：获得get请求成功；1：初始化http失败；2：传递url参数失败；3：设置网络失败；
//                   4：开启网络失败；5：建立连接失败；6：发送请求失败；7：获得返回失败；
//参数说明：data：存放返回的小区描述信息。格式为:mcc(移动国家号码),mnc(移动网络号码),
//                   lac(位置区域码),cell id(小区号),bsic(基站标示码)。例如:460,000,5280,e467,32
//                   注：其中lac和cell id为十六进制表示。data数组最少占25个字节
//功能概要：获得当前的小区信息
//修改日期：【2018-03-09】增（WB）
//=====================================================================
uint_8 http_get(uint_8 ip[],uint_8 port[],uint_8 url[],uint_8 result[])
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
	if(uecom_config(ip, port))
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

//====================================================================
//函数名称：uecom_interrupt
//函数返回：无
//参数说明：ch：串口中断接收到的数据；length：接收到的gprs数据长度；
//                   recvData：存储接收到的gprs数据
//功能概要：本函数需要放在串口中断中。并需要传入串口中断接收到的数据。本构件的功能实现依赖
//                   于本函数。
//修改日期：【2018-03-09】增（WB）
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
//	else
    uecom_recv(ch,length,recvData); //处理GPRS接收到的数据
}

//=====================================================================
//函数名称：gnss_init
//函数返回： 无
//参数说明： 无
//功能概要：控制通信模块供电状态
//修改日期：2018.03.05,WB
//=====================================================================
void gnss_init(uint_8 way)
{
	//（1）使用串口与GNSS模组通信，首先初始化串口
//	uart_init(UART_UE, 115200);   //初始化UE模块串口，波特率为115200
//	uart_enable_re_int(UART_UE);  //打开UE模块串口中断
    //（2）打开gnss电源
	if(uecom_sendCmd(GPSACT_1,1000,2))
	{
		goto gnss_init_exit;
	}
	uecom_delay_ms(500);    //延时500ms开启GNSS模块
	//（3）设置冷启动GNSS模块
	switch(way)
	{
	case 1:uecom_sendCmd(GPSRESTART_1,500,2);break;
	case 2:uecom_sendCmd(GPSRESTART_2,500,2);break;
	case 3:uecom_sendCmd(GPSRESTART_3,500,2);break;
	default:break;
	}
	gnss_init_exit:
	return;
}

//=====================================================================
//函数名称：gnss_get_time
//函数返回：0：获得UTC时间成功；1：发送AT指令失败；2：获得UTC时间失败。
//参数说明： 初始为不少于14字节的字符串，存储当前的UTC时间，格式为：年月日时分秒。
//                    如：20180305055948。本地为东八区，小时应该+8
//功能概要：获得当前的UTC时间
//修改日期：2018.03.05,WB
//=====================================================================
uint_8 gnss_get_time(char * time)
{
	//（1）定义本函数使用的变量
	uint_8 i,j,retVal,index;
	double a,b,c,d;
	//（2）发送AT指令获得最小定位信息。最多尝试3次
	if(uecom_sendCmd(GPSRMC,500,2))
	{
		gnss_init(4);//重新初始化
		if(uecom_sendCmd(GPSRMC,500,3))
		{
			retVal = 1;
			goto gnss_get_time_error;
		}
	}
	//（3）若未获得正确时间则退出
    if(strstr((char*)AT_reBuf,"20000101000000"))
    {
    	retVal = 2;
    	goto gnss_get_time_error;
    }
    //（4）提取出时间信息
    i=0;
    j=0;
    while(i<14 && j<30)
    {
    	if(AT_reBuf[j]>='0' &&  AT_reBuf[j] <= '9')
    		time[i++] = AT_reBuf[j];
    	j++;
    }
    retVal = 0;
gnss_get_time_error:
    return retVal;
}

//=====================================================================
//函数名称：gnss_get_location
//函数返回：0：获得位置成功；1：发送AT指令失败；2：获得位置失败。
//参数说明： latitude ：存储解析出的纬度信息（double类型）；
//                    longitude：存储解析出的经度信息（double类型）；
//功能概要：获得当前所在的位置
//修改日期：2018.03.05,WB
//=====================================================================
uint_8 gnss_get_location(double *latitude, double *longitude)
{
	//（1）定义本函数使用的变量
	uint_8 i,j,retVal,index;
	double a,b,c,d;
	//（2）发送AT指令获得最小定位信息。最多尝试3次
	if(uecom_sendCmd(GPSRMC,500,2))
	{
		gnss_init(4);//重新初始化
		if(uecom_sendCmd(GPSRMC,500,3))
		{
			retVal = 1;
			goto gnss_get_location_error;
		}
	}
	//（3）若未获得有效数据则退出
	i = strstr(AT_reBuf,"+GPSRMC:")-(char*)AT_reBuf;
	i+=24;
    if(AT_reBuf[i]!='A')
    {
    	retVal = 2;
    	goto gnss_get_location_error;
    }
    //（4）提取出位置信息（根据AT命令返回结果获取）
    //（4.1）首先提取纬度信息，并存入latitude变量中
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
	*latitude = (a + b/60)*d;
	//（4.2）首先提取经度信息，并存入longitude变量中
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
	*longitude = (a + b/60)*d;
    retVal = 0;
gnss_get_location_error:
    return retVal;
}

//=====================================================================
//函数名称：gnss_get_speed
//函数返回：0：获得速度成功；1：发送AT指令失败；2：获得速度失败。
//参数说明： 初始为不少于10字节的字符串，存储当前的速度。单位为：节（1节=0.5144444米/秒）
//功能概要：获得当前的速度
//修改日期：2018.03.05,WB
//=====================================================================
uint_8 gnss_get_speed(double * speed)
{
	//（1）定义本函数使用的变量
	uint_8 i,j,retVal,index;
	double a,b,c,d;
	//（2）发送AT指令获得最小定位信息。最多尝试3次
	if(uecom_sendCmd(GPSRMC,500,2))
	{
		gnss_init(4);//重新初始化
		if(uecom_sendCmd(GPSRMC,500,3))
		{
			retVal = 1;
			goto gnss_get_speed_error;
		}

	}
	//（3）若未获得有效数据则退出
	i = strstr(AT_reBuf,"+GPSRMC:")-(char*)AT_reBuf;
	i+=24;
    if(AT_reBuf[i]!='A')
    {
    	retVal = 2;
    	goto gnss_get_speed_error;
    }
    //（4）提取出速度信息（根据AT命令返回结果获取）
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
	* speed = a*0.5144444;
    retVal = 0;
gnss_get_speed_error:
    return retVal;
}

//=====================================================================
//函数名称：gnss_get_attitude
//函数返回：0：获得海拔成功；1：发送AT指令失败；2：获得海拔失败。
//参数说明： 初始为不少于10字节的字符串，存储当前的海拔高度。单位为：米
//功能概要：获得当前的海拔高度
//修改日期：2018.03.05,WB
//=====================================================================
uint_8 gnss_get_attitude(double *attitude)
{
	//（1）定义本函数使用的变量
	uint_8 i,j,retVal,index;
	double a,b,c,d;
	//（2）发送AT指令获得最小定位信息。最多尝试3次
	if(uecom_sendCmd(GPSRMC,500,2))
	{
		gnss_init(4);//重新初始化
		if(uecom_sendCmd(GPSRMC,500,3))
		{
			retVal = 1;
			goto gnss_get_attitude_error;
		}
	}
	//（3）若未获得有效数据则退出
	i = strstr(AT_reBuf,"+GPSRMC:")-(char*)AT_reBuf;
	i+=24;
    if(AT_reBuf[i]!='A')
    {
    	retVal = 2;
    	goto gnss_get_attitude_error;
    }
    //（4）提取出海拔信息（根据AT命令返回结果获取）
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
	*attitude = a;
    retVal = 0;
    gnss_get_attitude_error:
    return retVal;
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
	while(AT_haveCommand!=1)
	{
		AT_haveCommand=1;  //置位AT命令标志
		uecom_delay_ms(1);
	}
    AT_reLength = 0;
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
		AT_reBuf[0]=0;
		if(strcmp(cmd,"SEND") && strcmp(cmd,"wait") )//“SEND”用于判断发送成功
		{
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
	        	break;
	        }
		}
		//至此，说明未接收到正确返回。
    	j*=2;//若失败，等待时间加倍
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

//=====================================================================
//函数名称：uecom_config
//函数返回：  0：成功建立TCP连接；
//       1：建立TCP连接失败
//参数说明： IP:待连接服务器的IP地址
//        port:待连接服务器的端口号
//功能概要：与指定的服务器端口建立TCP连接
//内部调用：uecom_sendCmd，uart_send_string，uecom_delay_ms
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_config(uint_8 *ip,uint_8* port)
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
			ret_val = 1;
			goto uecom_config_exit;
		}
	}
	ret_val = 0;
	//退出处
uecom_config_exit:
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
