//===================================================================
//�ļ����ƣ�uecom.c
//���ܸ�Ҫ��UE��������ͷ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//���¼�¼��2018-05-23   V2.0
//��        ע��������M6220
//===================================================================
#include "uecom.h"

#define AT_reBuf_length 1100
static uint_8 AT_reBuf[1100];            //���UE��ATָ��ķ��ؽ��
static uint_8 AT_reLength = 0;         //ATָ��ص����ݳ���
static uint_8 AT_haveCommand=0;//��־��ǰ�Ƿ���ATָ���
static uint_8 isGPRSdata = 0;
static uint_8 ATSendFlag;                 //���һ��ATָ���ѷ���
static uint_8 frameHead[2] = "V!"; //֡ͷ
static uint_8 frameTail[2] = "S$";     //֡β
static uint_8 IMSI[15]={0};                //�洢��ʼ�����ȡ����IMSI��
static uint_8 IMEI[15]={0};                //�洢��ʼ�����ȡ����IMEI��

//======================�ڲ���������====================================
//�ں������崦����ϸ˵��
uint_8 uecom_sendCmd(uint_8 *cmd,uint_16 maxDelayMs,uint_16 maxTimes);
void uint_to_str(uint_32 ch,char *str);
void frameEncode(uint_8 *imsi,uint_8 *data,uint_16 dlen,uint_8 *frame,uint_16 *framelen);
uint_8 frameDecode(uint_8 *frame,uint_8 *cdata,uint_16 *len);
uint_16 crc16(uint_8 *ptr,uint_16 len);
uint_8 uecom_internal_getIMEI(uint_8 *dest);
uint_8 uecom_internal_getIMSI(uint_8 *dest);
uint_8 uecom_recv(uint_8 ch,uint_16 *dataLen, uint_8 *data);
//======================�ڲ�������������==================================


//========================��������======================================
	//������ʹ�õ���ATָ��
	char AT[]   = "AT\r\n";                                          //������GPRSģ��ͨѶ��ָ��
	char CGSN[] = "AT+CGSN\r\n";                            //��ȡ�豸��IMEI��ָ��
	char CGDCONT[] =                                             //��������������Ϊnet������������
			"AT+CGDCONT=1, \"IP\", \"CMMTM\"\r\n";
	char CGACT_1[] ="AT+CGACT=1,1\r\n";                  //����PDP�������վ�������ip��ַ
//	char CGACT_0[] ="AT+CGACT=0\r\n";                //�ر�PDP
	char CIFSR[] ="AT+CIFSR\r\n";                    //��ȡ���䵽��IP��ַ
	char CIPCLOSE[] ="AT+CIPCLOSE\r\n";                    //�ر�TCP
	char CIPSEND[] ="AT+CIPSEND\r\n";                    //����TCP����
	char CIMI[] = "AT+CIMI\r\n";                              //��ȡ�豸��IMSI��ָ��
	char ATE_0[] = "ATE0\r\n";                                 //�رջ���
	char ATE_1[] = "ATE1\r\n";                                 //�򿪻���
	char CCED_1[] = "AT+CCED=0,1\r\n";            //��ȡ��ǰС����Ϣ
	char CSQ[] = "AT+CSQ\r\n";                             //����ź�ǿ��
    //��http��ص�
	char HTTPINIT[] = "AT+HTTPINIT\r\n";
	char HTTPSEND_0[] = "AT+HTTPSEND=0\r\n";
	char HTTPREAD[] = "AT+HTTPREAD?\r\n";  //?���ʺţ���KDS�±�����
	//��GNSS��ص�ATָ��
	char GPSACT_0[] = "AT+GPSACT=0\r\n";
	char GPSACT_1[] = "AT+GPSACT=1\r\n";
	char GPSPOS[] = "AT+GPSPOS\r\n";
	char GPSRMC[] = "AT+GPSRMC\r\n";
	char GPSRESTART_1[] = "AT+GPSRESTART=1\r\n";
	char GPSRESTART_2[] = "AT+GPSRESTART=2\r\n";
	char GPSRESTART_3[] = "AT+GPSRESTART=3\r\n";

	//AT�����Ƿ�ִ�гɹ��ı�־
	char OK[] = "OK";
	char ERROR[] = "ERROR";
//========================������������====================================

