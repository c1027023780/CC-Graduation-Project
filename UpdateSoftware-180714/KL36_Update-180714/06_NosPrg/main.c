//说明见工程文件夹下的Doc文件夹内Readme.txt文件
//==============================================================
//【不动】GLOBLE_VAR宏定义及包含总头文件
#define GLOBLE_VAR  //只需在main.c中定义一次，用来处理全局变量声明与使用问题
#include "includes.h"   //包含总头文件
//初始化重新烧录程序后63扇区的值，即：初始化存储在flash中的配置信息
__attribute__((section (".MacConfig"))) const struct FlashData flashInit[]=
{
		" 【金葫芦物联网】 AHL-IoT-GEC ",      //产品名称
		 "215000-000000-000001",                    //产品序列号
		 "AHL-101-1AC【M6220】",                 //产品型号
		"  金葫芦物联网有限责任制公司  ",           //生产厂家
		"V3.2",                                                      //软件版本
		1530453305,                                           //生产时间的时间戳
		"苏州大学软件开发人员",                        //用户名
		"13000000000",
		"122.112.217.197",
		"8093",
		600,
		0,
		"U0"
};

//【根据实际需要增删】main.c使用的内部函数声明处
void flashData_get(struct FlashData * data);     //读取flash中的参数信息并保存到data中
void userData_init(struct UserData *data);        //初始化用户信息data
void userData_get(struct UserData *data);        //获取用户信息data
void LCDShowTime(void);    //LCD上更新时间显示
void LCD_ShowInit();            //初始化LCD的显示，主要是标签
//void LCD_ShowGet();            //获得LCD上指定标签应该显示的内容，并在LCD上显示
//void get_gnss();                    //尝试获得GNSS信息，并通过串口打出是否成功获得
void LCD_ShowFlash();        //获得LCD上显示的存储于flash中的信息，并显示

extern void LPTMR0_IRQ(void);
extern void TSI0_IRQ(void);
extern void UART1_IRQ(void);

