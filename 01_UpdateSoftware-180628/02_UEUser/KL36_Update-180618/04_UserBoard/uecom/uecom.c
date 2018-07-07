//===================================================================
//文件名称：uecom.c（uecom驱动源程序文件）
//===================================================================
#include "uecom.h"    //包含对应驱动头文件，说明在头文件中

//文件级变量
static uint_8 AT_reBuf[512];     //存放UE的AT指令的返回结果
static uint_8 AT_reFlag = 0;      //AT指令成功返回的标志
static uint_8 AT_reLength = 0;      //AT指令返回的数据长度
static uint_8 curATCommand[100] = {0};  //存储当前的AT命令
static uint_8 ATSendFlag;                    //标记一个AT指令已发送
static uint_8 frameHead[2] = "V!";    //帧头
static uint_8 frameTail[2] = "S$";        //帧尾
static uint_8 IMSI[16]={0};                   //存储初始化后读取到的IMSI号
static uint_8 IMEI[16]={0};                  //存储初始化后读取到的IMEI号
//对外提供服务的变量，在头文件中声明，此处定义
uint_16 gRecvLength = 0;        //接收到的GPRS数据，为0时表示没有接收到，否则表示接收到的数据长度
uint_8   gRecvBuf[600];          //存储接收到的GPRS数据，最大1000个字节（因为帧结构占掉约24个字节）


//======================内部函数声明====================================
uint_8 uecom_sendCmd(uint_8 *cmd,uint_8 buf[],uint_16 maxDelayMs);
uint_8 uecom_link(uint_8 *dest);
uint_8 uecom_config(uint_8 *ip,uint_8* port);
//void delay_ms(uint_16 mseconds);
void uint_to_str(uint_32 ch,char *str);
void frameEncode(uint_8 *imsi,uint_8 *data,uint_16 dlen,
		         uint_8 *frame,uint_16 *framelen);
uint_8 frameDecode(uint_8 *frame,uint_8 *cdata,uint_16 *len);
uint_16 crc16(uint_8 *ptr,uint_16 len);
uint_8 uecom_internal_getIMEI(uint_8 *dest);
uint_8 uecom_internal_getIMSI(uint_8 *dest);
extern void uecom_interrupt(void);
//======================内部函数声明结束==================================

//========================变量声明======================================
    //函数中使用到的AT指令
    char AT[]   = "AT\r\n";               //测试与GPRS模块通讯的指令
    char IPR[] = "AT+IPR=115200\r\n";     //将模块波特率设置为115200
    char CGSN[] = "AT+CGSN\r\n";          //获取设备的IMEI号指令
    char CGCLASS_B[] ="AT+CGCLASS=\"B\"\r\n";  //将卡的类型设置为B型
    char CGDCONT[] =                           //将上网类型设置为net，可以上外网
            "AT+CGDCONT=1, \"IP\", \"CMNET\"\r\n";
    char CGACT_1[] ="AT+CGACT=1\r\n";      //激活PDP，并向基站请求分配ip地址
    char CGACT_0[] ="AT+CGACT=0\r\n";      //关闭PDP
    char QILOCIP[] ="AT+QILOCIP\r\n";      //获取分配到的IP地址
    char QICLOSE[] = "AT+QICLOSE\r\n";
    char CIPCLOSE_0[] = "AT+CIPCLOSE=0\r\n";
    char CIPCLOSE_1[] = "AT+CIPCLOSE=1\r\n";
    char CIPCLOSE_2[] = "AT+CIPCLOSE=2\r\n";
    char CIMI[] = "AT+CIMI\r\n";       //获取设备的IMSI号指令
    char CCED_1[] = "AT+CCED=0,1\r\n";  //获取当前小区信息【2018-03-09】增
    char ATE_0[] = "ATE0\r\n";          //关闭回显
    char CSQ[] = "AT+CSQ\r\n";          //获得信号强度
    //AT命令是否执行成功的标志
    char OK[] = "OK";
    char ERROR[] = "ERROR";
//========================变量声明结束====================================