//=====================================================================
//�������ƣ�uecom_switch
//�������أ� 0�������ɹ���1������ʧ��
//����˵���� state:UEģ�鹩��״̬����ȡֵΪUECOM_OFF���رա�
//        UECOM_ON���رա�UECOM_REBOOT��������
//���ܸ�Ҫ������ͨ��ģ�鹩��״̬
//�ڲ����ã�gpio_init��uecom_delay_ms
//�޸����ڣ�2017.09.25,WB,CC
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
//�������ƣ�uecom_init
//�������أ� 0���ɹ���1����UEģ�鴮��ͨѶʧ�ܣ�3���رջ���ʧ�ܣ�4��IMEI��ѯʧ�ܣ�
//                    5��IMSI��ѯʧ�ܣ�11:����TCP����ʧ��
//����˵���� dest���������ݵĵ�ַ���ܳ�74�ֽڣ�IMEI,15���ֽڣ�0~14��
//                    IMSI,15���ֽڣ�15~29��
//                    IP:�����������IP��ַ
//                    PORT:����������Ķ˿ں�
//���ܸ�Ҫ��uecomģ���ʼ���������������MCU��ͨѶģ��Ļ���ͨѶ����֮��ִ�и��ź�����
//        ���������봫���IP��ַ�Ͷ˿ںŽ���TCP���ӡ���ִ�б������е���������֮ǰ����
//        ���ñ���������uecom_fast_init������
//�ڲ����ã�uecom_fast_init��uecom_config
//�޸����ڣ�2017.08.11,QHJ,CC��2017.09.25��WB��2018.05.23��WB��
//��        ע������ֵ����������Ϊ������AHL�汾�ķ�����Ϣһ��
//=====================================================================
uint_8 uecom_init(uint_8 *dest,uint_8 *IP,uint_8* PORT)
{
	uint_8 ret_val,flag;
	//��1���������ͨ��ģ��Ŀ����Ͷ�ȡIMEI��IMSI���ź�ǿ��
	flag = uecom_fast_init();
	if(flag)
	{
		ret_val = flag;
		goto uecom_init_exit;
	}
	memcpy(dest,IMEI,15);      //����ȡ����IMEI�Ŵ洢��ȫ�ֱ���IMEI��
	memcpy(dest+15,IMSI,15); //����ȡ����IMSI�Ŵ洢��ȫ�ֱ���IMSI�С�
	//��7������TCP����
	flag = uecom_config(IP,PORT);
	if(flag)
	{
		ret_val = 10 + flag;      //����TCP����ʧ��
		goto uecom_init_exit;  //����TCP����ʧ��
	}
	//���ˣ�û��ʧ���˳����ɹ���
	ret_val = 0;
	//�˳���
uecom_init_exit:
    return ret_val;
}

//=====================================================================
//�������ƣ�uecom_fast_init
//�������أ� 0���ɹ���1����UEģ�鴮��ͨѶʧ�ܣ�3���رջ���ʧ�ܣ�
//                    4����ȡIMEI��ʧ�ܣ�5����ȡIMSI��ʧ�ܣ�
//����˵���� ��
//���ܸ�Ҫ��uecomģ����ٳ�ʼ�������������MCU��ͨѶģ��Ļ���ͨѶ���ܣ���ִ�б������е�
//        ��������֮ǰ������ñ���������uecom_init������
//�ڲ����ã�uart_init��uecom_sendCmd
//�޸����ڣ�2017.09.28��WB��
//��        ע������ֵ����������Ϊ������AHL�汾�ķ�����Ϣһ��
//=====================================================================
uint_8 uecom_fast_init(void)
{
	//��1�����������͸���ֵ
	uint_16 i;
	uint_8 ret_val ;
	uint_8 flag;
	//��2��ʹ�ô�����ͨ��ģ��ͨ�ţ����ȳ�ʼ������
	uart_init(UART_UE, 115200);   //��ʼ��UEģ�鴮�ڣ�������Ϊ115200
	uart_enable_re_int(UART_UE);  //��UEģ�鴮���ж�
	//��3������GPRSģ���Ƿ��������С��������У�������
	if(uecom_sendCmd((uint_8 *)AT,100,4))
		uecom_switch(UECOM_REBOOT);  //��UEģ�����¹���
	if(uecom_sendCmd((uint_8 *)AT,100,4))
		goto uecom_init_err1;     //UEģ��ͨѶʧ��
	//��4���رջ���
	if(uecom_sendCmd((uint_8 *)ATE_0,200,3))
		goto uecom_init_err3;     //�رջ���ʧ��
	//��5����ѯģ���IMEI,����IMEI�ŷ���dest�����ǰ15���ֽ�
	if(uecom_internal_getIMEI(IMEI))
		goto uecom_init_err4;     //IMEI��ѯʧ��
	//��6����ѯģ���IMSI,����IMSI�ŷ���dest�����15-29�±�Ŀռ���
	if(uecom_internal_getIMSI(IMSI))
		goto uecom_init_err5;      //IMSI��ѯʧ��
	//���ˣ�û��ʧ���˳����ɹ���
	ret_val = 0;
	goto uecom_init_exit;      //�ɹ���ret_val=0
	//�����˳�
uecom_init_err1:
	ret_val = 1;                             //ATָ���ʧ��
	goto uecom_init_exit;
uecom_init_err2:
	ret_val = 2;                            //UEģ��Ĳ���������ʧ��
	goto uecom_init_exit;
uecom_init_err3:
	ret_val = 3;                           //�رջ���ʧ��
	goto uecom_init_exit;
uecom_init_err4:
	ret_val = 4;             //IMEI��ѯʧ��
	goto uecom_init_exit;
uecom_init_err5:
	ret_val = 5;              //IMSI��ѯʧ��
	goto uecom_init_exit;
	//�˳���
uecom_init_exit:
    return ret_val;
}

//=====================================================================
//�������ƣ�uecom_deInit
//�������أ���
//����˵������
//���ܸ�Ҫ��UE����ʼ�����رմ����жϣ�ȡ�����ڵ����Ÿ��ã����ر�UEģ��Ĺ��硣
//�ڲ����ã�uart_enable_re_int��uecom_switch
//�޸����ڣ�2017.09.22,QHJ,CC��2017.09.25��WB��
//=====================================================================
void uecom_deInit(void)
{
	uart_disable_re_int(UART_UE);                   //�ر�UEģ�鴮���ж�
	uecom_switch(UECOM_OFF);                   //�ر�UEģ��Ĺ���
}

