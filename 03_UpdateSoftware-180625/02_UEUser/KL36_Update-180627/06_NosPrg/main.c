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
		1528455798,                                           //����ʱ���ʱ���
		"���ݴ�ѧ���������Ա",                        //�û���
		"13000000000",
		"122.112.217.197",
		"8093",
		600,
		0
};
//������ʵ����Ҫ��ɾ��main.cʹ�õ��ڲ�����������
void flashData_get(struct FlashData * data);     //��ȡflash�еĲ�����Ϣ�����浽data��
void userData_init(struct UserData *data);        //��ʼ���û���Ϣdata
void userData_get(struct UserData *data);        //��ȡ�û���Ϣdata
void LCDShowTime(void);    //LCD�ϸ���ʱ����ʾ
void LCD_ShowInit();            //��ʼ��LCD����ʾ����Ҫ�Ǳ�ǩ
void LCD_ShowGet();            //���LCD��ָ����ǩӦ����ʾ�����ݣ�����LCD����ʾ
void get_gnss();                    //���Ի��GNSS��Ϣ����ͨ�����ڴ���Ƿ�ɹ����
void LCD_ShowFlash();        //���LCD����ʾ�Ĵ洢��flash�е���Ϣ������ʾ

extern void LPTMR0_IRQ(void);
extern void TSI0_IRQ(void);