//根据宏UART_UE决定使用的中断函数
#if UART_UE == UART_1
void UART1_IRQHandler(void)
{
	uecom_interrupt();
}
#elif UART_UE == UART_2
void UART2_IRQHandler(void)
{
	uecom_interrupt();
}
#elif UART_UE == UART_3
void UART3_IRQHandler(void)
{
	uecom_interrupt();
}
#elif UART_UE == UART_4
void UART4_IRQHandler(void)
{
	uecom_interrupt();
}
#elif UART_UE == UART_5
void UART5_IRQHandler(void)
{
	uecom_interrupt();
}
#elif UART_UE == UART_6
void UART6_IRQHandler(void)
{
	uecom_interrupt();
}
#endif

//====================================================================
//函数名称：uecom_switch
//函数返回： 0：操作成功；1：操作失败
//参数说明： state:UE模块供电状态。可取值为UECOM_OFF：关闭。
//        UECOM_ON：关闭。UECOM_REBOOT：重启。
//功能概要：控制通信模块供电状态
//内部调用：gpio_init，Delay_ms
//修改日期：2017.09.25,WB,CC
//====================================================================
uint_8 uecom_switch(uint_8 state)
{
    switch(state)
    {
    case UECOM_OFF:
        gpio_init(POWER_CONTROL,1,0);
        Delay_ms(1000);break;
    case UECOM_ON:
        gpio_init(POWER_CONTROL,1,1);
        Delay_ms(1000);break;
    case UECOM_REBOOT:
        gpio_init(POWER_CONTROL,1,0);
        Delay_ms(2000);
        gpio_init(POWER_CONTROL,1,1);
        Delay_ms(10000);break;
        uart_enable_re_int(UART_1);
    }
    return 0;
}