//=====================================================================
//�������ƣ�uecom_send
//�������أ�  0�����ͳɹ���1������ʧ�ܣ�2�����س�ʱ������δ��ȷ��������ģʽ��
//����˵���� data:���������ݻ��������������
//		           length:���������ݵĳ���
//���ܸ�Ҫ��������ͨ���Ѿ�������TCP/UDPͨ�����ͳ�ȥ
//�ڲ����ã�uint_to_str��uart_send_string��uecom_delay_ms��uart_sendN��
//       uart_init��uart_enable_re_int��uart_disable_re_int��
//       uart_re1��uart_send1��uart_enable_re_int
//�޸����ڣ�2017.08.11,QHJ,CC��2017.09.25��WB��
//=====================================================================
uint_8 uecom_send(uint_16 length, uint_8 *data)
{
	//��1����������������ֵ
	uint_8 ch,flag,k,i,l;
	uint_8 ret_val;
	char dataLen[5]="";
	char CIPSEND[20] = "";    //���÷��͵��ֽ���
	char buf[50];
	uint_8 frame[1024];
	uint_16 frameLen;
    //��2����֡
	frameEncode(IMSI,data, length,frame,&frameLen);
	//��3������Ҫ���͵����ݳ��ȷ���UEģ�飬����������ģʽ
	uint_to_str(frameLen,dataLen);        //�����������ݵĳ���ת��Ϊ�ַ�����ʽ
	strcat(CIPSEND,"AT+CIPSEND=");		//��ȡ��������ģʽ��ATָ���ַ���
	strcat(CIPSEND, dataLen);
	strcat(CIPSEND, "\r\n");
	uart_send_string(UART_UE, CIPSEND);	//ָ�����͵������ֽڸ����������뷢��ģʽ
	//��4���ȴ�����ģʽ�ɹ�����
	if(uecom_sendCmd("wait",3000,1))
	{
			ret_val = 1;
			goto uecom_send_exit;          //����ʧ��
	}
	AT_haveCommand=1;  //��λAT�����־
    AT_reLength = 0;
    //��5����ʼ��������
	uart_sendN(UART_UE,frameLen,frame);    //��������
	//��6���ȴ����ط��ͳɹ�
	if(uecom_sendCmd("SEND",6000,1))
	{
		ret_val = 2;
		goto uecom_send_exit;         //����ʧ��
	}
	ret_val = 0;
	uecom_send_exit:
	return ret_val;
}

//==================================================================
//�������ƣ�uecom_getIMEI
//�������أ� 0����ȡIMEI�ɹ���1����ȡIMEIʧ��
//����˵����dest����ŷ��ص�IMEI�ţ�15λ
//���ܸ�Ҫ����ȡ�豸IMEI��
//�ڲ����ã�uecom_sendCmd
//�޸����ڣ�2017.08.11,QHJ,CC
//==================================================================
uint_8 uecom_getIMEI(uint_8 *dest)
{
	int i;
	for(i=0;i<15&&IMEI[i];i++)
		dest[i]=IMEI[i];
	return 0;
}

//==================================================================
//�������ƣ�uecom_getIMSI
//�������أ�0����ȡIMSI�ɹ���1����ȡIMSIʧ��
//����˵����dest����ŷ��ص�IMSI�ţ�15λ
//���ܸ�Ҫ����ȡ�豸IMSI��
//�ڲ����ã�uecom_sendCmd
//�޸����ڣ�2017.08.11,QHJ,CC
//==================================================================
uint_8 uecom_getIMSI(uint_8 *dest)
{
	int i;
	for(i=0;i<15&&IMEI[i];i++)
		dest[i]=IMSI[i];
	return 0;
}