//主函数，一般情况下可以认为程序从此开始运行（实际上有启动过程见书稿）
int main(void)
{
    //（1）【根据实际需要增删】声明主函数使用的变量（声明时不准赋值）
    uint_64 mSec;           //主循环使用的“秒”
    uint_64 mMin;          //主循环使用的“分”
    uint_32 mTsi;            //记录当前触摸按键次数
    uint_8 mSendFlag;          //标记是否发送数据
    uint_16 mWaitSecond;    //已经等待接收的秒数
    uint_8 mRetdata[100];    //存放通信构件uecom初始化返回的结果
    uint_16 mUserLen;          //需要发送的字节长度
    uint_16 mFlashLen;         //需要存储flash的字节长度
    uint_8 imsi[17];               //存放IMSI号
    uint_8 showData[8];       //显示缓冲区
    uint_64 mRecvCount;     //收到的字节数
    uint_8 mi;                       //使用到的循环变量
    uint_8 mj;                       //使用到的循环变量
    uint_16 mi_16;               //使用到的循环变量
    uint_64 mTmp;         //主循环使用的临时变量
    float mFloat;         //主循环使用的临时变量
    uint_8 mString[30];      //数字转文本使用的临时数组
	uint_8 mCmd[2];         //存储命令
    uint_8 mflag;                  //主循环使用的标志判断变量
    uint_8 mFlashFlag;         //表明是否需要将数据写入flash
    uint_8 mLCDFlag;         //表明是否需要更新LCD
    uint_64 mLoopCount;         //记录循环次数
    uint_8 mSendData[1000];   //待发送数据缓冲区
    uint_16 mSendLen;             //待发送数据的长度
    uint_8 date[20];
    uint_8 sInfo[24];
    //（2）【不动】 关总中断
    DISABLE_INTERRUPTS;
    //（3） 【根据实际需要增删】给有关变量赋初值
    //（3.1）给主函数使用的变量赋初值
    mSec=0;      //主循环使用的“秒”
    mMin=0;     //主循环使用的“分”
    mTsi=0;       //记录当前触摸按键次数
    mSendFlag = 1;      //标记是否发送数据,上电首先发送一次数据
    mFlashFlag = 0;      //默认不写入flash
    mWaitSecond = 0;  //清零已经等待接收的秒数
    mUserLen = sizeof(struct UserData);  //获得需要发送的字节长度
    mFlashLen = sizeof(struct FlashData);  //获得存入flash的字节长度
    mLoopCount = 0;  //清空循环次数
    mRecvCount = 0;  //清空接收次数
    mLCDFlag = 0;
    //（3.2）给全局变量赋初值
    gRecvLength = 0;     //收到数据的字节长度
    //（4） 【根据实际需要增删】初始化外设模块
	cpy_nvic();
	funPointerInit();

	systemTimeGet(date);  //获取系统时间
	systemInfoGet(sInfo); //获取系统信息

    uecom_power(UECOM_ON);    //给通信模组供电（因需延时，所以最先操作）
    light_init(LIGHT_RED,LIGHT_ON);  //初始化红灯，默认暗
    lptmr_init(500);    //LPTMR计时器初始化为500毫秒
    //uart_init(UART_2, 115200);  //初始化串口2，波特率115200
    tsi_init(3,TSI_VALUE);           //初始化触摸按键TSI（通道3）,设定触发阈值
    flash_init();                           //初始化flash
    LCD_Init();             //LCD初始化
	LCD_ShowInit();   //初始化LCD的默认显示内容
    //（5）【根据实际需要增删】 使能模块中断
	setVector(LPTMR0_IRQn,(uint_32)&LPTMR0_IRQ);
	setVector(TSI0_IRQn,(uint_32)&TSI0_IRQ);
	setVector(UART1_IRQn,(uint_32)&UART1_IRQ);
    enable_lptmr_int();              //使能LPTMR中断
    tsi_enable_re_int();               //使能TSI中断

    //uart_enable_re_int(UART_2);    //使能串口2接收中断
    //（6）【不变】开总中断
    ENABLE_INTERRUPTS;

    systemTimeSet(1514764800);//2018-01-01 00:00:00

    //（7.4.5）显示设备的温度
    adc_init(MUXSEL_A,0,16,SAMPLE32);
    mi_16 = adc_read(26);
    mFloat = 25.0-(mi_16*3.3*1000/65535-719)/1.715;
    //printf("当前套片芯片温度为：%2.1f\n",mFloat);
    LCD_ShowString(105,105,BLUE,GRAY,(uint_8*)myFtoa(mFloat,1,mString));

    //（7）【根据实际需要增删】主循环前的初始化操作
	if(ENABLE_WDOG)wdog_feed();    //若开启看门狗，则喂狗
	//（7.1）读取flash中的配置信息至gFlashData；初始化用户帧数据gUserData
	flashData_get(&gFlashData);
	userData_init(&gUserData);
	//（7.2）判断复位状态，并将复位状态数据存储到flash中
	mTmp = gFlashData.resetCount;      // 保存当前看门狗复位次数
	if (BGET(RCM_SRS0_WDOG_SHIFT, RCM_SRS0))  //若为看门狗复位，则加1
		gFlashData.resetCount++;
	else if(BGET(RCM_SRS0_POR_SHIFT, RCM_SRS0))   //若为上电复位，则置零
		gFlashData.resetCount = 0;
	if(gFlashData.resetCount!=mTmp)     //若复位次数发送变化，则重新写入flash
	{
 		flash_erase(63);
 		flash_write(63,0,mFlashLen,(uint_8*)(&gFlashData));
	}
    //printf("当前的看门狗复位次数为：%ld\n",gFlashData.resetCount);
    //（7.3）初始化通信模组，并在LCD上显示初始化过程
    while(1)
    {
    	//（7.3.1）通信模组上电并等待延时等待约12秒
    	uecom_power(UECOM_ON);    //给通信模组供电
        //printf("AHL Run .\n");
        LCD_ShowString(4,300,BLUE,GRAY,"AHL Run .                   ");
        Delay_ms(6000);                          //延时6秒
        //printf("AHL Run ..\n");
        LCD_ShowString(4,300,BLUE,GRAY,"AHL Run ..                  ");
        Delay_ms(6000);                         //延时6秒
        if(ENABLE_WDOG) 	wdog_feed();    //若开启看门狗，则喂狗
        //（7.3.2）通信模组初始化，包括联网和建立网络连接过程
        //初始化通信模组
        LCD_ShowString(4,300,BLUE,GRAY,"AHL Initialize .            ");
		mflag =uecom_init();
		if(mflag)continue;
		//与基站建立连接
        LCD_ShowString(4,300,BLUE,GRAY,"AHL Initialize ..           ");
		mflag =uecom_linkBase();
		if(mflag)continue;
		//与服务器建立连接
        LCD_ShowString(4,300,BLUE,GRAY,"AHL Initialize ...           ");
		mflag =uecom_linkCS(gFlashData.serverIP,gFlashData.serverPort);
		if(mflag)continue;
		LCD_ShowString(4,300,BLUE,GRAY,"AHL Initialize ....           ");
		//(7.3.3) 判断通信模组是否初始化成功
		if(!mflag)break;  //如果通信模组初始化成功，直接跳出循环，否则往下执行
		switch(mflag)
		{
			case 1:
				LCD_ShowString(4,300,BLUE,GRAY,"Inital Error:AT Error       ");
				//printf("Inital Error:AT Error\n");
				break;
			case 2:
				LCD_ShowString(4,300,BLUE,GRAY,"Inital Error:ATE0 Error     ");
				//printf("Inital Error:ATE0 Error\n");
				break;
			case 3:
				LCD_ShowString(4,300,BLUE,GRAY,"Inital Error:IMEI Error     ");
				//printf("Inital Error:IMEI Error\n");
				break;
			case 4:
				LCD_ShowString(4,300,BLUE,GRAY,"Inital Error:IMSI Error     ");
				//printf("Inital Error:IMSI Error\n");
				break;
			case 5:
				LCD_ShowString(4,300,BLUE,GRAY,"Inital Error:B Type Error   ");
				//printf("Inital Error:B Type Error\n");
				break;
			case 6:
				LCD_ShowString(4,300,BLUE,GRAY,"Inital Error:Net Error      ");
				//printf("Inital Error:Net Error\n");
				break;
			case 7:
				LCD_ShowString(4,300,BLUE,GRAY,"Inital Error:PDP Error      ");
				//printf("Inital Error:PDP Error\n");
				break;
			case 8:
				LCD_ShowString(4,300,BLUE,GRAY,"Inital Error:Get IP Error   ");
				//printf("Inital Error:Get IP Error\n");
				break;
			case 9:
				LCD_ShowString(4,300,BLUE,GRAY,"Inital Error:AT OK,IP Error ");
				//printf("Inital Error:AT OK,IP Error\n");
				break;
			case 10:
				 LCD_ShowString(4,300,BLUE,GRAY,"Inital Error:TCP Error     ");
				 //printf("Inital Error:TCP Error\n");
				 break;
		}
		uecom_power(UECOM_REBOOT);    //通信模组重启
    }
    //printf("AHL Initialize Successfully\n");
    LCD_ShowString(4,300,BLUE,GRAY,"AHL Initialize Successfully ");
    //（7.4）【根据实际需要增删】获得LCD需要显示的数据并显示
    //（7.4.1）显示型号
	copyN(mString,gFlashData.equipType,20);
	mString[20]=0;
	LCD_ShowString(70,45,BLUE,GRAY,mString);
	//（7.4.2）显示软件版本
	copyN(mString,gFlashData.softVer,4);
	mString[4]=0;
	LCD_ShowString(70,65,BLUE,GRAY,mString);
    //(7.4.3)显示设备的IMSI号
	uecom_modelInfo(mRetdata);    //获取通信模组信息
    //printf("当前套片IMSI号为：%s\n",mRetdata);  //串口输出IMSI号
    LCD_ShowString(70,85,BLUE,GRAY,mRetdata+20);
    //（7.4.5）显示设备的温度
    adc_init(MUXSEL_A,0,16,SAMPLE32);
    mi_16 = adc_read(26);
    mFloat = 25.0-(mi_16*3.3*1000/65535-719)/1.715;
    //printf("当前套片芯片温度为：%2.1f\n",mFloat);
    LCD_ShowString(105,105,BLUE,GRAY,(uint_8*)myFtoa(mFloat,1,mString));
    //（7.4.6）显示信号强度和小区号（基站号）
    for(mi=0;mi<5;mi++)
    {
    	mflag = uecom_baseInfo(mRetdata);
    	if(mflag)
    	{
    		Delay_ms(1000);
    		continue;
    	}
    	LCD_ShowString(90,129,BLUE,GRAY,mRetdata+1);
    	LCD_ShowString(90,150,BLUE,GRAY,(uint_8 *)myItoa(mRetdata[0],mString));
    }
//    //（7.4.7）显示设备的信号强度
//    for(mi=0;mi<5;mi++)
//    {
//	    mflag = uecom_getSignalPower(&mj);
//	    if(mflag == 0 && mj!=0)
//	    {
//		    break;
//	    }
//	    Delay_ms(2000);
//    }
//   // printf("通信模组信号强度：%d\n",mj);
//    LCD_ShowString(90,150,BLUE,GRAY,(uint_8 *)myItoa(mj,mString));
	//（7.4.8）显示IP
	copyN(mString,gFlashData.serverIP,15);
	mString[15]=0;
	LCD_ShowString(65,169,BLUE,GRAY,mString);
    //（7.4.9）显示PORT
	copyN(mString,gFlashData.serverPort,5);
	mString[5]=0;
	LCD_ShowString(65,189,BLUE,GRAY,mString);
	//（7.4.10）显示TSI
	LCD_ShowString(36,209,BLUE,GRAY,"0");
	//（7.4.11）显示发送频率
	LCD_ShowString(180,210,BLUE,GRAY,"          ");
	LCD_ShowString(180,210,BLUE,GRAY,
			(uint_8 *)myItoa(gFlashData.sendFrequencySec,mString));
    //测试http使用，1AC硬件支持
//    http_get("122.112.217.197","88","http://122.112.217.197:88/AHL-location/FrameMessage.aspx?460,000,5280,e46a",datadd);
    //主循环开始======================================================
    for(;;)
    {
    	mLoopCount++;
        //（1）一秒到达之后进行的操作
        if (mSec!=gUserData.currentTime)    //1秒到
        {
        	//（1.1）更新当前秒数并在LCD上显示时间
            mSec=gUserData.currentTime;   //更新秒变量mSec
            systemTimeGet(date);
            LCD_ShowString(49,230,BLUE,GRAY,date);
    		//timeChange(gUserData.currentTime,gTime);    //转成年月日时分秒
            //LCDShowTime();    //LCD上显示时间
            //（1.2）控制红灯闪烁
            light_change(LIGHT_RED);           //红灯每秒闪烁一次
            //（1.3）判断是否到达发送数据的时间
            if(gUserData.currentTime%gFlashData.sendFrequencySec == 0 )   //发送时间（秒的整倍数）
            {
                mSendFlag = 1;
            }
            //（1.4）判断是否到达检测gnss是否正常的时间
//            if(gUserData.currentTime%30 == 0)      //每30秒检测一次gnss信息
//            {
//                get_gnss();   //获取gnss信息
//            }
        }
        //（2）触摸一次TSI之后执行的操作
        if (mTsi != gUserData.touchNum)    //触摸1次
        {
        	//（2.1）更新当前当前TSI触摸次数，在LCD上进行显示，并用串口输出
        	mTsi = gUserData.touchNum;
            //printf("当前TSI触摸%d次\n",gUserData.touchNum);
            //（2.2）若触摸达到3次，则发送一次数据
            if(gUserData.touchNum%3==0)
                mSendFlag = 1;
        }
        //（3）若需要执行发送数据操作，则进行下列操作
        if(mSendFlag == 1)
        {
//            //（3.1）UE模块初始化
//            printf("提示H1-1001，通信模组开始初始化\n");
//            LCD_DrawSurface(4,300,236,316,GRAY);
//            LCD_ShowString(4,300,BLUE,GRAY,"提示H1-1001,开始初始化...");
//            mflag =    uecom_init(mRetdata,gFlashData.serverIP,gFlashData.serverPort);
//            //(3.2) 若初始化成功，则获取需要发送的数据mSendData
//            if(mflag)  goto main_loop1;           //初始化失败，LCD显示提示
//            //至此，UE模块初始化成功
//            printf("提示H1-1002，通信模组初始化成功\n");
//            LCD_ShowString(4,300,BLUE,GRAY,"提示H1-1002，初始化成功");
            userData_get(&gUserData);        //获取用户信息gUserData
            //根据命令，获得需要发送的数据
            if(gFlashData.frameCmd[0]=='U'&&gFlashData.frameCmd[1]=='0')
            {
            	mSendLen = mUserLen;
            	copyN(mSendData,(uint_8 *)&gUserData,mSendLen);
            }
            else if(gFlashData.frameCmd[0]=='U'&&gFlashData.frameCmd[1]=='1')
            {
            	copyN(mSendData,gFlashData.frameCmd,2);
            	copyN(mSendData+2,gUserData.IMSI,15);
            	copyN(mSendData+17,(uint_8 *)&gUserData.currentTime,8);
            	copyN(mSendData+25,(uint_8 *)&gUserData.mcuTemp,4);
            	copyN(mSendData+29,(uint_8 *)&gUserData.signalPower,1);
            	copyN(mSendData+30,(uint_8 *)&gUserData.bright,2);
            	copyN(mSendData+32,(uint_8 *)&gUserData.touchNum,2);
            	copyN(mSendData+34,gUserData.lbs_location,25);
                mSendLen = 59;
            }
            //（3.3）显示信号强度
            LCD_ShowString(90,150,BLUE,GRAY,"     ");
            LCD_ShowString(90,150,BLUE,GRAY,(uint_8 *)myItoa(gUserData.signalPower,mString));
            //printf("通信模组信号强度：%d\n",gUserData.signalPower);
            //（3.4）UE模块发送数据
            LCD_ShowString(4,300,BLUE,GRAY,"AHL Send .                  ");
            //printf("AHL Send .\n");
            mflag = uecom_send(mSendLen,mSendData); //结构体的地址可直接强制转为数组的地址
            //printf("AHL Send ..\n");
            LCD_ShowString(4,300,BLUE,GRAY,"AHL Send ..                 ");
            if(mflag)  goto main_loop1;                   //数据发送失败，LCD显示提示
            //printf("AHL Send Successfully\n");
            LCD_ShowString(4,300,BLUE,GRAY,"AHL Send Successfully       ");
            mWaitSecond = 0;                     //清零接收数据等待计时时间
            goto main_loop;
        main_loop1:
			//数据发送失败提示
			switch(mflag)
			{
				case 1:
					LCD_ShowString(4,300,BLUE,GRAY,"Send Error:Send Not Start   ");
					//printf("Send Error:Send Not Start\n");
					break;
				case 2:
					LCD_ShowString(4,300,BLUE,GRAY,"Send Error:Send Data Not OK ");
					//printf("Send Error:Send Data Not OK\n");
					break;
			}
			//重新初始化
			uecom_init();
			uecom_linkBase();
			uecom_linkCS(gFlashData.serverIP,gFlashData.serverPort);
            goto main_loop;
        main_loop:
            mSendFlag = 0;         //修改发送标记
        }
        //（4）判断是否接收到服务器发来数据
        if(gRecvLength != 0)
        {
        	mRecvCount++;
        	LCD_ShowString(90,249,BLUE,GRAY,myItoa(mRecvCount,mString));
        	LCD_ShowString(4,300,BLUE,GRAY,"AHL Recv one frame                 ");
        	mflag = 0xff;
        	mSendLen = 0;
        	copyN(mCmd,gRecvBuf,2);
        	copyN(mSendData,gRecvBuf,2);
			if(mCmd[0]=='A'&&mCmd[1]=='0')          //读取flash中的所有信息
			{
				mSendLen = mFlashLen+2;
	        	copyN(mSendData+2,(uint_8*)(&gFlashData),mFlashLen);
			}
			else if(mCmd[0]=='A'&&mCmd[1]=='1')  //读取flash中的产品信息
			{
				mSendLen = 145;
	        	copyN(mSendData+2,gFlashData.equipName,mSendLen-2);
			}
			else if(mCmd[0]=='A'&&mCmd[1]=='2')  //读取flash中的服务器信息
			{
				mSendLen = 22;
	        	copyN(mSendData+2,gFlashData.serverIP,mSendLen-2);
			}
			else if(mCmd[0]=='A'&&mCmd[1]=='3')  //读取用户存入flash的信息
			{
				mSendLen = 10;
	        	copyN(mSendData+2,(uint_8*)(&gFlashData.sendFrequencySec),mSendLen-2);
			}
			else if(mCmd[0]=='B'&&mCmd[1]=='0')   //更改flash中的所有信息
			{
				copyN((uint_8 *)(gFlashData.equipName),(uint_8*)&(gRecvBuf[2]),mFlashLen);
				mFlashFlag = 1;
				mSendLen = 9;
	        	copyN(mSendData+2,"success",mSendLen-2);
			}
			else if(mCmd[0]=='B'&&mCmd[1]=='1')  //更改flash中的产品信息
			{
				copyN((uint_8 *)(gFlashData.equipName),(uint_8*)&(gRecvBuf[2]),124);
				mFlashFlag = 1;
				mSendLen = 9;
	        	copyN(mSendData+2,"success",mSendLen-2);
			}
			else if(mCmd[0]=='B'&&mCmd[1]=='2')  //更改flash中的服务器信息
			{
				copyN((uint_8 *)(gFlashData.serverIP),(uint_8*)&(gRecvBuf[2]),30);
				mFlashFlag = 1;
				mSendLen = 9;
	        	copyN(mSendData+2,"success",mSendLen-2);
			}
			else if(mCmd[0]=='B'&&mCmd[1]=='3')  //更改用户存入flash的信息
			{
				copyN((uint_8 *)(&gFlashData.sendFrequencySec),(uint_8*)&(gRecvBuf[2]),8);
				mFlashFlag = 1;
				mSendLen = 9;
	        	copyN(mSendData+2,"success",mSendLen-2);
			}
			else if(mCmd[0]=='U'&&mCmd[1]=='0')
			{
				copyN(gFlashData.frameCmd,mCmd,2);
				if(gRecvLength == mUserLen)   //若为整帧数据
				{
					copyN((uint_8*)(&gUserData),gRecvBuf,mUserLen);
					copyN(gFlashData.equipName,gUserData.equipName,30);
					copyN(gFlashData.equipID,gUserData.equipID,20);
					copyN(gFlashData.equipType,gUserData.equipType,20);
					copyN(gFlashData.vendor,gUserData.vendor,30);
					copyN(gFlashData.softVer,gUserData.softVer,4);
					systemTimeSet(gUserData.currentTime);
					copyN(gFlashData.userName,gUserData.userName,20);
					copyN(gFlashData.phone,gUserData.phone,11);
					copyN(gFlashData.serverIP,gUserData.serverIP,15);
					copyN(gFlashData.serverPort,gUserData.serverPort,5);
					gFlashData.sendFrequencySec = gUserData.sendFrequencySec;
					gFlashData.resetCount = gUserData.resetCount;
					copyN(gFlashData.frameCmd,gUserData.cmd,2);
					mLCDFlag==1;
					mFlashFlag  =1;
				}
				mLCDFlag=1;
			}
			else if(mCmd[0]=='U'&&mCmd[1]=='1')
			{
				copyN(gFlashData.frameCmd,mCmd,2);
				if(gRecvLength == 59)
				{
	            	copyN(gUserData.cmd,gRecvBuf,2);
	            	copyN(gUserData.IMSI,gRecvBuf+2,15);
	            	copyN((uint_8 *)&gUserData.currentTime,gRecvBuf+17,8);
	            	copyN((uint_8 *)&gUserData.mcuTemp,gRecvBuf+25,4);
	            	copyN((uint_8 *)&gUserData.signalPower,gRecvBuf+29,1);
	            	copyN((uint_8 *)&gUserData.bright,gRecvBuf+30,2);
	            	copyN((uint_8 *)&gUserData.touchNum,gRecvBuf+32,2);
	            	copyN(gUserData.lbs_location,gRecvBuf+34,25);
				}
				mLCDFlag=1;
			}
			else                             //其他情况
			{
			}
			if(mSendLen>0)     //若有需要发送的数据
			{
				 mflag = uecom_send(mSendLen,mSendData); //结构体的地址可直接强制转为数组的地址
			}
            gRecvLength = 0;     //接收数据长度清零，表明已经读取，防止重复读取
			if(mflag==0)
				LCD_ShowString(4,300,BLUE,GRAY,"AHL Send Successfully              ");
			else if(mflag == 0xff)
				LCD_ShowString(4,300,BLUE,GRAY,"AHL Recv No Command         ");
			else
				LCD_ShowString(4,300,BLUE,GRAY,"Send Error:Send Data Not OK ");
             //判断是否需要写flash
             if(mFlashFlag == 1)
             {
         		flash_erase(63);
         		flash_write(63,0,mFlashLen,(uint_8*)(&gFlashData));
         		mFlashFlag = 0;
         		mLCDFlag=1;
             }
             if(mLCDFlag==1)
             {
          		LCD_ShowFlash();//更新LCD上的显示
             }

        }
        //（5）每隔一段时间喂狗
        if(mLoopCount%48000)
        {
        	if(ENABLE_WDOG)wdog_feed();    //若开启看门狗，则喂狗
        }
        //主循环结束======================================================
    }
}
//=====================================================================
//函数名称：flashData_get
//函数返回：无
//参数说明：data：需要赋值的结构体数据
//功能概要：给通信帧结构体comData赋值函数
//=====================================================================
void flashData_get(struct FlashData *data)
{
	//读取flash中的配置信息
	flash_read_logic((uint_8*)data,63,0,sizeof(struct FlashData));
}