//====================================================================
//函数名称：uecom_init
//函数返回： 0：成功；1：与UE模块串口通讯失败；2：UE模块的波特率设置失败；3：关闭回显失败；
//        4：IMEI查询失败；5：IMSI查询失败；6：设置模块的工作类型为B类失败；
//        7：设置联网方式为net失败；8：激活PDP失败；9：获得模块分得的IP地址失败；
//        10：AT指令返回OK，但是并没有获取到IP地址；11:建立TCP连接失败
//参数说明： dest：返回数据的地址，总长74字节，IMEI,15个字节，0~14；
//             IMSI,15个字节，15~29；firmVer，25个字节，30~54；
//             signalPower，2个字节，55~56；bsNum，2个字节，57~58；
//             UEIP，15个字节，59~73
//       IP:管理服务器的IP地址
//       PORT:管理服务器的端口号
//功能概要：uecom模块初始化。本函数在完成MCU与通讯模块的基本通讯功能之后，执行附着核心网
//        操作，并与传入的IP地址和端口号建立TCP连接。在执行本构件中的其他函数之前必须
//        调用本函数或者uecom_fast_init函数。
//内部调用：uart_init，uecom_sendCmd，uecom_internal_getIMEI，
//       uecom_internal_getIMSI， uecom_link，uecom_config
//修改日期：2017.08.11,QHJ,CC；2017.09.25，WB；
//====================================================================
uint_8 uecom_init(uint_8 *dest,uint_8 *IP,uint_8* PORT)
{
    //（1）变量声明和赋初值
    uint_16 i;
    uint_8 ret_val ;
    uint_8 flag;
    //（2）使用串口与通信模组通信，首先初始化串口
    uart_init(UART_UE, 115200);   //初始化UE模块串口，波特率为115200
    uart_enable_re_int(UART_UE);  //打开UE模块串口中断
    uecom_switch(UECOM_REBOOT);   //给UE模块重新供电
    //（3）测试与GPRS模块的通讯
    if(uecom_sendCmd((uint_8 *)AT,AT_reBuf,1000)
        	&& uecom_sendCmd((uint_8 *)AT,AT_reBuf,2000)
    	    && uecom_sendCmd((uint_8 *)AT,AT_reBuf,2000))
            goto uecom_init_err1;     //UE模块通讯失败
    //（4）设置UE模块的波特率为115200
    if(uecom_sendCmd((uint_8 *)IPR,AT_reBuf,1000))
        goto uecom_init_err2;     //UE模块的波特率设置失败
    //（5）关闭回显（发送AT命令之后，UE模块不会把原来的命令返回）
    if(uecom_sendCmd((uint_8 *)ATE_0,AT_reBuf,1000))
        goto uecom_init_err3;     //关闭回显失败
    //（6）查询模块的IMEI,并将IMEI号放在dest数组的前15个字节
    if(uecom_internal_getIMEI(dest))
        goto uecom_init_err4;     //IMEI查询失败
    memcpy(IMEI,dest,15);      //将读取到的IMEI号存储到全局变量IMEI中
    //（7）查询模块的IMSI,并将IMSI号放在dest数组的15-29下标的空间中
    if(uecom_internal_getIMSI(dest+15))
        goto uecom_init_err5;      //IMSI查询失败
    memcpy(IMSI,dest+15,15); //将读取到的IMSI号存储到全局变量IMSI中。
    //（8）向基站请求联网，附着核心网
    flag = uecom_link(dest+30);
    if(flag) goto uecom_init_err6;  //附着核心网失败
    //（9）建立TCP连接
    flag = uecom_config(IP,PORT);
    if(flag) goto uecom_init_err7;  //建立TCP连接失败

    //至此，没有失败退出，成功！
    ret_val = 0;
    goto uecom_init_exit;      //成功，ret_val=0

     //错误退出
uecom_init_err1:
    ret_val = 1;             //AT指令发送失败
    goto uecom_init_exit;
uecom_init_err2:
    ret_val = 2;             //UE模块的波特率设置失败
    goto uecom_init_exit;
uecom_init_err3:
    ret_val = 3;             //关闭回显失败
    goto uecom_init_exit;
uecom_init_err4:
    ret_val = 4;             //IMEI查询失败
    goto uecom_init_exit;
uecom_init_err5:
    ret_val = 5;              //IMSI查询失败
    goto uecom_init_exit;
uecom_init_err6:
    ret_val = 5 + flag;       //附着核心网失败
    goto uecom_init_exit;
uecom_init_err7:
    ret_val = 10 + flag;      //建立TCP连接失败
    goto uecom_init_exit;
    //退出处
uecom_init_exit:
    return ret_val;
}