//===================================================================
//�������ƣ�uecom_getFirmVer
//�������أ�0����ȡ�̼��汾�ųɹ���1����ȡ�̼��汾��ʧ��
//����˵����firmVer����ŷ��صĹ̼��汾�ţ�25λ
//���ܸ�Ҫ����ȡ�̼��汾�ţ�����ͳһ�ӿڣ���û��ʵ��
//�ڲ����ã���
//�޸����ڣ�2017.08.11,QHJ,CC
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
//�������ƣ�uecom_getSignalPower
//�������أ�0����ȡ��վ�ź�ǿ�ȳɹ���1����ȡ��վ�ź�ǿ��ʧ��
//����˵����signalPower����ŷ��صĻ�վ�ź�ǿ�Ⱥţ��ٷֱȸ�ʽ
//���ܸ�Ҫ����ȡ��վ�ź�ǿ��
//�ڲ����ã�uecom_sendCmd
//�޸����ڣ�2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_getSignalPower(uint_8 *signalPower)
{
	//��1�����������͸���ֵ
	uint_8 i,j,ret_val;

	for(j=0;j<3;j++)
	{
		//��2����ȡ�ź�ǿ��
		if( uecom_sendCmd((uint_8 *)CSQ,200,3))
		{
			ret_val = 1;
			goto uecom_getSignalPower_exit;
		}
		//��3����ȡ���ź�ǿ�ȣ�תΪuint_8��ʽ��������*signalPower
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
	//�˳���
uecom_getSignalPower_exit:
	return ret_val;
}

//=====================================================================
//�������ƣ�uecom_getUEIP
//�������أ�0����ȡ��վ����UE��IP��ַ�ɹ���1����ȡ��վ����UE��IP��ַʧ��
//����˵����ip����ŷ��ػ�վ����UE��IP��ַ��15λ
//���ܸ�Ҫ����ȡ��վ����UE��IP��ַ
//�ڲ����ã�uecom_sendCmd
//�޸����ڣ�2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_getUEIP(uint_8 *ip)
{
	//��1�����������͸���ֵ
	uint_8 i, k;
	uint_8 ret_val;

	//��2����ip�����е��������
	for(i = 0; i<=15; i++)
	{
		ip[i] = 0;
	}
	//��3�����ģ��ֵõ�IP��ַ
    if(uecom_sendCmd((uint_8 *)CIFSR,200,1))
    {
    	uecom_sendCmd((uint_8 *)CGACT_1,500,2);
    	if(uecom_sendCmd((uint_8 *)CIFSR,200,1))
    	{
    		ret_val = 1;
    		goto uecom_getUEIP_exit;
    	}
    }
	//��4����ȡ��õ�IP��ַ
    i = strstr(AT_reBuf,OK)-(char*)AT_reBuf;
    while(AT_reBuf[i]=='\r'||AT_reBuf[i]=='\n'  //��λi��λ��
    		||(AT_reBuf[i]>='0'&&AT_reBuf[i]<='9')||AT_reBuf[i]=='.')i--;
	k = 0;   //��¼���յ���ip��ַ��λ������ֹ����OK��ȴû��ip��ַ�����
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
		goto uecom_getUEIP_exit;//��Ȼ����OK�����ǣ���û�л�ȡ��IP��ַ
	}
	//���ˣ�û��ʧ���˳����ɹ���
	ret_val = 0;
	//�˳���
uecom_getUEIP_exit:
	return ret_val;
}

//=====================================================================
//�������ƣ�uecom_getBSNum
//�������أ�0����ȡ��վ�ųɹ���1����ȡ��վ��ʧ��
//����˵����bsNum����ŷ��صĻ�վ�ţ�2λ
//���ܸ�Ҫ��������ͳһ�ӿ�
//�ڲ����ã���
//�޸����ڣ�2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_getBSNum(uint_32 *bsNum)
{
	*bsNum = 0;
	return 0;
}

//====================================================================
//�������ƣ�uecom_getCurCommunity
//�������أ�0����ȡ��ǰС���ɹ���1����ȡ��ǰС��ʧ��
//����˵����data����ŷ��ص�С��������Ϣ����ʽΪ:mcc(�ƶ����Һ���),mnc(�ƶ��������),
//                   lac(λ��������),cell id(С����),bsic(��վ��ʾ��)������:460,000,5280,e467,32
//                   ע������lac��cell idΪʮ�����Ʊ�ʾ��data��������ռ25���ֽ�
//���ܸ�Ҫ����õ�ǰ��С����Ϣ
//�޸����ڣ���2018-03-09������WB��
//=====================================================================
uint_8 uecom_getCurCommunity(uint_8 *data)
{
    //��1��������ʱ����
	uint_8 i, j,*p,ret;
	//��2�����ͻ�ȡС����Ϣ�����෢���Σ�
	if(uecom_sendCmd(CCED_1,200,3))
	{
		ret = 1;              //����ֵ��Ϊ1
		goto uecom_getCurCommunity_exit;   //�˳�����
	}
    //��3���������ص����飬������data������
    p = strstr(AT_reBuf,"+CCED:");   //����ַ�':'���ڵ�λ�ã����Ϊ��������ݡ�
    p += 6;
    j = 0;
    i = 0;
    //ȡ��5������֮ǰ�����ݲ�����data������
    while(i<5)
    {
        if(*p == ',')
            i++;
        data[j++] = *p;
        p++;
    }
    if(j<=0)  //δ�ܳɹ�����������
    {
		ret = 2;              //����ֵ��Ϊ2
		goto uecom_getCurCommunity_exit;   //�˳�����
    }
    //���һ������ɾ����������Ϊ�ַ���������
    data[j-1]=0;
    ret = 0;          //����ֵ��Ϊ0
    //��4���˳�����
    uecom_getCurCommunity_exit:
    return ret;
}