//=====================================================================
//函数名称：userData_init
//函数返回：无
//参数说明：data：需要初始化的结构体数据
//功能概要：初始化用户帧结构体data
//=====================================================================
void userData_init(struct UserData *data)
{
	copyN(data->cmd,"U0",2);
	copyN(gFlashData.frameCmd,data->cmd,2);
    data->sn = 0;
    copyN(data->serverIP,gFlashData.serverIP,15);
    copyN(data->serverPort,gFlashData.serverPort,5);
    data->currentTime = gFlashData.productTime;
    data->resetCount = gFlashData.resetCount;
    data->sendFrequencySec = gFlashData.sendFrequencySec;
    copyN(data->userName,gFlashData.userName,20);
    copyN(data->softVer,gFlashData.softVer,4);
    copyN(data->equipName,gFlashData.equipName,30);
    copyN(data->equipID,gFlashData.equipID,20);
    copyN(data->equipType,gFlashData.equipType,20);
    copyN(data->vendor,gFlashData.vendor,30);
    copyN(data->phone,gFlashData.phone,11);
    data->touchNum = 0;
    copyN(data->cmd,gFlashData.frameCmd,2);
}

//=====================================================================
//函数名称：userData_get
//函数返回：无
//参数说明：data：需要赋值的结构体数据
//功能概要：给用户帧结构体data赋值
//=====================================================================
void userData_get(struct UserData *data)
{
    uint_16 tmpAD,brightAD;
    uint_8 sPower;
    uint_8 tempertature[6];
    uint_8 sTmp[40];
    static uint_32 sn = 4;
    data->sn = sn++;
    adc_init(MUXSEL_A,0,16,SAMPLE32);
    tmpAD = adc_read(26);
    data->mcuTemp = (int_32)((25.0-(tmpAD*3.3*1000/65535-719)/1.715)*10);
    LCD_ShowString(105,105,BLUE,GRAY,(uint_8*)myFtoa(data->mcuTemp/10.0,1,tempertature));
    //获取光照强度
    brightAD = adc_read(7);
    data->bright = brightAD;
    uecom_modelInfo(sTmp);
    copyN(data->IMEI,sTmp,15);
    copyN(data->IMSI,sTmp+20,15);
    uecom_baseInfo(sTmp);
    data->signalPower = sTmp[0];
    copyN(data->lbs_location,sTmp+1,19);
}