//====================================================================
//函数名称：uecom_fast_init
//函数返回： 0：成功；1：与UE模块串口通讯失败；2：UE模块的波特率设置失败；3：关闭回显失败；
//参数说明： 无
//功能概要：uecom模块快速初始化。本函数完成MCU与通讯模块的基本通讯功能，在执行本构件中的
//        其他函数之前必须调用本函数或者uecom_init函数。
//内部调用：uart_init，uecom_sendCmd
//修改日期：2017.09.28，WB；
//====================================================================
uint_8 uecom_fast_init(void)
{
    //（1）变量声明和赋初值
    uint_16 i;
    uint_8 ret_val ;
    uint_8 flag;
    //（2）使用串口与通信模组通信，首先初始化串口
    uart_init(UART_UE, 115200);             //初始化UE模块串口，波特率为115200
    uart_enable_re_int(UART_UE);         //打开UE模块串口中断
    uecom_switch(UECOM_REBOOT);  //给UE模块重新供电
    //（3）测试与GPRS模块的通讯
    if(uecom_sendCmd((uint_8 *)AT,AT_reBuf,1000)
    	&& uecom_sendCmd((uint_8 *)AT,AT_reBuf,1000))
            goto uecom_init_err1;     //UE模块通讯失败
    //（4）设置UE模块的波特率为115200
    if(uecom_sendCmd((uint_8 *)IPR,AT_reBuf,1000))
        goto uecom_init_err2;     //UE模块的波特率设置失败
    //（5）关闭回显（发送AT命令之后，UE模块不会把原来的命令返回）
    if(uecom_sendCmd((uint_8 *)ATE_0,AT_reBuf,1000))
        goto uecom_init_err3;     //关闭回显失败
    //（6）查询模块的IMEI,并将IMEI号放在dest数组的前15个字节
    if(uecom_internal_getIMEI(IMEI))
        goto uecom_init_err4;     //IMEI查询失败
    IMEI[15] = 0;
    //（7）查询模块的IMSI,并将IMSI号放在dest数组的15-29下标的空间中
    if(uecom_internal_getIMSI(IMSI))
        goto uecom_init_err5;      //IMSI查询失败
    IMSI[15] = 0;
    ret_val = 0;
    goto uecom_init_exit;
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

//====================================================================
//函数名称：uecom_deInit
//函数返回：无
//参数说明：无
//功能概要：UE反初始化。关闭串口中断，取消串口的引脚复用，并关闭UE模块的供电。
//内部调用：uart_enable_re_int，uecom_switch
//修改日期：2017.09.22,QHJ,CC；2017.09.25，WB；
//====================================================================
void uecom_deInit(void)
{
    uart_disable_re_int(UART_UE);      //关闭UE模块串口中断
    PORTE_PCR0 = PORT_PCR_MUX(0x0);    //关闭UART1_TXD
    PORTE_PCR1 = PORT_PCR_MUX(0x0);    //关闭UART1_RXD
    uecom_switch(UECOM_OFF);           //关闭UE模块的供电
}

//====================================================================
//函数名称：uecom_send
//函数返回：  0：发送成功；1：发送失败；2：返回超时（可能未正确开启发送模式）
//参数说明： data:待发送数据缓存区，传入参数
//                   length:待发送数据的长度
//功能概要：将数据通过已经建立的TCP/UDP通道发送出去
//内部调用：uint_to_str，uart_send_string，Delay_ms，uart_sendN，
//       uart_init，uart_enable_re_int，uart_disable_re_int，
//       uart_re1，uart_send1，uart_enable_re_int
//修改日期：2017.08.11,QHJ,CC；2017.09.25，WB；
//====================================================================
uint_8 uecom_send(uint_16 length, uint_8 *data)
{
    //（1）变量声明，赋初值
    uint_8 ch,flag,k,l;
    uint_8 ret_val;
    char dataLen[5]="";
    char QISEND[20] = "";   //设置发送的字节数
    char buf[50];
    uint_8 frame[1024];
    uint_16 frameLen;
    frameEncode(IMSI,data, length,frame,&frameLen);
    //（3）将需要发送的数据长度发给UE模块，并开启发送模式
    uint_to_str(frameLen,dataLen);  //将待发送数据的长度转换为字符串格式
    strcat(QISEND,"AT+QISEND=");    //获取启动发送模式的AT指令字符串
    strcat(QISEND, dataLen);
    strcat(QISEND, "\r\n");
    uart_send_string(UART_UE, QISEND);         //指定发送的数据字节个数，并进入发送模式
    Delay_ms(150);                                             //等待发送模式成功开启
    uart_sendN(UART_UE,frameLen,frame);  //发送数据
    if(uecom_sendCmd("\r\n",AT_reBuf,5000))
    {
        ret_val = 1;
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
    if(IMEI[0]==0)
    	return 1;
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
    if(IMSI[0]==0)
    	return 1;
    for(i=0;i<15&&IMSI[i];i++)
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
    uint_8 i,ret_val;
    //（2）获取信号强度
    if( uecom_sendCmd((uint_8 *)CSQ, AT_reBuf,3000))
    {
        ret_val = 1;
        goto uecom_getSignalPower_exit;
    }

    //（3）提取出信号强度，转为int_16格式，并赋给*signalPower
    i =0;
    while(AT_reBuf[i++] != ':')
    {
        if(i >= 200)
        {
            ret_val = 1;
            goto uecom_getSignalPower_exit;
        }
    }
    i++;//略过空格
    if(AT_reBuf[i] >= '0' && AT_reBuf[i] <= '9')
    {
        *signalPower = (AT_reBuf[i]-'0');
        i++;
    }
    if(AT_reBuf[i] >= '0' && AT_reBuf[i] <= '9')
    {
        *signalPower = *signalPower*10 + (AT_reBuf[i]-'0');
        *signalPower = (*signalPower)*100/31;
    }
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
    if(uecom_sendCmd((uint_8 *)QILOCIP,AT_reBuf,2000))
        goto uecom_getUEIP_err1;

    //（4）读取获得的IP地址
    i = 0;
    k = 0;   //记录接收到的ip地址的位数，防止返回OK，却没有ip地址的情况

    while(AT_reBuf[i] != 'O' && AT_reBuf[i+1] != 'K' && i <= 200)
    {
        if( (AT_reBuf[i] >= '0' && AT_reBuf[i] <= '9') ||
        		(AT_reBuf[i] == '.') )
        {
            ip[k++] = AT_reBuf[i++];
        }
        else
        {
            i++;
        }
    }
    if(k == 0)
        goto uecom_getUEIP_err2;    //虽然返回OK，但是，并没有获取到IP地址
    //至此，没有失败退出，成功！
    ret_val = 0;
    goto uecom_getUEIP_exit;        //成功，ret_val=0

uecom_getUEIP_err1:
    ret_val = 1;
    goto uecom_getUEIP_exit;//获得模块分得的IP地址失败
uecom_getUEIP_err2:
    ret_val = 2;
    goto uecom_getUEIP_exit;//AT指令返回OK，但是并没有获取到IP地址
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
//====================================================================
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
//修改日期：【2018-03-09】增（WB），
//=====================================================================
uint_8 uecom_getCurCommunity(uint_8 *data)
{
    //（1）声明临时变量
	uint_8 i, j,*p,ret;
	//（2）发送获取小区信息命令（最多发两次）
	 if(uecom_sendCmd(CCED_1,AT_reBuf,3000)
			 && uecom_sendCmd(CCED_1,AT_reBuf,3000))
	 {
	    	//清空data数组，表明未接收到数据
	    	for(i=0;i<25;i++)
	    		data[i]=0;
	        ret = 1;              //返回值设为1
		    goto uecom_getCurCommunity_exit;   //退出函数
	 }
    //（4）解析返回的数组，并存入data数组中
    p = strstr(AT_reBuf,":");   //获得字符':'所在的位置，其后为所需的数据。
    i = 0;
    j = 0;
    //取第5个逗号之前的数据并存入data数组中
    while(i<5)
    {
        p++;
        if(*p == ',')
            i++;
        data[j++] = *p;
    }
    //最后一个逗号删除，并设置为字符串结束符
    data[j-1]=0;
    ret = 0;          //返回值设为0
    //（5）退出函数
    uecom_getCurCommunity_exit:
    return ret;
}

//------以下为本构件调用的内部函数--------------------------------------------
//====================================================================
//函数名称：uecom_sendCmd
//函数返回：0：指令发送成功;1：指令发送失败
//参数说明：cmd：需要发送的AT指令的首地址;buf[]:传地址，带回AT指令返回结果,最多需要200
//        个字节
//功能概要：发送AT指令并获取返回信息。最多发送三次命令，每次命令最多等待3秒。
//内部调用：strTrim
//修改日期：2017.06.30，WYH；2017.08.11,QHJ,CC；；2017.09.25，WB；
//====================================================================
uint_8 uecom_sendCmd(uint_8 *cmd,uint_8 buf[],uint_16 maxDelayMs)
{
    //（1）变量声明和赋初值
	uint_8 ret;
	uint_16 i,j;
	strcpy(curATCommand,cmd);   //保存命令至当前命令
	uart_send_string(UART_UE,cmd); //通过串口发送AT指令
	j = maxDelayMs/100;
	for(i=0; i<j;i++)
	{
        if(AT_reFlag)
        	break;
        Delay_ms(100);
	}
	if(i == j)
	{
		ret = 1;
		goto uecom_sendCmd_exit;
	}
	ret = 0;
	uecom_sendCmd_exit:
	Delay_ms(100);      //防止命令发送过于频繁
	AT_reLength = 0;
	AT_reFlag = 0;
	curATCommand[0]=0;
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
    uint_8 i,k;

    //（2）获取IMEI号
    dest[15] = 0;        //字符串的结尾符
    if(uecom_sendCmd((uint_8 *)CGSN,AT_reBuf,1000)
    		&& uecom_sendCmd((uint_8 *)CGSN,AT_reBuf,1000))
    {
		ret_val = 1;
		goto uecom_internal_getIMEI_exit;  //获取IMEI号失败
    }
    //（3）将IMEI号存储在dest数组中
    i = 0;
    k = 0;
    while(AT_reBuf[i]!='\n'&&i++<200);
    if(i>=200)
    {
        ret_val = 2;
        goto uecom_internal_getIMEI_exit;     //解析IMEI号失败
    }
    i=0;
    while ( (AT_reBuf[i] != 'O' || AT_reBuf[i+1] != 'K')
            && i <= 200 && k<15)
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
    uint_8 i,k;

    dest[14] = 0;

    //（2）获取IMSI号
	if(uecom_sendCmd((uint_8 *)CIMI,AT_reBuf,2000)
			&& uecom_sendCmd((uint_8 *)CIMI,AT_reBuf,2000))
	{
			ret_val = 1;
			goto uecom_internal_getIMSI_exit;  //获取IMSI号失败
	}
    //（3）将IMSI号存储在dest数组中
    i = 0;
    k = 0;
    i = 0;
    while(AT_reBuf[i] != 'O' && AT_reBuf[i+1] != 'K' &&
            i <= 200 && k<15)
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
    ret_val = 0;
    //退出处
uecom_internal_getIMSI_exit:
    return ret_val;
}

//=====================================================================
//函数名称：uecom_link
//函数返回：0：成功与基站建立连接；
//      1：设置模块的工作类型为B类失败；2：设置联网方式为net失败；
//      3：激活PDP失败；
//      4：获得模块分得的IP地址失败；5：AT指令返回OK，但是并没有获取
//         到IP地址
//参数说明：*dest：存放基站给本模块的ip地址，格式为字符串，中间用"."
//         连接，dest指向的数组至少要有15个字节
//功能概要：建立与基站的连接
//内部调用：uecom_sendCmd，Delay_ms，uart_send_string，uecom_getUEIP
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_link(uint_8 *dest)
{
    //（1）变量声明、赋初值
    uint_8 i,ret_val,flag;

    //（2）设置模块的工作类型为B类
    if(uecom_sendCmd((uint_8 *)CGCLASS_B,AT_reBuf,1000)
    		&& uecom_sendCmd((uint_8 *)CGCLASS_B,AT_reBuf,1000))
        goto uecom_link_err1;      //设置模块的工作类型为B类失败
    //（3）设置联网方式为net
    if(uecom_sendCmd((uint_8 *)CGDCONT,AT_reBuf,1000)
    		&& uecom_sendCmd((uint_8 *)CGDCONT,AT_reBuf,1000))
        goto uecom_link_err2;      //设置联网方式为net失败
    //（4）激活PDP(最多2次)，并向基站请求分配ip地址
    if(uecom_sendCmd((uint_8 *)CGACT_1,AT_reBuf,4000))
    {
    	uecom_sendCmd((uint_8 *)CGACT_0,AT_reBuf,1000);
    	Delay_ms(500);
        if(uecom_sendCmd((uint_8 *)CGACT_1,AT_reBuf,4000))
            goto uecom_link_err3;//激活PDP失败
    }
    //（5）获得模块分得的IP地址
    flag = uecom_getUEIP(dest);
    if(flag) goto uecom_link_err4;//获得模块分得的IP地址失败

    //至此，没有失败退出，成功！
    ret_val = 0;
    goto uecom_link_exit;
    //错误退出
uecom_link_err1:
    ret_val = 1;            //设置模块的工作类型为B类失败
    goto uecom_link_exit;
uecom_link_err2:
    ret_val = 2;             //设置联网方式为net失败
    goto uecom_link_exit;
uecom_link_err3:
    ret_val = 3;             //激活PDP失败
    goto uecom_link_exit;
uecom_link_err4:
    ret_val = 3 + flag;      //获得模块分得的IP地址失败
    goto uecom_link_exit;
    //退出处
uecom_link_exit:
    return ret_val;
}

//=====================================================================
//函数名称：uecom_config
//函数返回：  0：成功建立TCP连接；
//       1：建立TCP连接失败
//参数说明： IP:待连接服务器的IP地址
//        port:待连接服务器的端口号
//功能概要：与指定的服务器端口建立TCP连接
//内部调用：uecom_sendCmd，uart_send_string，Delay_ms
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_config(uint_8 *ip,uint_8* port)
{
    //（1）变量声明和赋初值
    uint_8 i;
    uint_8 flag;
    uint_8 ret_val;
    uint_8 QIOPEN[50] = "";    //建立TCP连接的地址和端口
    uint_8 loc;
    //（2）组建AT指令
    strcat((char *)QIOPEN,"AT+QIOPEN=\"TCP\",\"");
    loc = strlen(QIOPEN);
    for(i=0; i<15; i++)
    {
        if(ip[i] == 0)
        {
            break;
        }
        QIOPEN[loc+i] = ip[i];
    }
    strcat((char *)QIOPEN,"\",\"");
    loc = strlen(QIOPEN);
    //ip地址最多5位
    for(i=0; i<5; i++)
    {
        if(port[i] == 0)
        {
            break;
        }
        QIOPEN[loc+i] = port[i];
    }
    strcat((char *)QIOPEN,"\"\r\n");
    //（3）建立TCP连接
    if(uecom_sendCmd(QIOPEN,AT_reBuf,4000))
    {
		uart_send_string(UART_UE, QICLOSE);
		Delay_ms(500);
		uart_send_string(UART_UE, CIPCLOSE_0);
		Delay_ms(500);
		uart_send_string(UART_UE, CIPCLOSE_1);
		Delay_ms(500);
		uart_send_string(UART_UE, CIPCLOSE_2);
		Delay_ms(500);
	    if(uecom_sendCmd(QIOPEN,AT_reBuf,4000))
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

//====================================================================
//函数名称：delay_ms
//函数返回：无
//参数说明：无
//功能概要：延时 - 毫秒级
//====================================================================
//void delay_ms(uint_16 mseconds)
//{
//  vuint_16 i;
//  vuint_32 j;
//  for(i = 0; i<mseconds; i++)
//  {
//    for(j = 0; j<4800; j++);
////   #if ENABLE_WDOG == 1
////		if(mseconds%30==0)
////			wdog_feed();           //【看门狗喂狗】
////	#endif
//  }
//}

//====================================================================
//函数名称：uint_to_str
//函数返回：无
//参数说明：ch:带转换的正整数        *str：存放转换后的字符串
//功能概要：将无符号整数转换为字符串
//====================================================================
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

//====================================================================
//函数名称：frameEncode
//功能概要：实现待发送数据的组帧,将待发送数据加上帧头、帧长、帧尾以及校验信息
//       frame=帧头+IMSI+帧长+data+CRC校验码+帧尾
//参数说明：imsi：发送数据的设备的IMSI号
//       data:待组帧的数据头指针
//       dlen:待组帧的数据长度
//       frame:组帧之后的待发送数据帧
//       framelen:组帧后的数据长度
//函数返回：0：成功
//====================================================================
void frameEncode(uint_8 *imsi,uint_8 *data,uint_16 dlen,
                 uint_8 *frame,uint_16 *framelen)
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

//===================================================================
//函数名称：crc16
//功能概要：将数据进行16位的CRC校验，返回校验后的结果值
//参数说明：ptr:需要校验的数据缓存区
//       len:需要检验的数据长度
//函数返回：计算得到的校验值
//===================================================================
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

//====================================================================
//函数名称：uecom_recv
//函数返回：0：接收到了通过TCP/UDP发来的数据；1：正在组帧；2接收数据错误
//参数说明：*data:将接收到的数据存放到该数组串中;
//                *dataLen:存储接收到的数据长度
//功能概要：将接收到的数据进行解帧，并将有效数据存入data数组之中，由串口的uart中断调用。
//内部调用：uart_init，uart_disable_re_int,uart_re1，
//修改日期：2017.08.11,QHJ,CC；2017.09.25，WB；
//====================================================================
uint_8 uecom_recv(uint_8 ch,uint_16 *dataLen, uint_8 *data)
{
    static uint_16 index = 0;
    static uint_16 length = 0;
    uint_8 ret_val;
    uint_16 i;
    uint_8 CRC[2];
    uint_16 crc;
    if((index == 0 && ch != frameHead[0]) ||
      (index == 1 && ch != frameHead[1]))  //未接收到数据或者未遇到帧头
    {
        index = 0;
        length = 0;
        ret_val = 2;     //接收数据错误
        goto uecom_recv_exit;
    }
    data[index++] = ch;//存入数据
    if(index == 19)//读取有效数据长度
    {
        length = ((uint_16)data[17]<<8) + data[18];
    }
    //接收到的数据达到一帧长度。23为帧头帧尾等长度
    if(length != 0 && index >= length+23)
    {
        //CRC校验
        crc = crc16(data+2,length+17);
        CRC[0] = (crc>>8)&0xff;
        CRC[1] = crc&0xff;

        if(data[index-2]!=frameTail[0] ||
        		data[index-1]!= frameTail[1]  //未遇到帧尾
            || CRC[0] != data[length+19] ||
			CRC[1] != data[length+20])        //CRC检验错误
        {
            index = 0;
            length = 0;
            ret_val = 2;
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
        goto uecom_recv_exit;
    }
    *dataLen = 0;//正在组帧
    ret_val = 1;
uecom_recv_exit:
    return ret_val;
}

//===================================================================
//函数名称：uecom_ATret
//功能概要：在中断中处理AT返回的指令
//参数说明：ch：串口接收到的数据
//函数返回：无
//===================================================================
//处理AT返回
void uecom_ATret(uint_8 ch)
{
    //（1）变量声明和赋初值
	if(curATCommand[0] == 0)       //若当前不是AT命令发送模式
	{
		return;
	}
	if(AT_reLength >= 510)             //若接收到的数据超过510个，则重新接收
	{
		AT_reLength = 0;
	}
	AT_reBuf[AT_reLength++] = ch;    //存储接收到的数据到缓冲区AT_reBuf中
	AT_reBuf[AT_reLength] = 0;
	//若缓冲区AT_reBuf中接收到OK或者ERROR
	if( (AT_reBuf[AT_reLength-2]=='O' && AT_reBuf[AT_reLength-1]=='K')
			|| (AT_reLength > 4 && AT_reBuf[AT_reLength-5]=='E'
			&&  AT_reBuf[AT_reLength-4]=='R'  &&  AT_reBuf[AT_reLength-3]=='R'
			&&  AT_reBuf[AT_reLength-2]=='O' &&  AT_reBuf[AT_reLength-1]=='R'))
	{
		curATCommand[0] = 0;         //结束AT接收
		AT_reFlag = AT_reLength;      //标志已经接收到数据
	}
	return ;
}
//===================================================================
//函数名称：uecom_interrupt
//功能概要：在中断中应处理的操作
//参数说明：无
//函数返回：无
//===================================================================
void uecom_interrupt(void)
{
	uint_8 ch,flag;
	 DISABLE_INTERRUPTS;
	if(uart_get_re_int(UART_UE))
	{
		uart_disable_re_int(UART_UE);    //关串口总中断
		ch = uart_re1 (UART_UE, &flag);    //调用接收一个字节的函数，清接收中断位
		if(flag)
		{
//			uart_send1(UART_2,ch);
			uecom_ATret(ch);                               //处理AT命令返回的数据
			uecom_recv(ch,&gRecvLength,gRecvBuf); //处理GPRS接收到的数据
		}
		uart_enable_re_int(UART_UE);    //开串口总中断
	}
    ENABLE_INTERRUPTS;
}
