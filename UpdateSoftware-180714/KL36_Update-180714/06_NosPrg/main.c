//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//==============================================================
//��������GLOBLE_VAR�궨�弰������ͷ�ļ�
#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ���������ȫ�ֱ���������ʹ������
#include "includes.h"   //������ͷ�ļ�
//��ʼ��������¼�����63������ֵ��������ʼ���洢��flash�е�������Ϣ
__attribute__((section (".MacConfig"))) const struct FlashData flashInit[]=
{
		" �����«�������� AHL-IoT-GEC ",      //��Ʒ����
		 "215000-000000-000001",                    //��Ʒ���к�
		 "AHL-101-1AC��M6220��",                 //��Ʒ�ͺ�
		"  ���«���������������ƹ�˾  ",           //��������
		"V3.2",                                                      //����汾
		1530453305,                                           //����ʱ���ʱ���
		"���ݴ�ѧ���������Ա",                        //�û���
		"13000000000",
		"122.112.217.197",
		"8093",
		600,
		0,
		"U0"
};

//������ʵ����Ҫ��ɾ��main.cʹ�õ��ڲ�����������
void flashData_get(struct FlashData * data);     //��ȡflash�еĲ�����Ϣ�����浽data��
void userData_init(struct UserData *data);        //��ʼ���û���Ϣdata
void userData_get(struct UserData *data);        //��ȡ�û���Ϣdata
void LCDShowTime(void);    //LCD�ϸ���ʱ����ʾ
void LCD_ShowInit();            //��ʼ��LCD����ʾ����Ҫ�Ǳ�ǩ
//void LCD_ShowGet();            //���LCD��ָ����ǩӦ����ʾ�����ݣ�����LCD����ʾ
//void get_gnss();                    //���Ի��GNSS��Ϣ����ͨ�����ڴ���Ƿ�ɹ����
void LCD_ShowFlash();        //���LCD����ʾ�Ĵ洢��flash�е���Ϣ������ʾ

extern void LPTMR0_IRQ(void);
extern void TSI0_IRQ(void);
extern void UART1_IRQ(void);