//=====================================================================
//函数名称：LCD_ShowInit
//函数返回：无
//参数说明：无
//功能概要：初始化LCD上电显示的内容
//=====================================================================
void LCD_ShowInit()
{
//    uint_16 tmpAD;         //读取到的芯片温度AD值
//    float tmp;             //真实的芯片温度值
//    uint_8 flag,i;
//    uint_8 imsi[20];
//    uint_8 Community[30];
//    uint_8 sSignalPower[10];
//    uint_8 IntegerToChar[10];
    uint_8 temp[30] = {0};
	//（1）设置全局底色为灰色
	LCD_DrawSurface(0,0,240,320,GRAY);
    //（2）设置第一区（标题区）
	aotu_panel(2,2,238,38,1);    //凸起
	LCD_ShowString(66,15,RED,GRAY,"金葫芦IoT-GEC");
	if(ENABLE_WDOG)    //若开启看门狗，则喂狗
		wdog_feed();
	//（3）设置第二区（与通信无关区）
	aotu_panel(2,43,238,123,0);    //凹下
	LCD_ShowString(4,45,BLACK,GRAY,"【Type】 ");
	LCD_ShowString(4,65,BLACK,GRAY,"【Soft】 ");
	LCD_ShowString(4,85,BLACK,GRAY,"【eSIM】 ");
	LCD_ShowString(4,105,BLACK,GRAY,"【Temp(度)】  ");
	//（4）设置第三区（与通信相关）
	aotu_panel(2,127,238,247,0);    //凹下
	LCD_ShowString(4,129,BLACK,GRAY,"【BSNum】");
	LCD_ShowString(4,149,BLACK,GRAY,"【Signal】 ");

	LCD_ShowString(4,169,BLACK,GRAY,"【I  P】");
	LCD_ShowString(4,189,BLACK,GRAY,"【PORT】");
	LCD_ShowString(4,209,BLACK,GRAY,"TSI:       Freq(s):   ");
	LCD_ShowString(4,229,BLACK,GRAY,"Time:");
	LCD_ShowString(49,230,BLUE,GRAY,"2018-07-01 21:55:00");
	LCD_ShowString(4,249,BLACK,GRAY,"RecvCount:");
	//（5）设置第四区（运行状态显示区）
	aotu_panel(2,274,238,318,1);    //凸起
	LCD_ShowString(4,276,BLACK,GRAY,"Run State:");
//    //（6）显示flash中的型号、软件版本号、TSI、发送频率、IP和PORT
//	LCD_ShowFlash();
}