//��������һ������¿�����Ϊ����Ӵ˿�ʼ���У�ʵ�������������̼���壩
int main(void)
{
	//��1��������ʵ����Ҫ��ɾ������������ʹ�õı���������ʱ��׼��ֵ��
	uint_64 mSec;           //��ѭ��ʹ�õġ��롱
	uint_64 mMin;          //��ѭ��ʹ�õġ��֡�
	uint_64 mTmp;         //��ѭ��ʹ�õ���ʱ����
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
	uint_8 mflag;                  //��ѭ��ʹ�õı�־�жϱ���
	uint_8 mFlashFlag;         //�����Ƿ���Ҫ������д��flash
	uint_8 mLCDFlag;         //�����Ƿ���Ҫ����LCD
	uint_8 mSignalPower[10];   //�洢�ַ�ʽ���ź�ǿ��
	uint_8 mIntegerToChar[10];//�洢�ַ�����
	uint_64 mLoopCount;         //��¼ѭ������
	uint_8 mTouchNum[5];       //�洢�ַ�ʽ��TSI��������
	uint_8 mSendData[1000];   //���������ݻ�����
	uint_16 mSendLen;             //���������ݵĳ���
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

	light_init(LIGHT_RED,LIGHT_ON);  //��ʼ����ƣ�Ĭ�ϰ�
	//LCD_Init();             //LCD��ʼ��
	lptmr_init(500);    //LPTMR��ʱ����ʼ��Ϊ500����
	//uart_init(UART_2, 115200);  //��ʼ������2��������115200
	tsi_init(3,TSI_VALUE);           //��ʼ����������TSI��ͨ��3��,�趨������ֵ
	flash_init();                           //��ʼ��flash
	//��5��������ʵ����Ҫ��ɾ�� ʹ��ģ���ж�
	enable_lptmr_int();              //ʹ��LPTMR�ж�
	tsi_enable_re_int();               //ʹ��TSI�ж�
	setVector(LPTMR0_IRQn,(uint_32)&LPTMR0_IRQ);
	setVector(TSI0_IRQn,(uint_32)&TSI0_IRQ);

	//uart_enable_re_int(UART_2);    //ʹ�ܴ���2�����ж�
	//��6�������䡿�����ж�
	ENABLE_INTERRUPTS;
	//��7��������ʵ����Ҫ��ɾ����ѭ��ǰ�ĳ�ʼ������
	//��7.1�����������Ź�����ι��
	if(ENABLE_WDOG)
		wdog_feed();
	//��7.2����ȡflash�е�������Ϣ
	flashData_get(&gFlashData);
	//��7.3����ʼ���û�֡����
	userData_init(&gUserData);
	//��7.4���жϸ�λ״̬���������ݴ洢��flash��
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
	//��7.5����ʼLCD����ʾ���ݺʹ��������Ϣ
	//printf("��ǰ�Ŀ��Ź���λ����Ϊ��%ld\n",gFlashData.resetCount);
	//LCD_ShowInit();        //��ʼ��LCD��Ĭ����ʾ����
	//LCD_ShowGet();      //���LCD��Ҫ��ʾ��������Ϣ
	//����httpʹ�ã�1ACӲ��֧��
	//    http_get("122.112.217.197","88","http://122.112.217.197:88/AHL-location/FrameMessage.aspx?460,000,5280,e46a",datadd);
	//��ѭ����ʼ======================================================
	for(;;)
	{
		uint_8 i;
		uint_8 sTime[25] = {0};
		uint_8 showString[50]={0};
		mLoopCount++;
		//��1��һ�뵽��֮����еĲ���
		if (mSec!=gUserData.currentTime)    //1�뵽
		{
			//��1.1�����µ�ǰ��������LCD����ʾʱ��
			mSec=gUserData.currentTime;   //���������mSec
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
			//��3.1��UEģ���ʼ��
			//printf("��ʾH1-1001��ͨ��ģ�鿪ʼ��ʼ��\n");
			//LCD_DrawSurface(4,300,236,316,GRAY);
			//LCD_ShowString(4,300,BLUE,GRAY,"��ʾH1-1001,��ʼ��ʼ��...");
			mflag =    uecom_init(mRetdata,gFlashData.serverIP,gFlashData.serverPort);
			//(3.2) ����ʼ���ɹ������ȡ��Ҫ���͵�����mSendData
			if(mflag)  goto main_loop1;           //��ʼ��ʧ�ܣ�LCD��ʾ��ʾ
			//���ˣ�UEģ���ʼ���ɹ�
			//printf("��ʾH1-1002��ͨ��ģ���ʼ���ɹ�\n");
			//LCD_ShowString(4,300,BLUE,GRAY,"��ʾH1-1002����ʼ���ɹ�");
			//            flashData_get(&gFlashData);      //��ȡ������ϢgFlashData
			userData_get(&gUserData);        //��ȡ�û���ϢgUserData

			if(gFrameCmd[0]=='U'&&gFrameCmd[1]=='0')
			{
				mSendLen = mUserLen;
				memcpy(mSendData,&gUserData,mSendLen);
			}
			else if(gFrameCmd[0]=='U'&&gFrameCmd[1]=='1')
			{
				memcpy(mSendData,gFrameCmd,2);
				memcpy(mSendData+2,gUserData.IMSI,15);
				memcpy(mSendData+17,&gUserData.currentTime,8);
				memcpy(mSendData+25,&gUserData.mcuTemp,4);
				memcpy(mSendData+29,&gUserData.signalPower,1);
				memcpy(mSendData+30,&gUserData.bright,2);
				memcpy(mSendData+32,&gUserData.touchNum,2);
				memcpy(mSendData+34,gUserData.lbs_location,25);
				mSendLen = 59;
			}
			//��3.3����ʾ�ź�ǿ��
			//LCD_ShowString(105,150,BLUE,GRAY,"     ");
			//LCD_ShowString(105,150,BLUE,GRAY,(uint_8 *)myItoa(gUserData.signalPower,mSignalPower,10));
			//printf("ͨ��ģ���ź�ǿ�ȣ�%d\n",gUserData.signalPower);
			//��3.4��UEģ�鷢������
			//printf("��ʾH1-1003��ͨ��ģ�鿪ʼ��������\n");
			//LCD_DrawSurface(4,300,236,316,GRAY);
			//LCD_ShowString(4,300,BLUE,GRAY,"��ʾH1-1003����ʼ��������...");
			mflag = uecom_send(mSendLen,mSendData); //�ṹ��ĵ�ַ��ֱ��ǿ��תΪ����ĵ�ַ
			if(mflag)  goto main_loop2;                   //���ݷ���ʧ�ܣ�LCD��ʾ��ʾ
			//printf("��ʾH1-1004��ͨ��ģ�����ݷ��ͳɹ�\n");
			//LCD_DrawSurface(4,300,236,316,GRAY);
			//LCD_ShowString(4,300,BLUE,GRAY,"��ʾH1-1004�����ݷ��ͳɹ�");
			mWaitSecond = 0;                     //����������ݵȴ���ʱʱ��
			goto main_loop;
			main_loop1:
			//LCD��ʾ��F1-10xx��,uecom��ʼ��ʧ����ʾ
			if(mflag<=4)      //��ͨ��ģ��ͨ�Ų�����
				mi = 1;
			else if(mflag == 5) //SIM��������
				mi = 2;
			else if(mflag == 11)//�����������������������ʧ��
				mi = 3;
			//printf("��ʾF1-%d��UE��ʼ��ʧ��\n",1000+mi);
			//LCD_DrawSurface(4,300,236,316,GRAY);
			strcat(showString,"��ʾF1-");
			strcat(showString,(uint_8 *)myItoa(1000+mi,mIntegerToChar,10));
			strcat(showString,"��UE��ʼ��ʧ��");
			//LCD_ShowString(4,300,BLUE,GRAY,showString);
			goto main_loop;
			main_loop2:
			//LCD��ʾ��F1-20xx��,���ݷ���ʧ����ʾ
			//printf("��ʾF2-%d��UE��������ʧ��\n",2000+mflag);
			//LCD_DrawSurface(4,300,236,316,GRAY);
			strcat(showString,"��ʾF2-");
			strcat(showString,(uint_8 *)myItoa(2000+mflag,mIntegerToChar,10));
			strcat(showString,"��UE��������ʧ��");
			//LCD_ShowString(4,300,BLUE,GRAY,showString);
			goto main_loop;
			main_loop:
			mSendFlag = 0;         //�޸ķ��ͱ��
		}
		//��4���ж��Ƿ���յ���������������
		if(gRecvLength != 0)
		{
			uint_8 cmd[2];
			uint_8 sTmp[10];
			mRecvCount++;
			//LCD_ShowString(90,249,BLUE,GRAY,myItoa(mRecvCount,sTmp,10));
			//LCD_ShowString(4,300,BLUE,GRAY,"�յ�����!                    ");
			mflag = 0xff;
			mSendLen = 0;
			memcpy(cmd,gRecvBuf,2);
			memcpy(mSendData,gRecvBuf,2);
			if(cmd[0]=='A'&&cmd[1]=='0')          //��ȡflash�е�������Ϣ
			{
				mSendLen = mFlashLen+2;
				memcpy(mSendData+2,(uint_8*)(&gFlashData),mFlashLen);
			}
			else if(cmd[0]=='A'&&cmd[1]=='1')  //��ȡflash�еĲ�Ʒ��Ϣ
			{
				mSendLen = 145;
				memcpy(mSendData+2,gFlashData.equipName,mSendLen-2);
			}
			else if(cmd[0]=='A'&&cmd[1]=='2')  //��ȡflash�еķ�������Ϣ
			{
				mSendLen = 22;
				memcpy(mSendData+2,gFlashData.serverIP,mSendLen-2);
			}
			else if(cmd[0]=='A'&&cmd[1]=='3')  //��ȡ�û�����flash����Ϣ
			{
				mSendLen = 10;
				memcpy(mSendData+2,(uint_8*)(&gFlashData.sendFrequencySec),mSendLen-2);
			}
			else if(cmd[0]=='B'&&cmd[1]=='0')   //����flash�е�������Ϣ
			{
				memcpy((uint_8 *)(gFlashData.equipName),(uint_8*)&(gRecvBuf[2]),mFlashLen);
				mFlashFlag = 1;
				mSendLen = 9;
				memcpy(mSendData+2,"success",mSendLen-2);
			}
			else if(cmd[0]=='B'&&cmd[1]=='1')  //����flash�еĲ�Ʒ��Ϣ
			{
				memcpy((uint_8 *)(gFlashData.equipName),(uint_8*)&(gRecvBuf[2]),124);
				mFlashFlag = 1;
				mSendLen = 9;
				memcpy(mSendData+2,"success",mSendLen-2);
			}
			else if(cmd[0]=='B'&&cmd[1]=='2')  //����flash�еķ�������Ϣ
			{
				memcpy((uint_8 *)(gFlashData.serverIP),(uint_8*)&(gRecvBuf[2]),30);
				mFlashFlag = 1;
				mSendLen = 9;
				memcpy(mSendData+2,"success",mSendLen-2);
			}
			else if(cmd[0]=='B'&&cmd[1]=='3')  //�����û�����flash����Ϣ
			{
				memcpy((uint_8 *)(&gFlashData.sendFrequencySec),(uint_8*)&(gRecvBuf[2]),8);
				mFlashFlag = 1;
				mSendLen = 9;
				memcpy(mSendData+2,"success",mSendLen-2);
			}
			else if(cmd[0]=='U'&&cmd[1]=='0')
			{
				memcpy(gFrameCmd,cmd,2);
				if(gRecvLength == mUserLen)   //��Ϊ��֡����
					memcpy((uint_8*)(&gUserData),gRecvBuf,mUserLen);
				mLCDFlag=1;
			}
			else if(cmd[0]=='U'&&cmd[1]=='1')
			{
				memcpy(gFrameCmd,cmd,2);
				if(gRecvLength == 59)
				{
					memcpy(gUserData.cmd,gRecvBuf,2);
					memcpy(gUserData.IMSI,gRecvBuf+2,15);
					memcpy(&gUserData.currentTime,gRecvBuf+17,8);
					memcpy(&gUserData.mcuTemp,gRecvBuf+25,4);
					memcpy(&gUserData.signalPower,gRecvBuf+29,1);
					memcpy(&gUserData.bright,gRecvBuf+30,2);
					memcpy(&gUserData.touchNum,gRecvBuf+32,2);
					memcpy(gUserData.lbs_location,gRecvBuf+34,25);
				}
				mLCDFlag=1;
			}
			else                             //�������
			{
				if(gRecvLength == mUserLen)   //��Ϊ��֡���ݣ�������ǰ��
				{
					memcpy((uint_8*)(&gUserData),gRecvBuf,mUserLen);
				}
			}
			if(mSendLen>0)     //������Ҫ���͵�����
			{
				mflag = uecom_send(mSendLen,mSendData); //�ṹ��ĵ�ַ��ֱ��ǿ��תΪ����ĵ�ַ
			}
			gRecvLength = 0;     //�������ݳ������㣬�����Ѿ���ȡ����ֹ�ظ���ȡ
			//			if(mflag==0)
			//				LCD_ShowString(4,300,BLUE,GRAY,"���ݷ��ͳɹ�!                    ");
			//			else if(mflag == 0xff)
			//				LCD_ShowString(4,300,BLUE,GRAY,"δ�յ���������!                  ");
			//			else
			//				LCD_ShowString(4,300,BLUE,GRAY,"���ݷ���ʧ��!                    ");
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
#if ENABLE_WDOG == 1
			wdog_feed();
#endif
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
	strncpy(data->cmd,"U0",2);
	strncpy(gFrameCmd,data->cmd,2);
	data->sn = 0;
	strncpy(data->serverIP,gFlashData.serverIP,15);
	strncpy(data->serverPort,gFlashData.serverPort,5);
	data->currentTime = gFlashData.productTime;
	data->resetCount = gFlashData.resetCount;
	data->sendFrequencySec = gFlashData.sendFrequencySec;
	strncpy(data->userName,gFlashData.userName,20);
	strncpy(data->softVer,gFlashData.softVer,4);
	strncpy(data->equipName,gFlashData.equipName,30);
	strncpy(data->equipID,gFlashData.equipID,20);
	strncpy(data->equipType,gFlashData.equipType,20);
	strncpy(data->vendor,gFlashData.vendor,30);
	strncpy(data->phone,gFlashData.phone,11);
	data->touchNum = 0;
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
	static uint_32 sn = 4;

	data->sn = sn++;
	adc_init(MUXSEL_A,0,16,SAMPLE32);
	tmpAD = adc_read(26);
	data->mcuTemp = (int_32)((25.0-(tmpAD*3.3*1000/65535-719)/1.715)*10);
	//LCD_ShowString(105,105,BLUE,GRAY,(uint_8*)myFtoa(data->mcuTemp/10.0,1,tempertature));
	//��ȡ����ǿ��
	brightAD = adc_read(7);
	data->bright = brightAD;
	uecom_getIMSI(data->IMSI);
	uecom_getIMEI(data->IMEI);
	uecom_getSignalPower(&sPower);
	data->signalPower = sPower;
	uecom_getCurCommunity(data->lbs_location);


	//    adc_init(MUXSEL_A,0,16,SAMPLE32);
	//    tmpAD = adc_read(26);
	//    data->mcuTemp = (int_32)((25.0-(tmpAD*3.3*1000/65535-719)/1.715)*10);
	//    //��ȡ����ǿ��
	//    brightAD = adc_read(7);
	//    data->bright = brightAD;
	//    uecom_getIMEI(data->IMEI);
	//    uecom_getIMSI(data->IMSI);
	//    uecom_getSignalPower(&sPower);
	//    data->signalPower = sPower;
	//    uecom_getCurCommunity(data->lbs_location);

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
	//    uint_8 temp[30] = {0};
	//	//��1������ȫ�ֵ�ɫΪ��ɫ
	//	//LCD_DrawSurface(0,0,240,320,GRAY);
	//    //��2�����õ�һ������������
	//	//buttonUp(2,2,238,38);    //͹��
	//	//LCD_ShowString(66,15,RED,GRAY,"���«IoT-GEC");
	//	if(ENABLE_WDOG)    //���������Ź�����ι��
	//		wdog_feed();
	//	//��3�����õڶ�������ͨ���޹�����
	//	//buttonDown(2,43,238,123);    //����
	//	//LCD_ShowString(4,45,BLACK,GRAY,"���ͺš�");
	//	//LCD_ShowString(4,65,BLACK,GRAY,"������汾�š�");
	//	//LCD_ShowString(4,85,BLACK,GRAY,"��eSIM���š�");
	//	//LCD_ShowString(4,105,BLACK,GRAY,"��оƬ�¶ȡ�");
	//	//��4�����õ���������ͨ����أ�
	//	buttonDown(2,127,238,247);    //����
	//	LCD_ShowString(4,129,BLACK,GRAY,"����վ�š�");
	//	LCD_ShowString(4,149,BLACK,GRAY,"���ź�ǿ�ȡ�  ");
	//
	//	LCD_ShowString(4,169,BLACK,GRAY,"��IP  ��");
	//	LCD_ShowString(4,189,BLACK,GRAY,"��PORT��");
	//	LCD_ShowString(4,209,BLACK,GRAY,"TSI:     ����Ƶ��(��):");
	//	LCD_ShowString(4,229,BLACK,GRAY,"ʱ��:");
	//	LCD_ShowString(4,249,BLACK,GRAY,"���ռ���:");
	//	//��5�����õ�����������״̬��ʾ����
	//	buttonUp(2,274,238,318);    //͹��
	//	LCD_ShowString(4,276,BLACK,GRAY,"����״̬:");
	//    //��6����ʾflash�е��ͺš�����汾�š�TSI������Ƶ�ʡ�IP��PORT
	//	LCD_ShowFlash();
}

//=====================================================================
//�������ƣ�LCD_ShowGet
//�������أ���
//����˵������
//���ܸ�Ҫ�����LCD��ָ����ǩӦ����ʾ�����ݣ�����LCD����ʾ
//=====================================================================
void LCD_ShowGet()
{
	uint_16 tmpAD;         //��ȡ����оƬ�¶�ADֵ
	float tmp;             //��ʵ��оƬ�¶�ֵ
	uint_8 flag,i;
	uint_8 imsi[20];
	uint_8 Community[30];
	uint_8 sSignalPower[10];
	uint_8 IntegerToChar[10];
	uint_8 tempertature[6];
	//��7����ʾ�豸���¶�
	adc_init(MUXSEL_A,0,16,SAMPLE32);
	tmpAD = adc_read(26);
	tmp = 25.0-(tmpAD*3.3*1000/65535-719)/1.715;
	//����2λ��Ч����
	//printf("��ǰ��ƬоƬ�¶�Ϊ��%2.2f\n",tmp);
	sprintf(tempertature,"%f",tmp);
	//LCD_ShowString(105,105,BLUE,GRAY,(uint_8*)myFtoa(tmp,1,tempertature));
	//��8����ʾ�豸��IMSI��
	while(uecom_fast_init())	   //���ٳ�ʼ��UEģ��
	{
		//LCD_ShowString(4,300,BLUE,GRAY,"��ʼ��ʧ�ܣ��������³�ʼ��...");
	}
	flag = uecom_getIMSI(imsi+1);   //��ȡIMSI��
	if(!flag)         //��ȡIMSI�ųɹ�
	{
		imsi[0]=' ';   //��һ���ַ�Ϊ�ո�ռλ
		imsi[16]=0;    //�ַ����Ľ�����
		//printf("��ǰ��ƬIMSI��Ϊ��%s\n",imsi+1);  //�������IMSI��
		//LCD_ShowString(105,85,BLUE,GRAY,imsi+1);
	}
	//��9����ʾ�豸���ź�ǿ��
	for(i=0;i<5;i++)
	{
		uint_8 sPower;
		flag = uecom_getSignalPower(&sPower);
		if(flag == 0 && sPower!=0)
		{
			gUserData.signalPower = sPower;
			break;
		}
		Delay_ms(2000);
	}
	//printf("ͨ��ģ���ź�ǿ�ȣ�%d\n",gUserData.signalPower);
	//LCD_ShowString(105,150,BLUE,GRAY,(uint_8 *)myItoa(gUserData.signalPower,sSignalPower,10));
	//��10����ʾС����
	for(i=0;i<5;i++)
	{
		if(uecom_getCurCommunity(Community))
			continue;
		Community[17] = 0;
		//LCD_ShowString(89,129,BLUE,GRAY,Community);
		break;
	}
}

//����Ϊ�ڲ�����
//=====================================================================
//�������ƣ�get_gnss
//�������أ���
//����˵������
//���ܸ�Ҫ�������Ƿ���gnss��Ϣ����ͨ������������
//=====================================================================
void get_gnss()
{
	double a,b;
	char time[20];
	double speed,high;
	if(gnss_get_location(&a,&b))
	{
		uart_send_string(UART_2,"��ȡ��λ��Ϣʧ��\r\n");
	}
	else
	{
		uart_send_string(UART_2,"��ȡ��λ��Ϣ�ɹ�\r\n");
	}

	if(gnss_get_time(time))
	{
		uart_send_string(UART_2,"��ȡʱ��ʧ��\r\n");
	}
	else
	{
		uart_send_string(UART_2,"��ȡʱ����Ϣ�ɹ�\r\n");
	}

	if(gnss_get_speed(&speed))
	{
		uart_send_string(UART_2,"��ȡ�ٶ�ʧ��\r\n");
	}
	else
	{
		uart_send_string(UART_2,"��ȡ�ٶȳɹ�\r\n");
	}

	if(gnss_get_attitude(&high))
	{
		uart_send_string(UART_2,"��ȡ�߶�ʧ��\r\n");
	}
	else
	{
		uart_send_string(UART_2,"��ȡ�߶ȳɹ�\r\n");
	}
}

//=====================================================================
//�������ƣ�LCDShowTime
//�������أ���
//����˵����void
//���ܸ�Ҫ����Һ��������ʾʱ��
//=====================================================================
void LCDShowTime(void)
{
	uint_8 i,j;
	uint_8 sTime[30] = {0};
	for(j=0,i=0;i<14;j++)
	{
		if(j == 4)
		{
			strcat(sTime,"��");
			j++;
		}
		else if(j == 8)
		{
			strcat(sTime,"��");
			j++;
		}
		else if(j == 12)
		{
			strcat(sTime,"��");
			j++;
		}
		else if(j == 16)
		{
			strcat(sTime,":");
		}
		else if(j == 19)
		{
			strcat(sTime,":");
		}
		else
			sTime[j] = gTime[i++]+'0';
	}
	//LCD_ShowString(49,230,BLUE,GRAY,sTime);
}

//=====================================================================
//�������ƣ�LCD_ShowFlash
//�������أ���
//����˵������
//���ܸ�Ҫ�����LCD����ʾ�Ĵ洢��flash�е���Ϣ������ʾ
//=====================================================================
void LCD_ShowFlash()
{
	//    uint_8 temp[30] = {0};
	//	//��6.1����ʾ�ͺ�
	//	strncpy(temp,gFlashData.equipType,20);
	//	temp[20]=0;
	//	//LCD_ShowString(70,45,BLUE,GRAY,temp);
	//	//��6.2����ʾ����汾
	//	strncpy(temp,gFlashData.softVer,4);
	//	temp[4]=0;
	//	LCD_ShowString(118,65,BLUE,GRAY,temp);
	//	//��6.3����ʾTSI
	//	LCD_ShowString(36,209,BLUE,GRAY,"0");
	//	//��6.4����ʾ����Ƶ��
	//	LCD_ShowString(180,210,BLUE,GRAY,"          ");
	//	LCD_ShowString(180,210,BLUE,GRAY,
	//			(uint_8 *)myItoa(gFlashData.sendFrequencySec,temp,10));
	//	//��6.3����ʾIP
	//	strncpy(temp,gFlashData.serverIP,15);
	//	temp[15]=0;
	//	LCD_ShowString(65,169,BLUE,GRAY,temp);
	//    //��6.3����ʾPORT
	//	strncpy(temp,gFlashData.serverPort,5);
	//	temp[5]=0;
	//	LCD_ShowString(65,189,BLUE,GRAY,temp);
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




////˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
////==============================================================
//
////��������GLOBLE_VAR�궨�弰������ͷ�ļ�
//#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ���������ȫ�ֱ���������ʹ������
//#include "includes.h"   //������ͷ�ļ�
//
////������ʵ����Ҫ��ɾ��main.cʹ�õ��ڲ�����������
//
////��������һ������¿�����Ϊ����Ӵ˿�ʼ���У�ʵ�������������̼���壩
//int main(void)
//{
//    //��1��������ʵ����Ҫ��ɾ������������ʹ�õı���������ʱ��׼��ֵ��
//	uint_32 mCount;
//	uint_64 ct;
//	uint_8 date[15];
//
//    //��2���������� �����ж�
//    DISABLE_INTERRUPTS;
//
//    cpy_nvic();
//    funPointerInit();
//
//    gpio_init(LIGHT_RED,GPIO_OUTPUT,LIGHT_OFF);
//    flash_init();
//    //��3�� ������ʵ����Ҫ��ɾ�����йر�������ֵ
//	mCount = 0;
//
//    //��3.1����������ʹ�õı�������ֵ
//
//    //��3.2����ȫ�ֱ�������ֵ
//
//    //��6��������ʵ����Ҫ��ɾ�� ʹ��ģ���ж�
//    //��7�������䡿�����ж�
//    ENABLE_INTERRUPTS;
//    //��8�����LCD��Ҫ��ʾ�����ݲ���ʾ
//
//    //��ѭ����ʼ======================================================
//    for(;;)
//    {
//		mCount++;
//		if(mCount > 0xfffff)
//		{
//			mCount = 0;
//			gpio_reverse(LIGHT_RED);
//			//light_change(LIGHT_RED);
//			uart_send_string(UART_2,"CCTest\n");
//			systemTimeGet(date);
//		}
//        //��ѭ������======================================================
//    }
//}

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