//��������һ������¿�����Ϊ����Ӵ˿�ʼ���У�ʵ�������������̼���壩
int main(void)
{
    //��1��������ʵ����Ҫ��ɾ������������ʹ�õı���������ʱ��׼��ֵ��
    uint_64 mSec;           //��ѭ��ʹ�õġ��롱
    uint_64 mMin;          //��ѭ��ʹ�õġ��֡�
    uint_32 mTsi;            //��¼��ǰ������������
    uint_8 mSendFlag;          //����Ƿ�������
    uint_16 mWaitSecond;    //�Ѿ��ȴ����յ�����
    uint_8 mRetdata[100];    //���ͨ�Ź���uecom��ʼ�����صĽ��
    uint_16 mUserLen;          //��Ҫ���͵��ֽڳ���
    uint_16 mFlashLen;         //��Ҫ�洢flash���ֽڳ���
    uint_8 imsi[17];               //���IMSI��
    uint_8 showData[8];       //��ʾ������
    uint_64 mRecvCount;     //�յ����ֽ���
    uint_8 mi;                       //ʹ�õ���ѭ������
    uint_8 mj;                       //ʹ�õ���ѭ������
    uint_16 mi_16;               //ʹ�õ���ѭ������
    uint_64 mTmp;         //��ѭ��ʹ�õ���ʱ����
    float mFloat;         //��ѭ��ʹ�õ���ʱ����
    uint_8 mString[30];      //����ת�ı�ʹ�õ���ʱ����
	uint_8 mCmd[2];         //�洢����
    uint_8 mflag;                  //��ѭ��ʹ�õı�־�жϱ���
    uint_8 mFlashFlag;         //�����Ƿ���Ҫ������д��flash
    uint_8 mLCDFlag;         //�����Ƿ���Ҫ����LCD
    uint_64 mLoopCount;         //��¼ѭ������
    uint_8 mSendData[1000];   //���������ݻ�����
    uint_16 mSendLen;             //���������ݵĳ���
    uint_8 date[20];
    uint_8 sInfo[24];
    //��2���������� �����ж�
    DISABLE_INTERRUPTS;
    //��3�� ������ʵ����Ҫ��ɾ�����йر�������ֵ
    //��3.1����������ʹ�õı�������ֵ
    mSec=0;      //��ѭ��ʹ�õġ��롱
    mMin=0;     //��ѭ��ʹ�õġ��֡�
    mTsi=0;       //��¼��ǰ������������
    mSendFlag = 1;      //����Ƿ�������,�ϵ����ȷ���һ������
    mFlashFlag = 0;      //Ĭ�ϲ�д��flash
    mWaitSecond = 0;  //�����Ѿ��ȴ����յ�����
    mUserLen = sizeof(struct UserData);  //�����Ҫ���͵��ֽڳ���
    mFlashLen = sizeof(struct FlashData);  //��ô���flash���ֽڳ���
    mLoopCount = 0;  //���ѭ������
    mRecvCount = 0;  //��ս��մ���
    mLCDFlag = 0;
    //��3.2����ȫ�ֱ�������ֵ
    gRecvLength = 0;     //�յ����ݵ��ֽڳ���
    //��4�� ������ʵ����Ҫ��ɾ����ʼ������ģ��
	cpy_nvic();
	funPointerInit();

	systemTimeGet(date);  //��ȡϵͳʱ��
	systemInfoGet(sInfo); //��ȡϵͳ��Ϣ

    uecom_power(UECOM_ON);    //��ͨ��ģ�鹩�磨������ʱ���������Ȳ�����
    light_init(LIGHT_RED,LIGHT_ON);  //��ʼ����ƣ�Ĭ�ϰ�
    lptmr_init(500);    //LPTMR��ʱ����ʼ��Ϊ500����
    //uart_init(UART_2, 115200);  //��ʼ������2��������115200
    tsi_init(3,TSI_VALUE);           //��ʼ����������TSI��ͨ��3��,�趨������ֵ
    flash_init();                           //��ʼ��flash
    LCD_Init();             //LCD��ʼ��
	LCD_ShowInit();   //��ʼ��LCD��Ĭ����ʾ����
    //��5��������ʵ����Ҫ��ɾ�� ʹ��ģ���ж�
	setVector(LPTMR0_IRQn,(uint_32)&LPTMR0_IRQ);
	setVector(TSI0_IRQn,(uint_32)&TSI0_IRQ);
	setVector(UART1_IRQn,(uint_32)&UART1_IRQ);
    enable_lptmr_int();              //ʹ��LPTMR�ж�
    tsi_enable_re_int();               //ʹ��TSI�ж�

    //uart_enable_re_int(UART_2);    //ʹ�ܴ���2�����ж�
    //��6�������䡿�����ж�
    ENABLE_INTERRUPTS;

    systemTimeSet(1514764800);//2018-01-01 00:00:00

    //��7.4.5����ʾ�豸���¶�
    adc_init(MUXSEL_A,0,16,SAMPLE32);
    mi_16 = adc_read(26);
    mFloat = 25.0-(mi_16*3.3*1000/65535-719)/1.715;
    //printf("��ǰ��ƬоƬ�¶�Ϊ��%2.1f\n",mFloat);
    LCD_ShowString(105,105,BLUE,GRAY,(uint_8*)myFtoa(mFloat,1,mString));

    //��7��������ʵ����Ҫ��ɾ����ѭ��ǰ�ĳ�ʼ������
	if(ENABLE_WDOG)wdog_feed();    //���������Ź�����ι��
	//��7.1����ȡflash�е�������Ϣ��gFlashData����ʼ���û�֡����gUserData
	flashData_get(&gFlashData);
	userData_init(&gUserData);
	//��7.2���жϸ�λ״̬��������λ״̬���ݴ洢��flash��
	mTmp = gFlashData.resetCount;      // ���浱ǰ���Ź���λ����
	if (BGET(RCM_SRS0_WDOG_SHIFT, RCM_SRS0))  //��Ϊ���Ź���λ�����1
		gFlashData.resetCount++;
	else if(BGET(RCM_SRS0_POR_SHIFT, RCM_SRS0))   //��Ϊ�ϵ縴λ��������
		gFlashData.resetCount = 0;
	if(gFlashData.resetCount!=mTmp)     //����λ�������ͱ仯��������д��flash
	{
 		flash_erase(63);
 		flash_write(63,0,mFlashLen,(uint_8*)(&gFlashData));
	}
    //printf("��ǰ�Ŀ��Ź���λ����Ϊ��%ld\n",gFlashData.resetCount);
    //��7.3����ʼ��ͨ��ģ�飬����LCD����ʾ��ʼ������
    while(1)
    {
    	//��7.3.1��ͨ��ģ���ϵ粢�ȴ���ʱ�ȴ�Լ12��
    	uecom_power(UECOM_ON);    //��ͨ��ģ�鹩��
        //printf("AHL Run .\n");
        LCD_ShowString(4,300,BLUE,GRAY,"AHL Run .                   ");
        Delay_ms(6000);                          //��ʱ6��
        //printf("AHL Run ..\n");
        LCD_ShowString(4,300,BLUE,GRAY,"AHL Run ..                  ");
        Delay_ms(6000);                         //��ʱ6��
        if(ENABLE_WDOG) 	wdog_feed();    //���������Ź�����ι��
        //��7.3.2��ͨ��ģ���ʼ�������������ͽ����������ӹ���
        //��ʼ��ͨ��ģ��
        LCD_ShowString(4,300,BLUE,GRAY,"AHL Initialize .            ");
		mflag =uecom_init();
		if(mflag)continue;
		//���վ��������
        LCD_ShowString(4,300,BLUE,GRAY,"AHL Initialize ..           ");
		mflag =uecom_linkBase();
		if(mflag)continue;
		//���������������
        LCD_ShowString(4,300,BLUE,GRAY,"AHL Initialize ...           ");
		mflag =uecom_linkCS(gFlashData.serverIP,gFlashData.serverPort);
		if(mflag)continue;
		LCD_ShowString(4,300,BLUE,GRAY,"AHL Initialize ....           ");
		//(7.3.3) �ж�ͨ��ģ���Ƿ��ʼ���ɹ�
		if(!mflag)break;  //���ͨ��ģ���ʼ���ɹ���ֱ������ѭ������������ִ��
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
		uecom_power(UECOM_REBOOT);    //ͨ��ģ������
    }
    //printf("AHL Initialize Successfully\n");
    LCD_ShowString(4,300,BLUE,GRAY,"AHL Initialize Successfully ");
    //��7.4��������ʵ����Ҫ��ɾ�����LCD��Ҫ��ʾ�����ݲ���ʾ
    //��7.4.1����ʾ�ͺ�
	copyN(mString,gFlashData.equipType,20);
	mString[20]=0;
	LCD_ShowString(70,45,BLUE,GRAY,mString);
	//��7.4.2����ʾ����汾
	copyN(mString,gFlashData.softVer,4);
	mString[4]=0;
	LCD_ShowString(70,65,BLUE,GRAY,mString);
    //(7.4.3)��ʾ�豸��IMSI��
	uecom_modelInfo(mRetdata);    //��ȡͨ��ģ����Ϣ
    //printf("��ǰ��ƬIMSI��Ϊ��%s\n",mRetdata);  //�������IMSI��
    LCD_ShowString(70,85,BLUE,GRAY,mRetdata+20);
    //��7.4.5����ʾ�豸���¶�
    adc_init(MUXSEL_A,0,16,SAMPLE32);
    mi_16 = adc_read(26);
    mFloat = 25.0-(mi_16*3.3*1000/65535-719)/1.715;
    //printf("��ǰ��ƬоƬ�¶�Ϊ��%2.1f\n",mFloat);
    LCD_ShowString(105,105,BLUE,GRAY,(uint_8*)myFtoa(mFloat,1,mString));
    //��7.4.6����ʾ�ź�ǿ�Ⱥ�С���ţ���վ�ţ�
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
//    //��7.4.7����ʾ�豸���ź�ǿ��
//    for(mi=0;mi<5;mi++)
//    {
//	    mflag = uecom_getSignalPower(&mj);
//	    if(mflag == 0 && mj!=0)
//	    {
//		    break;
//	    }
//	    Delay_ms(2000);
//    }
//   // printf("ͨ��ģ���ź�ǿ�ȣ�%d\n",mj);
//    LCD_ShowString(90,150,BLUE,GRAY,(uint_8 *)myItoa(mj,mString));
	//��7.4.8����ʾIP
	copyN(mString,gFlashData.serverIP,15);
	mString[15]=0;
	LCD_ShowString(65,169,BLUE,GRAY,mString);
    //��7.4.9����ʾPORT
	copyN(mString,gFlashData.serverPort,5);
	mString[5]=0;
	LCD_ShowString(65,189,BLUE,GRAY,mString);
	//��7.4.10����ʾTSI
	LCD_ShowString(36,209,BLUE,GRAY,"0");
	//��7.4.11����ʾ����Ƶ��
	LCD_ShowString(180,210,BLUE,GRAY,"          ");
	LCD_ShowString(180,210,BLUE,GRAY,
			(uint_8 *)myItoa(gFlashData.sendFrequencySec,mString));
    //����httpʹ�ã�1ACӲ��֧��
//    http_get("122.112.217.197","88","http://122.112.217.197:88/AHL-location/FrameMessage.aspx?460,000,5280,e46a",datadd);
    //��ѭ����ʼ======================================================
    for(;;)
    {
    	mLoopCount++;
        //��1��һ�뵽��֮����еĲ���
        if (mSec!=gUserData.currentTime)    //1�뵽
        {
        	//��1.1�����µ�ǰ��������LCD����ʾʱ��
            mSec=gUserData.currentTime;   //���������mSec
            systemTimeGet(date);
            LCD_ShowString(49,230,BLUE,GRAY,date);
    		//timeChange(gUserData.currentTime,gTime);    //ת��������ʱ����
            //LCDShowTime();    //LCD����ʾʱ��
            //��1.2�����ƺ����˸
            light_change(LIGHT_RED);           //���ÿ����˸һ��
            //��1.3���ж��Ƿ񵽴﷢�����ݵ�ʱ��
            if(gUserData.currentTime%gFlashData.sendFrequencySec == 0 )   //����ʱ�䣨�����������
            {
                mSendFlag = 1;
            }
            //��1.4���ж��Ƿ񵽴���gnss�Ƿ�������ʱ��
//            if(gUserData.currentTime%30 == 0)      //ÿ30����һ��gnss��Ϣ
//            {
//                get_gnss();   //��ȡgnss��Ϣ
//            }
        }
        //��2������һ��TSI֮��ִ�еĲ���
        if (mTsi != gUserData.touchNum)    //����1��
        {
        	//��2.1�����µ�ǰ��ǰTSI������������LCD�Ͻ�����ʾ�����ô������
        	mTsi = gUserData.touchNum;
            //printf("��ǰTSI����%d��\n",gUserData.touchNum);
            //��2.2���������ﵽ3�Σ�����һ������
            if(gUserData.touchNum%3==0)
                mSendFlag = 1;
        }
        //��3������Ҫִ�з������ݲ�������������в���
        if(mSendFlag == 1)
        {
//            //��3.1��UEģ���ʼ��
//            printf("��ʾH1-1001��ͨ��ģ�鿪ʼ��ʼ��\n");
//            LCD_DrawSurface(4,300,236,316,GRAY);
//            LCD_ShowString(4,300,BLUE,GRAY,"��ʾH1-1001,��ʼ��ʼ��...");
//            mflag =    uecom_init(mRetdata,gFlashData.serverIP,gFlashData.serverPort);
//            //(3.2) ����ʼ���ɹ������ȡ��Ҫ���͵�����mSendData
//            if(mflag)  goto main_loop1;           //��ʼ��ʧ�ܣ�LCD��ʾ��ʾ
//            //���ˣ�UEģ���ʼ���ɹ�
//            printf("��ʾH1-1002��ͨ��ģ���ʼ���ɹ�\n");
//            LCD_ShowString(4,300,BLUE,GRAY,"��ʾH1-1002����ʼ���ɹ�");
            userData_get(&gUserData);        //��ȡ�û���ϢgUserData
            //������������Ҫ���͵�����
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
            //��3.3����ʾ�ź�ǿ��
            LCD_ShowString(90,150,BLUE,GRAY,"     ");
            LCD_ShowString(90,150,BLUE,GRAY,(uint_8 *)myItoa(gUserData.signalPower,mString));
            //printf("ͨ��ģ���ź�ǿ�ȣ�%d\n",gUserData.signalPower);
            //��3.4��UEģ�鷢������
            LCD_ShowString(4,300,BLUE,GRAY,"AHL Send .                  ");
            //printf("AHL Send .\n");
            mflag = uecom_send(mSendLen,mSendData); //�ṹ��ĵ�ַ��ֱ��ǿ��תΪ����ĵ�ַ
            //printf("AHL Send ..\n");
            LCD_ShowString(4,300,BLUE,GRAY,"AHL Send ..                 ");
            if(mflag)  goto main_loop1;                   //���ݷ���ʧ�ܣ�LCD��ʾ��ʾ
            //printf("AHL Send Successfully\n");
            LCD_ShowString(4,300,BLUE,GRAY,"AHL Send Successfully       ");
            mWaitSecond = 0;                     //����������ݵȴ���ʱʱ��
            goto main_loop;
        main_loop1:
			//���ݷ���ʧ����ʾ
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
			//���³�ʼ��
			uecom_init();
			uecom_linkBase();
			uecom_linkCS(gFlashData.serverIP,gFlashData.serverPort);
            goto main_loop;
        main_loop:
            mSendFlag = 0;         //�޸ķ��ͱ��
        }
        //��4���ж��Ƿ���յ���������������
        if(gRecvLength != 0)
        {
        	mRecvCount++;
        	LCD_ShowString(90,249,BLUE,GRAY,myItoa(mRecvCount,mString));
        	LCD_ShowString(4,300,BLUE,GRAY,"AHL Recv one frame                 ");
        	mflag = 0xff;
        	mSendLen = 0;
        	copyN(mCmd,gRecvBuf,2);
        	copyN(mSendData,gRecvBuf,2);
			if(mCmd[0]=='A'&&mCmd[1]=='0')          //��ȡflash�е�������Ϣ
			{
				mSendLen = mFlashLen+2;
	        	copyN(mSendData+2,(uint_8*)(&gFlashData),mFlashLen);
			}
			else if(mCmd[0]=='A'&&mCmd[1]=='1')  //��ȡflash�еĲ�Ʒ��Ϣ
			{
				mSendLen = 145;
	        	copyN(mSendData+2,gFlashData.equipName,mSendLen-2);
			}
			else if(mCmd[0]=='A'&&mCmd[1]=='2')  //��ȡflash�еķ�������Ϣ
			{
				mSendLen = 22;
	        	copyN(mSendData+2,gFlashData.serverIP,mSendLen-2);
			}
			else if(mCmd[0]=='A'&&mCmd[1]=='3')  //��ȡ�û�����flash����Ϣ
			{
				mSendLen = 10;
	        	copyN(mSendData+2,(uint_8*)(&gFlashData.sendFrequencySec),mSendLen-2);
			}
			else if(mCmd[0]=='B'&&mCmd[1]=='0')   //����flash�е�������Ϣ
			{
				copyN((uint_8 *)(gFlashData.equipName),(uint_8*)&(gRecvBuf[2]),mFlashLen);
				mFlashFlag = 1;
				mSendLen = 9;
	        	copyN(mSendData+2,"success",mSendLen-2);
			}
			else if(mCmd[0]=='B'&&mCmd[1]=='1')  //����flash�еĲ�Ʒ��Ϣ
			{
				copyN((uint_8 *)(gFlashData.equipName),(uint_8*)&(gRecvBuf[2]),124);
				mFlashFlag = 1;
				mSendLen = 9;
	        	copyN(mSendData+2,"success",mSendLen-2);
			}
			else if(mCmd[0]=='B'&&mCmd[1]=='2')  //����flash�еķ�������Ϣ
			{
				copyN((uint_8 *)(gFlashData.serverIP),(uint_8*)&(gRecvBuf[2]),30);
				mFlashFlag = 1;
				mSendLen = 9;
	        	copyN(mSendData+2,"success",mSendLen-2);
			}
			else if(mCmd[0]=='B'&&mCmd[1]=='3')  //�����û�����flash����Ϣ
			{
				copyN((uint_8 *)(&gFlashData.sendFrequencySec),(uint_8*)&(gRecvBuf[2]),8);
				mFlashFlag = 1;
				mSendLen = 9;
	        	copyN(mSendData+2,"success",mSendLen-2);
			}
			else if(mCmd[0]=='U'&&mCmd[1]=='0')
			{
				copyN(gFlashData.frameCmd,mCmd,2);
				if(gRecvLength == mUserLen)   //��Ϊ��֡����
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
			else                             //�������
			{
			}
			if(mSendLen>0)     //������Ҫ���͵�����
			{
				 mflag = uecom_send(mSendLen,mSendData); //�ṹ��ĵ�ַ��ֱ��ǿ��תΪ����ĵ�ַ
			}
            gRecvLength = 0;     //�������ݳ������㣬�����Ѿ���ȡ����ֹ�ظ���ȡ
			if(mflag==0)
				LCD_ShowString(4,300,BLUE,GRAY,"AHL Send Successfully              ");
			else if(mflag == 0xff)
				LCD_ShowString(4,300,BLUE,GRAY,"AHL Recv No Command         ");
			else
				LCD_ShowString(4,300,BLUE,GRAY,"Send Error:Send Data Not OK ");
             //�ж��Ƿ���Ҫдflash
             if(mFlashFlag == 1)
             {
         		flash_erase(63);
         		flash_write(63,0,mFlashLen,(uint_8*)(&gFlashData));
         		mFlashFlag = 0;
         		mLCDFlag=1;
             }
             if(mLCDFlag==1)
             {
          		LCD_ShowFlash();//����LCD�ϵ���ʾ
             }

        }
        //��5��ÿ��һ��ʱ��ι��
        if(mLoopCount%48000)
        {
        	if(ENABLE_WDOG)wdog_feed();    //���������Ź�����ι��
        }
        //��ѭ������======================================================
    }
}
//=====================================================================
//�������ƣ�flashData_get
//�������أ���
//����˵����data����Ҫ��ֵ�Ľṹ������
//���ܸ�Ҫ����ͨ��֡�ṹ��comData��ֵ����
//=====================================================================
void flashData_get(struct FlashData *data)
{
	//��ȡflash�е�������Ϣ
	flash_read_logic((uint_8*)data,63,0,sizeof(struct FlashData));
}

//=====================================================================
//�������ƣ�userData_init
//�������أ���
//����˵����data����Ҫ��ʼ���Ľṹ������
//���ܸ�Ҫ����ʼ���û�֡�ṹ��data
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
//�������ƣ�userData_get
//�������أ���
//����˵����data����Ҫ��ֵ�Ľṹ������
//���ܸ�Ҫ�����û�֡�ṹ��data��ֵ
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
    //��ȡ����ǿ��
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
//�������ƣ�LCD_ShowInit
//�������أ���
//����˵������
//���ܸ�Ҫ����ʼ��LCD�ϵ���ʾ������
//=====================================================================
void LCD_ShowInit()
{
//    uint_16 tmpAD;         //��ȡ����оƬ�¶�ADֵ
//    float tmp;             //��ʵ��оƬ�¶�ֵ
//    uint_8 flag,i;
//    uint_8 imsi[20];
//    uint_8 Community[30];
//    uint_8 sSignalPower[10];
//    uint_8 IntegerToChar[10];
    uint_8 temp[30] = {0};
	//��1������ȫ�ֵ�ɫΪ��ɫ
	LCD_DrawSurface(0,0,240,320,GRAY);
    //��2�����õ�һ������������
	aotu_panel(2,2,238,38,1);    //͹��
	LCD_ShowString(66,15,RED,GRAY,"���«IoT-GEC");
	if(ENABLE_WDOG)    //���������Ź�����ι��
		wdog_feed();
	//��3�����õڶ�������ͨ���޹�����
	aotu_panel(2,43,238,123,0);    //����
	LCD_ShowString(4,45,BLACK,GRAY,"��Type�� ");
	LCD_ShowString(4,65,BLACK,GRAY,"��Soft�� ");
	LCD_ShowString(4,85,BLACK,GRAY,"��eSIM�� ");
	LCD_ShowString(4,105,BLACK,GRAY,"��Temp(��)��  ");
	//��4�����õ���������ͨ����أ�
	aotu_panel(2,127,238,247,0);    //����
	LCD_ShowString(4,129,BLACK,GRAY,"��BSNum��");
	LCD_ShowString(4,149,BLACK,GRAY,"��Signal�� ");

	LCD_ShowString(4,169,BLACK,GRAY,"��I  P��");
	LCD_ShowString(4,189,BLACK,GRAY,"��PORT��");
	LCD_ShowString(4,209,BLACK,GRAY,"TSI:       Freq(s):   ");
	LCD_ShowString(4,229,BLACK,GRAY,"Time:");
	LCD_ShowString(49,230,BLUE,GRAY,"2018-07-01 21:55:00");
	LCD_ShowString(4,249,BLACK,GRAY,"RecvCount:");
	//��5�����õ�����������״̬��ʾ����
	aotu_panel(2,274,238,318,1);    //͹��
	LCD_ShowString(4,276,BLACK,GRAY,"Run State:");
//    //��6����ʾflash�е��ͺš�����汾�š�TSI������Ƶ�ʡ�IP��PORT
//	LCD_ShowFlash();
}

//����Ϊ�ڲ�����
//=====================================================================
//�������ƣ�get_gnss
//�������أ���
//����˵������
//���ܸ�Ҫ�������Ƿ���gnss��Ϣ����ͨ������������
//=====================================================================
//void get_gnss()
//{
//    double a,b;
//    char time[20];
//    double speed,high;
//	if(gnss_get_location(&a,&b))
//	{
//		uart_send_string(UART_2,"��ȡ��λ��Ϣʧ��\r\n");
//	}
//	else
//	{
//		uart_send_string(UART_2,"��ȡ��λ��Ϣ�ɹ�\r\n");
//	}
//
//	if(gnss_get_time(time))
//	{
//		uart_send_string(UART_2,"��ȡʱ��ʧ��\r\n");
//	}
//	else
//	{
//		uart_send_string(UART_2,"��ȡʱ����Ϣ�ɹ�\r\n");
//	}
//
//	if(gnss_get_speed(&speed))
//	{
//		uart_send_string(UART_2,"��ȡ�ٶ�ʧ��\r\n");
//	}
//	else
//	{
//		uart_send_string(UART_2,"��ȡ�ٶȳɹ�\r\n");
//	}
//
//	if(gnss_get_attitude(&high))
//	{
//		uart_send_string(UART_2,"��ȡ�߶�ʧ��\r\n");
//	}
//	else
//	{
//		uart_send_string(UART_2,"��ȡ�߶ȳɹ�\r\n");
//	}
//}

//=====================================================================
//�������ƣ�LCDShowTime
//�������أ���
//����˵����void
//���ܸ�Ҫ����Һ��������ʾʱ��
//=====================================================================
void LCDShowTime(void)
{
//	uint_8 i,j;
//	uint_8 sTime[30] = {0};
//	for(j=0,i=0;i<14;j++)
//	{
//		if(j == 4)
//		{
//			strcat(sTime,"��");
//			j++;
//		}
//		else if(j == 8)
//		{
//			strcat(sTime,"��");
//			j++;
//		}
//		else if(j == 12)
//		{
//			strcat(sTime,"��");
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
//�������ƣ�LCD_ShowFlash
//�������أ���
//����˵������
//���ܸ�Ҫ�����LCD����ʾ�Ĵ洢��flash�е���Ϣ������ʾ
//=====================================================================
void LCD_ShowFlash()
{
    uint_8 temp[30] = {0};
	//��6.1����ʾ�ͺ�
	copyN(temp,gFlashData.equipType,20);
	temp[20]=0;
	LCD_ShowString(70,45,BLUE,GRAY,temp);
	//��6.2����ʾ����汾
	copyN(temp,gFlashData.softVer,4);
	temp[4]=0;
	LCD_ShowString(70,65,BLUE,GRAY,temp);
	//��6.3����ʾ����Ƶ��
	LCD_ShowString(180,210,BLUE,GRAY,"          ");
	LCD_ShowString(180,210,BLUE,GRAY,
			(uint_8 *)myItoa(gFlashData.sendFrequencySec,temp));
	//��6.4����ʾIP
	copyN(temp,gFlashData.serverIP,15);
	temp[15]=0;
	LCD_ShowString(65,169,BLUE,GRAY,temp);
    //��6.5����ʾPORT
	copyN(temp,gFlashData.serverPort,5);
	temp[5]=0;
	LCD_ShowString(65,189,BLUE,GRAY,temp);
//	//��6.6����ʾTSI
//	LCD_ShowString(36,209,BLUE,GRAY,"0");
}

/*
 ֪ʶҪ�أ�
 1.main.c��һ�������ʵ����ִ����س������̲����漰�κλ�����оƬ���⡣
 ���ļ����д���������漰�����Ӳ���ͻ�������ͨ��������ع�����ʵ����Ӳ��
 ϵͳ�Ľ�����
 2.���ļ�����������룬һ��Ϊ������ʵ����Ҫ��ɾ�������������ݾ���
 ��Ŀ������и��ģ���һ��Ϊ������������������������Ŀ�޹أ���ͨ�õģ�
 ������ݾ�����Ŀ���и��ġ�
 3.���ļ��Ժ�GLOBLE_VAR�����˶��壬 �����ڰ���"includes.h"ͷ�ļ�ʱ
 �ᶨ��ȫ�ֱ������������ļ��а���"includes.h"ͷ�ļ�ʱ����������ͷ�ļ���
 ��ȫ�ֱ�����
 */