//以下为内部函数
//=====================================================================
//函数名称：get_gnss
//函数返回：无
//参数说明：无
//功能概要：测试是否有gnss信息，并通过串口输出结果
//=====================================================================
//void get_gnss()
//{
//    double a,b;
//    char time[20];
//    double speed,high;
//	if(gnss_get_location(&a,&b))
//	{
//		uart_send_string(UART_2,"获取定位信息失败\r\n");
//	}
//	else
//	{
//		uart_send_string(UART_2,"获取定位信息成功\r\n");
//	}
//
//	if(gnss_get_time(time))
//	{
//		uart_send_string(UART_2,"获取时间失败\r\n");
//	}
//	else
//	{
//		uart_send_string(UART_2,"获取时间信息成功\r\n");
//	}
//
//	if(gnss_get_speed(&speed))
//	{
//		uart_send_string(UART_2,"获取速度失败\r\n");
//	}
//	else
//	{
//		uart_send_string(UART_2,"获取速度成功\r\n");
//	}
//
//	if(gnss_get_attitude(&high))
//	{
//		uart_send_string(UART_2,"获取高度失败\r\n");
//	}
//	else
//	{
//		uart_send_string(UART_2,"获取高度成功\r\n");
//	}
//}

//=====================================================================
//函数名称：LCDShowTime
//函数返回：无
//参数说明：void
//功能概要：在液晶屏上显示时间
//=====================================================================
void LCDShowTime(void)
{
//	uint_8 i,j;
//	uint_8 sTime[30] = {0};
//	for(j=0,i=0;i<14;j++)
//	{
//		if(j == 4)
//		{
//			strcat(sTime,"年");
//			j++;
//		}
//		else if(j == 8)
//		{
//			strcat(sTime,"月");
//			j++;
//		}
//		else if(j == 12)
//		{
//			strcat(sTime,"日");
//			j++;
//		}
//		else if(j == 16)
//		{
//			strcat(sTime,":");
//		}
//		else if(j == 19)
//		{
//			strcat(sTime,":");
//		}
//		else
//		    sTime[j] = gTime[i++]+'0';
//	}
//	LCD_ShowString(49,230,BLUE,GRAY,sTime);
}