//====================================================================
//�������ƣ�http_get
//�������أ�0�����get����ɹ���1����ʼ��httpʧ�ܣ�2������url����ʧ�ܣ�3����������ʧ�ܣ�
//                   4����������ʧ�ܣ�5����������ʧ�ܣ�6����������ʧ�ܣ�7����÷���ʧ�ܣ�
//����˵����data����ŷ��ص�С��������Ϣ����ʽΪ:mcc(�ƶ����Һ���),mnc(�ƶ��������),
//                   lac(λ��������),cell id(С����),bsic(��վ��ʾ��)������:460,000,5280,e467,32
//                   ע������lac��cell idΪʮ�����Ʊ�ʾ��data��������ռ25���ֽ�
//���ܸ�Ҫ����õ�ǰ��С����Ϣ
//�޸����ڣ���2018-03-09������WB��
//=====================================================================
uint_8 http_get(uint_8 ip[],uint_8 port[],uint_8 url[],uint_8 result[])
{
	uint_8 ret;
	uint_8 allUrl[500] = "AT+HTTPPARA=\"URL\",\"";
	uint_16 len = 0 , i;
	uint_8 *p;
	//��1����ʼ��http
	if(uecom_sendCmd(HTTPINIT,2000,2))
	{
		ret = 1;
		goto http_get_exit;
	}
	//��2�����ݲ���
	strcat(allUrl,url);
	strcat(allUrl,"\"\r\n");
	if(uecom_sendCmd(allUrl,2000,2))
	{
		ret = 2;
		goto http_get_exit;
	}
	//��3����������
	if(uecom_sendCmd(CGDCONT,2000,2))
	{
		ret = 3;
		goto http_get_exit;
	}
	//��4����������
	if(uecom_sendCmd(CGACT_1,5000,3))
	{
		ret = 4;
		goto http_get_exit;
	}
	//��5����������
	if(uecom_config(ip, port))
	{
		ret = 5;
		goto http_get_exit;
	}
	//��6����������
	if(uecom_sendCmd(HTTPSEND_0,5000,3))
	{
		ret = 6;
		goto http_get_exit;
	}
	uecom_delay_ms(500);
	//��7����÷���
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
	//�������гɹ�
	ret = 0;
	http_get_exit:
	return ret;
}

//====================================================================
//�������ƣ�uecom_interrupt
//�������أ���
//����˵����ch�������жϽ��յ������ݣ�length�����յ���gprs���ݳ��ȣ�
//                   recvData���洢���յ���gprs����
//���ܸ�Ҫ����������Ҫ���ڴ����ж��С�����Ҫ���봮���жϽ��յ������ݡ��������Ĺ���ʵ������
//                   �ڱ�������
//�޸����ڣ���2018-03-09������WB��
//=====================================================================
void uecom_interrupt(uint_8 ch,uint_16 *length,uint_8 recvData[])
{
	//�������ݵ���������
	if(AT_haveCommand)// && !isGPRSdata)
	{
		AT_reBuf[AT_reLength] = ch;    //�洢���յ������ݵ�������AT_reBuf��
		AT_reLength = (AT_reLength+1)%AT_reBuf_length;//���յ��������±����
		AT_reBuf[AT_reLength] = 0;
	}
//	else
    uecom_recv(ch,length,recvData); //����GPRS���յ�������
}

//=====================================================================
//�������ƣ�gnss_init
//�������أ� ��
//����˵���� ��
//���ܸ�Ҫ������ͨ��ģ�鹩��״̬
//�޸����ڣ�2018.03.05,WB
//=====================================================================
void gnss_init(uint_8 way)
{
	//��1��ʹ�ô�����GNSSģ��ͨ�ţ����ȳ�ʼ������
//	uart_init(UART_UE, 115200);   //��ʼ��UEģ�鴮�ڣ�������Ϊ115200
//	uart_enable_re_int(UART_UE);  //��UEģ�鴮���ж�
    //��2����gnss��Դ
	if(uecom_sendCmd(GPSACT_1,1000,2))
	{
		goto gnss_init_exit;
	}
	uecom_delay_ms(500);    //��ʱ500ms����GNSSģ��
	//��3������������GNSSģ��
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
//�������ƣ�gnss_get_time
//�������أ�0�����UTCʱ��ɹ���1������ATָ��ʧ�ܣ�2�����UTCʱ��ʧ�ܡ�
//����˵���� ��ʼΪ������14�ֽڵ��ַ������洢��ǰ��UTCʱ�䣬��ʽΪ��������ʱ���롣
//                    �磺20180305055948������Ϊ��������СʱӦ��+8
//���ܸ�Ҫ����õ�ǰ��UTCʱ��
//�޸����ڣ�2018.03.05,WB
//=====================================================================
uint_8 gnss_get_time(char * time)
{
	//��1�����屾����ʹ�õı���
	uint_8 i,j,retVal,index;
	double a,b,c,d;
	//��2������ATָ������С��λ��Ϣ����ೢ��3��
	if(uecom_sendCmd(GPSRMC,500,2))
	{
		gnss_init(4);//���³�ʼ��
		if(uecom_sendCmd(GPSRMC,500,3))
		{
			retVal = 1;
			goto gnss_get_time_error;
		}
	}
	//��3����δ�����ȷʱ�����˳�
    if(strstr((char*)AT_reBuf,"20000101000000"))
    {
    	retVal = 2;
    	goto gnss_get_time_error;
    }
    //��4����ȡ��ʱ����Ϣ
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
//�������ƣ�gnss_get_location
//�������أ�0�����λ�óɹ���1������ATָ��ʧ�ܣ�2�����λ��ʧ�ܡ�
//����˵���� latitude ���洢��������γ����Ϣ��double���ͣ���
//                    longitude���洢�������ľ�����Ϣ��double���ͣ���
//���ܸ�Ҫ����õ�ǰ���ڵ�λ��
//�޸����ڣ�2018.03.05,WB
//=====================================================================
uint_8 gnss_get_location(double *latitude, double *longitude)
{
	//��1�����屾����ʹ�õı���
	uint_8 i,j,retVal,index;
	double a,b,c,d;
	//��2������ATָ������С��λ��Ϣ����ೢ��3��
	if(uecom_sendCmd(GPSRMC,500,2))
	{
		gnss_init(4);//���³�ʼ��
		if(uecom_sendCmd(GPSRMC,500,3))
		{
			retVal = 1;
			goto gnss_get_location_error;
		}
	}
	//��3����δ�����Ч�������˳�
	i = strstr(AT_reBuf,"+GPSRMC:")-(char*)AT_reBuf;
	i+=24;
    if(AT_reBuf[i]!='A')
    {
    	retVal = 2;
    	goto gnss_get_location_error;
    }
    //��4����ȡ��λ����Ϣ������AT����ؽ����ȡ��
    //��4.1��������ȡγ����Ϣ��������latitude������
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
	//��4.2��������ȡ������Ϣ��������longitude������
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
//�������ƣ�gnss_get_speed
//�������أ�0������ٶȳɹ���1������ATָ��ʧ�ܣ�2������ٶ�ʧ�ܡ�
//����˵���� ��ʼΪ������10�ֽڵ��ַ������洢��ǰ���ٶȡ���λΪ���ڣ�1��=0.5144444��/�룩
//���ܸ�Ҫ����õ�ǰ���ٶ�
//�޸����ڣ�2018.03.05,WB
//=====================================================================
uint_8 gnss_get_speed(double * speed)
{
	//��1�����屾����ʹ�õı���
	uint_8 i,j,retVal,index;
	double a,b,c,d;
	//��2������ATָ������С��λ��Ϣ����ೢ��3��
	if(uecom_sendCmd(GPSRMC,500,2))
	{
		gnss_init(4);//���³�ʼ��
		if(uecom_sendCmd(GPSRMC,500,3))
		{
			retVal = 1;
			goto gnss_get_speed_error;
		}

	}
	//��3����δ�����Ч�������˳�
	i = strstr(AT_reBuf,"+GPSRMC:")-(char*)AT_reBuf;
	i+=24;
    if(AT_reBuf[i]!='A')
    {
    	retVal = 2;
    	goto gnss_get_speed_error;
    }
    //��4����ȡ���ٶ���Ϣ������AT����ؽ����ȡ��
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
//�������ƣ�gnss_get_attitude
//�������أ�0����ú��γɹ���1������ATָ��ʧ�ܣ�2����ú���ʧ�ܡ�
//����˵���� ��ʼΪ������10�ֽڵ��ַ������洢��ǰ�ĺ��θ߶ȡ���λΪ����
//���ܸ�Ҫ����õ�ǰ�ĺ��θ߶�
//�޸����ڣ�2018.03.05,WB
//=====================================================================
uint_8 gnss_get_attitude(double *attitude)
{
	//��1�����屾����ʹ�õı���
	uint_8 i,j,retVal,index;
	double a,b,c,d;
	//��2������ATָ������С��λ��Ϣ����ೢ��3��
	if(uecom_sendCmd(GPSRMC,500,2))
	{
		gnss_init(4);//���³�ʼ��
		if(uecom_sendCmd(GPSRMC,500,3))
		{
			retVal = 1;
			goto gnss_get_attitude_error;
		}
	}
	//��3����δ�����Ч�������˳�
	i = strstr(AT_reBuf,"+GPSRMC:")-(char*)AT_reBuf;
	i+=24;
    if(AT_reBuf[i]!='A')
    {
    	retVal = 2;
    	goto gnss_get_attitude_error;
    }
    //��4����ȡ��������Ϣ������AT����ؽ����ȡ��
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
//===================����Ϊ�ڲ�����=======================================
//=====================================================================

//=====================================================================
//�������ƣ�uecom_sendCmd
//�������أ�0��ָ��ͳɹ�;1��ָ���ʧ��
//����˵����cmd����Ҫ���͵�ATָ����׵�ַ;buf[]:����ַ������ATָ��ؽ��,�����Ҫ200
//                   ���ֽ�
//���ܸ�Ҫ������ATָ���ȡ������Ϣ����෢���������ÿ���������ȴ�3�롣
//�ڲ����ã�strTrim
//�޸����ڣ�2017.06.30��WYH��2017.08.11,QHJ,CC����2017.09.25��WB��
//=====================================================================
uint_8 uecom_sendCmd(uint_8 *cmd,uint_16 maxDelayMs,uint_16 maxTimes)
{
	//�жϷ�ʽʵ��
    //��1�����������͸���ֵ
	uint_8 ret;
	uint_16 i,j,k,count;
	uint_8 wait[30]="OK";
	AT_haveCommand=1;  //��λAT�����־
	while(AT_haveCommand!=1)
	{
		AT_haveCommand=1;  //��λAT�����־
		uecom_delay_ms(1);
	}
    AT_reLength = 0;
	uart_enable_re_int(UART_UE);//�����ж�
	//�����ȴ����ַ�����Ĭ��Ϊ"OK"
	if(!strcmp(cmd,"SEND"))
		strcpy(wait,"SEND");
	else if(strstr(cmd,"AT+CIPSTART")!=NULL)
		strcpy(wait,"CONNECT OK");
	else if(strstr(cmd,"wait")!=NULL)
		strcpy(wait,">");
	uecom_delay_ms(10);//��ֹ����AT����͹���Ƶ��
	j = maxDelayMs/10;
	for(count=0;count<maxTimes;count++)
	{
		AT_reBuf[0]=0;
		if(strcmp(cmd,"SEND") && strcmp(cmd,"wait") )//��SEND�������жϷ��ͳɹ�
		{
			uart_send_string(UART_UE,cmd); //ͨ�����ڷ���ATָ��
		}
		//�ȴ�maxDelayMs����
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
	        	uecom_delay_ms(100);//����100����
	        	uart_send_string(UART_UE,AT); //ͨ�����ڷ���ATָ��
	        	uecom_delay_ms(100);//����100����
	        	AT_reLength = 0;   //���¿�ʼ��������
	        	break;
	        }
		}
		//���ˣ�˵��δ���յ���ȷ���ء�
    	j*=2;//��ʧ�ܣ��ȴ�ʱ��ӱ�
	}
	ret = 1;
	uecom_sendCmd_exit:
	AT_haveCommand=0;
	return ret;
}

//==================================================================
//�������ƣ�uecom_internal_getIMEI
//�������أ� 0����ȡIMEI�ɹ���1����ȡIMEIʧ�ܣ�2������IMEI��ʧ��
//����˵����dest����ŷ��ص�IMEI�ţ�15λ
//���ܸ�Ҫ����ȡ�豸IMEI��
//�ڲ����ã�uecom_sendCmd
//�޸����ڣ�2017.08.11,QHJ,CC
//==================================================================
uint_8 uecom_internal_getIMEI(uint_8 *dest)
{
	//��1�����������͸���ֵ
	uint_8 ret_val;
	uint_16 i,k;

	//��2����ȡIMEI��
	dest[15] = 0;				      //�ַ����Ľ�β��
	if(uecom_sendCmd((uint_8 *)CGSN,500,2))
	{
		uecom_sendCmd((uint_8 *)AT,200,2);
		if(uecom_sendCmd((uint_8 *)CGSN,500,2))
		{
			ret_val = 1;
			goto uecom_internal_getIMEI_exit;      //��ȡIMEI��ʧ��
		}
	}
	//��3����IMEI�Ŵ洢��dest������
	//��λIMEI����ʼλ��
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
	if(k < 15)    //�趨IMEI����С����Ϊ15
	{
			ret_val = 2;
			goto uecom_internal_getIMEI_exit;      //����IMEI��ʧ��
	}
	ret_val = 0;
	//�˳���
uecom_internal_getIMEI_exit:
	return ret_val;
}

//==================================================================
//�������ƣ�uecom_internal_getIMSI
//�������أ�0����ȡIMSI�ɹ���1����ȡIMSIʧ��
//����˵����dest����ŷ��ص�IMSI�ţ�15λ
//���ܸ�Ҫ����ȡ�豸IMSI��
//�ڲ����ã�uecom_sendCmd
//�޸����ڣ�2017.08.11,QHJ,CC
//==================================================================
uint_8 uecom_internal_getIMSI(uint_8 *dest)
{
	//��1�����������͸���ֵ
	uint_8 ret_val;
	int_16 i,k;

	dest[14] = 0;

	//��2����ȡIMSI��
	if(uecom_sendCmd((uint_8 *)CIMI,5000,2))
	{
		uecom_sendCmd((uint_8 *)AT,200,2);
		if(uecom_sendCmd((uint_8 *)CIMI,5000,1))
		{
			ret_val = 1;
			goto uecom_internal_getIMSI_exit;     //��ȡIMSI��ʧ��
		}
	}
	//��3����IMSI�Ŵ洢��dest������
	//��λIMSI����ʼλ��
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
	//�˳���
uecom_internal_getIMSI_exit:
	return ret_val;
}

//=====================================================================
//�������ƣ�uecom_config
//�������أ�  0���ɹ�����TCP���ӣ�
//       1������TCP����ʧ��
//����˵���� IP:�����ӷ�������IP��ַ
//        port:�����ӷ������Ķ˿ں�
//���ܸ�Ҫ����ָ���ķ������˿ڽ���TCP����
//�ڲ����ã�uecom_sendCmd��uart_send_string��uecom_delay_ms
//�޸����ڣ�2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_config(uint_8 *ip,uint_8* port)
{
	//��1�����������͸���ֵ
	uint_8 i;
	uint_8 flag;
	uint_8 ret_val;
	uint_8 CIPSTART[50] = "";				//����TCP���ӵĵ�ַ�Ͷ˿�
	uint_8 loc;

	//��2���齨ATָ��
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
	//��3������TCP����
	uecom_sendCmd(CIPCLOSE,2000,1);//���ȹر�TCP
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
	//�˳���
uecom_config_exit:
	return ret_val;
}

//======================================================================
//�������ƣ�uecom_default_delay_ms
//�������أ���
//����˵������
//���ܸ�Ҫ����ʱ - ���뼶
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
//�������ƣ�uint_to_str
//�������أ���
//����˵����ch:��ת����������		*str�����ת������ַ���
//���ܸ�Ҫ�����޷�������ת��Ϊ�ַ���
//======================================================================
void uint_to_str(uint_32 ch,char *str)
{
	int i,j,sign;
	char s[10];
	if((sign=ch)<0)//��¼����
		ch=-ch;//ʹn��Ϊ����
	i=0;
	do{
	       s[i++]=ch%10+'0';//ȡ��һ������
	}
	while ((ch/=10)>0);//ɾ��������
	if(sign<0)
	  s[i++]='-';
	for(j=i-1;j>=0;j--)//���ɵ�����������ģ�����Ҫ�������
	       str[i-1-j]=s[j];
	str[i]=0;
}


//=====================================================================
//�������ƣ�frameEncode
//���ܸ�Ҫ��ʵ�ִ��������ݵ���֡,�����������ݼ���֡ͷ��֡����֡β�Լ�У����Ϣ
//       frame=֡ͷ+IMSI+֡��+data+CRCУ����+֡β
//����˵����imsi���������ݵ��豸��IMSI��
//       data:����֡������ͷָ��
//       dlen:����֡�����ݳ���
//       frame:��֮֡��Ĵ���������֡
//       framelen:��֡������ݳ���
//�������أ�0���ɹ�
//=====================================================================
void frameEncode(uint_8 *imsi,uint_8 *data,uint_16 dlen,uint_8 *frame,uint_16 *framelen)
{
	uint_16 crc,len;

	//��֡����
	//֡ͷ
	frame[0] = frameHead[0];
	frame[1] = frameHead[1];
	len = 2;
	//IMSI��
	strncpy(frame+2,imsi,15);
	len += 15;
    //֡��
	frame[len++] = dlen>>8;
	frame[len++] = dlen;
	//֡����
	memcpy(frame+19,data,dlen);
	len += dlen;
	//CRCУ����
	crc = crc16(frame+2,dlen+17);
	frame[len++] = crc>>8;
	frame[len++] = crc;
	//֡β
	frame[len++] = frameTail[0];
	frame[len++] = frameTail[1];
	frame[len] = 0;
	*framelen = len;
}

//=====================================================================
//�������ƣ�crc16
//���ܸ�Ҫ�������ݽ���16λ��CRCУ�飬����У���Ľ��ֵ
//����˵����ptr:��ҪУ������ݻ�����
//                len:��Ҫ��������ݳ���
//�������أ�����õ���У��ֵ
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
//�������ƣ�uecom_recv
//�������أ�0�����յ���ͨ��TCP/UDP���������ݣ�1��������֡��2�������ݴ���
//����˵����ch:���ڽ��յ�������
//                   *dataLen:�洢���յ������ݳ���
//                   *data:�����յ������ݴ�ŵ������鴮��;
//���ܸ�Ҫ�������յ������ݽ��н�֡��������Ч���ݴ���data����֮�У��ɴ��ڵ�uart�жϵ��á�
//�ڲ����ã�uart_init��uart_disable_re_int,uart_re1��
//�޸����ڣ�2017.08.11,QHJ,CC��2017.09.25��WB��
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
	    //��֡
		if(index>recvLength)   //�������ֵ
		{
        	index = 0;
        	length = 0;
    		ret_val = 2;
    		flag = 0;//���µȴ���������
    		isGPRSdata = 0;
    		goto uecom_recv_exit;
		}
		if((index == 0 && ch != frameHead[0]) ||
				(index == 1 && ch != frameHead[1]))//δ���յ����ݻ���δ����֡ͷ
		{
	    	index = 0;
	    	length = 0;
			ret_val = 2;     //�������ݴ���
			isGPRSdata = 0;
			goto uecom_recv_exit;
		}
		data[index++] = ch;//��������
		if(index == 19)//��ȡ��Ч���ݳ���
		{
			length = ((uint_16)data[17]<<8) + data[18];
		}
	    if(length != 0 && index >= length+23)//���յ������ݴﵽһ֡���ȡ�23Ϊ֡ͷ֡β�ȳ���
	    {
	    	//CRCУ��
	    	crc = crc16(data+2,length+17);
	    	CRC[0] = (crc>>8)&0xff;
	    	CRC[1] = crc&0xff;

	    	if(data[index-2]!=frameTail[0] || data[index-1]!= frameTail[1]//δ����֡β
	    		||	CRC[0] != data[length+19] || CRC[1] != data[length+20])//CRC�������
	    	{
	        	index = 0;
	        	length = 0;
	    		ret_val = 2;
	    		flag = 0;//���µȴ���������
	    		isGPRSdata = 0;
	    		goto uecom_recv_exit;
	    	}
	    	for(i=0;i<length;i++)
	    	{
	    		data[i] = data[i+19];//19Ϊ��Ч�ֽ�ǰ�����ݳ���
	    	}
	    	*dataLen = length;
	    	index = 0;
	    	length = 0;
	    	ret_val = 0;
	    	flag = 0;//���µȴ���������
	    	isGPRSdata = 0;
	    	goto uecom_recv_exit;
	    }
	    *dataLen = 0;//������֡
	    ret_val = 1;
		break;
	}
uecom_recv_exit:
    return ret_val;
}