//=====================================================================
//函数名称：LCD_ShowFlash
//函数返回：无
//参数说明：无
//功能概要：获得LCD上显示的存储于flash中的信息，并显示
//=====================================================================
void LCD_ShowFlash()
{
    uint_8 temp[30] = {0};
	//（6.1）显示型号
	copyN(temp,gFlashData.equipType,20);
	temp[20]=0;
	LCD_ShowString(70,45,BLUE,GRAY,temp);
	//（6.2）显示软件版本
	copyN(temp,gFlashData.softVer,4);
	temp[4]=0;
	LCD_ShowString(70,65,BLUE,GRAY,temp);
	//（6.3）显示发送频率
	LCD_ShowString(180,210,BLUE,GRAY,"          ");
	LCD_ShowString(180,210,BLUE,GRAY,
			(uint_8 *)myItoa(gFlashData.sendFrequencySec,temp));
	//（6.4）显示IP
	copyN(temp,gFlashData.serverIP,15);
	temp[15]=0;
	LCD_ShowString(65,169,BLUE,GRAY,temp);
    //（6.5）显示PORT
	copyN(temp,gFlashData.serverPort,5);
	temp[5]=0;
	LCD_ShowString(65,189,BLUE,GRAY,temp);
//	//（6.6）显示TSI
//	LCD_ShowString(36,209,BLUE,GRAY,"0");
}

/*
 知识要素：
 1.main.c是一个具体的实例，执行相关程序流程不会涉及任何环境，芯片问题。
 该文件所有代码均不会涉及具体的硬件和环境，它通过调用相关构件来实现与硬件
 系统的交互。
 2.本文件共有两类代码，一类为【根据实际需要增删】，此类代码根据具体
 项目需求进行更改；另一类为【不动】，此类代码与具体项目无关，是通用的，
 无需根据具体项目进行更改。
 3.本文件对宏GLOBLE_VAR进行了定义， 所以在包含"includes.h"头文件时
 会定义全局变量，在其他文件中包含"includes.h"头文件时，仅声明该头文件中
 的全局变量。
 */
