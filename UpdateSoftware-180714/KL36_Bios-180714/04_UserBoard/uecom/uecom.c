//===================================================================
//�ļ����ƣ�uecom.c
//���ܸ�Ҫ��UE��������ͷ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//���¼�¼��2018-05-23   V2.0
//��        ע��������M6220
//===================================================================
#include "uecom.h"

#define AT_reBuf_length 200
static uint_8 AT_reBuf[200];            //���UE��ATָ��ķ��ؽ��
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
//�������ƣ�uecom_power
//�������أ���
//����˵����state:ͨ��ģ���Դ���������ȡֵΪ��ʹ�ú곣������
//         UECOM_OFF���ر�ͨ��ģ���Դ��
//         UECOM_ON����ͨ��ģ���Դ��
//         UECOM_REBOOT������ͨ��ģ�飨�ȹرգ���ʱ���ٿ�������
//���ܸ�Ҫ������ͨ��ģ�鹩��״̬
//�޸����ڣ���20180610��
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
//�������ƣ�uecom_init
//�������أ� 0���ɹ���1����UEģ�鴮��ͨѶʧ�ܣ�2����ȡSIM����IMSI��ʧ�ܣ�
//����˵���� ��
//���ܸ�Ҫ��uecomģ���ʼ������1��������ͨѶģ��Ĵ���ͨѶ����2����ȡSIM����IMSI�š�
//�޸����ڣ���20180706��WB��
//=====================================================================
uint_8 uecom_init(void)
{
	//��1�����������͸���ֵ
	uint_16 i;
	uint_8 ret_val ;
	uint_8 flag;
	//��2��ʹ�ô�����ͨ��ģ��ͨ�ţ����ȳ�ʼ������
	uart_init(UART_UE, 115200);   //��ʼ��UEģ�鴮�ڣ�������Ϊ115200
	uart_enable_re_int(UART_UE);  //��UEģ�鴮���ж�
	//��3������GPRSģ���Ƿ��������С��������У�������
	if(uecom_sendCmd((uint_8 *)AT,500,3))
	{
		uecom_power(UECOM_REBOOT);  //��UEģ�����¹���
		uecom_delay_ms(8000);
	}
	if(uecom_sendCmd((uint_8 *)AT,500,3))
		goto uecom_init_err1;     //UEģ��ͨѶʧ��
	//��4���رջ���
	if(uecom_sendCmd((uint_8 *)ATE_0,500,2))
		goto uecom_init_err1;     //�رջ���ʧ��
	//��5����ѯģ���IMEI,����IMEI�ŷ���dest�����ǰ15���ֽ�
	if(uecom_internal_getIMEI(IMEI))
		goto uecom_init_err1;     //IMEI��ѯʧ��
	//��6����ѯģ���IMSI,����IMSI�ŷ���dest�����15-29�±�Ŀռ���
	if(uecom_internal_getIMSI(IMSI))
		goto uecom_init_err2;      //IMSI��ѯʧ��
	//���ˣ�û��ʧ���˳����ɹ���
	ret_val = 0;
	goto uecom_init_exit;       //�ɹ���ret_val=0
	//�����˳�
uecom_init_err1:
	ret_val = 1;                       //��UEģ�鴮��ͨѶʧ��
	goto uecom_init_exit;
uecom_init_err2:
	ret_val = 2;                      //��ȡSIM����IMSI��ʧ��
	goto uecom_init_exit;
	//�˳���
uecom_init_exit:
    return ret_val;
}

//=====================================================================
//�������ƣ� uecom_ linkBase
//�������أ� 0���ɹ�����TCP���ӣ�1�����Ӳ���������
//����˵������
//���ܸ�Ҫ����������Ӫ�̵Ļ�վ����������������
//�޸����ڣ���20180706��WB��
//=====================================================================
uint_8 uecom_linkBase(void)
{
	//��1�����岢��ʼ��ʹ�õ��ı���
	uint_8 ret = 1;
	//��2������������ʽ
    if(uecom_sendCmd(CGDCONT,3000,2))
        goto uecom_linkBase_exit;
    //��3�����ź�����
    if(uecom_sendCmd(CGACT_1,3000,2))
        goto uecom_linkBase_exit;
    //��4�����ˣ�ִ�гɹ�
    ret = 0;
    uecom_linkBase_exit:
    return ret;
}

//=====================================================================
//�������ƣ�uecom_linkCS
//�������أ� 0���ɹ�����TCP���ӣ�1������TCP����ʧ��
//����˵���� IP:�����ӷ�������IP��ַ�� port:�����ӷ������Ķ˿ں�
//���ܸ�Ҫ����ָ���ķ������Ͷ˿ڽ���TCP���ӡ�
//�޸����ڣ���20180706��WB��
//=====================================================================
uint_8 uecom_linkCS(uint_8 *ip,uint_8* port)
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
			ret_val = 2;
			goto uecom_config_exit;
		}
	}
	//��4�����ˣ�ִ�гɹ�
	ret_val = 0;
uecom_config_exit:
	return ret_val;
}

//=====================================================================
//�������ƣ�uecom_send
//�������أ�  0�����ͳɹ���1������ʧ�ܣ�2�����س�ʱ������δ��ȷ��������ģʽ��
//����˵���� data:���������ݻ��������������
//		           length:���������ݵĳ���
//���ܸ�Ҫ��������ͨ���Ѿ�������TCP/UDPͨ�����ͳ�ȥ�����500���ֽڡ�
//�޸����ڣ���20180706��WB��
//=====================================================================
uint_8 uecom_send(uint_16 length, uint_8 *data)
{
	//��1����������������ֵ
	uint_8 ch,flag,k,i,l;
	uint_8 ret_val;
	char dataLen[5]="";
	char CIPSEND[20] = "";    //���÷��͵��ֽ���
	char buf[50];
	uint_8 frame[524];
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

//====================================================================
//�������ƣ�uecom_interrupt
//�������أ���
//����˵����ch�������жϽ��յ������ݣ�length�����յ����������ݳ��ȣ�
//                recvData���洢���յ�����������
//���ܸ�Ҫ����������Ҫ���ڴ����ж��С�����Ҫ���봮���жϽ��յ������ݡ�
//  �����������й���ʵ�־�������api����apiʵ�ֵĹ��ܣ�
//  ��1���������緢����������;��2�������ڲ�ʹ�ñ�api��ģ��������ݽ���
//�޸����ڣ���20180309��WB��
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
    uecom_recv(ch,length,recvData); //����GPRS���յ�������
}

//====================================================================
//�������ƣ�uecom_baseLocation
//�������أ�0����ȡ��վ��Ϣ�ɹ���1����ȡ�ź�ǿ��ʧ�ܣ�2����ȡ��վ��ʧ��
//����˵����retData���洢���ص���Ϣ,���ٷ���20���ֽڡ�
//          ��Ϣ��ɣ��ź�ǿ��(1���ֽ�)+��վ�ţ�19���ֽڣ�
//���ܸ�Ҫ����ȡ���վ��ص���Ϣ���ź�ǿ�Ⱥͻ�վ��
//�޸����ڣ���20180309��WB��
//=====================================================================
uint_8 uecom_baseInfo (uint_8 retData [20])
{
	//��1�����������͸���ֵ
	uint_8 i,j,*p,ret,ret_val;
	//��2����ȡ�ź�ǿ�ȣ�����ȡ���Σ����1��
	ret_val = 1;           //���ô��󷵻�Ϊ1
	for(j=0;j<3;j++)
	{
		//��2.1����ģ�鷢�ͻ�ȡ�ź�ǿ�ȵ�ָ��
		if( uecom_sendCmd((uint_8 *)CSQ,500,3))
			goto uecom_baseInfo_exit;
		//��2.2���ӽ��յ��������н������ź�ǿ�ȣ�תΪuint_8��ʽ��������*signalPower
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
	if(j==3)goto uecom_baseInfo_exit;  //��3�ξ�ʧ�ܣ�����Ϊ��ȡ�ź�ǿ��ʧ��
    //��3����û�վ��
	ret = 2;           //���ô��󷵻�Ϊ2
	//��3.1����ģ�鷢�ͻ�ȡ��վ�ŵ�ָ��
	if(uecom_sendCmd(CCED_1,500,3))
		goto uecom_baseInfo_exit;
    //��3.2���ӽ��յ��������н�������վ�ţ�������data������
    p = strstr(AT_reBuf,"+CCED:");   //����ַ�':'���ڵ�λ�ã����Ϊ��������ݡ�
    p += 6;
    j = 1;
    i = 0;
    //ȡ��5������֮ǰ�����ݲ�����data������
    while(i<5)
    {
        if(*p == ',')
            i++;
        retData[j++] = *p;
        p++;
    }
    if(j<=0)  //δ�ܳɹ�����������
		goto uecom_baseInfo_exit;   //�˳�����
    //���һ������ɾ����������Ϊ�ַ���������
    retData[j-1]=0;
    //��4���������ˣ�˵��ִ�гɹ�
    ret = 0;          //����ֵ��Ϊ0
    uecom_baseInfo_exit:
    return ret;
}

//====================================================================
//�������ƣ�uecom_ modelInfo
//�������أ�0����ȡģ����Ϣ�ɹ���1����ȡģ����Ϣʧ��
//����˵����retData���洢���ص���Ϣ,���ٷ���40���ֽڡ�
//          ��Ϣ��ɣ�IMSI (20���ֽ�)+IMEI��20���ֽڣ�
//���ܸ�Ҫ�������Ҫ��ģ����ص���Ϣ��������IMEI�ţ�IMSI��
//�޸����ڣ���20180309��WB��
//=====================================================================
uint_8 uecom_modelInfo (uint_8 retData[40])
{
	int i;
	//��1���ӱ�������ȫ�ֱ�����ȡ��IMEI����ֵ
	for(i=0;i<15;i++)
		retData[i]=IMEI[i];
	retData[i] = 0;    //������
	//��2���ӱ�������ȫ�ֱ�����ȡ��IMSI����ֵ
	for(i=20;i<35;i++)
		retData[i]=IMSI[i-20];
	retData[i] = 0;   //������
	return 0;
}

//=====================================================================
//�������ƣ�uecom_ gnssSwitch
//�������أ�0������GNSS�ɹ���1������GNSSʧ��
//����˵����state������GNSS�Ŀ���״̬��0���رգ�1����������2����������3����������
//          ����Ĭ��ʹ����������
//���ܸ�Ҫ������GNSS��״̬��������ر�GNSS�����趨������ʽ��
//�޸����ڣ���20180707��WB��
//=====================================================================
uint_8 uecom_gnssSwitch (uint_8 state)
{
	//��1������ʹ�õ��ı���
	uint_8  ret;
	ret = 1;    //Ĭ�Ϸ���ʧ��
	//��2�������ǹر�GNSS�������ȿ�����Դ
    if(state != 0)
	{
	    uecom_sendCmd(GPSACT_1,1000,2);
		uecom_delay_ms(500);    //��ʱ500ms����GNSSģ��
	}
	//��3�����ݴ���ֵ����GNSS
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
	//���ˣ��������������
	ret = 0;
	uecom_gnssSwitch_exit:
	return ret;
}

//====================================================================
//�������ƣ�uecom_ gnssGetInfo
//�������أ�0����ȡ��Ϣ�ɹ���1����ͨ��ģ��ͨ��ʧ�ܣ�2����ȡUTCʱ��ʧ�ܣ�
//                 3����ȡ��λ��Ϣʧ�ܣ�4����ȡ�ٶ���Ϣʧ�ܣ�5����ȡ������Ϣʧ��
//����˵����data���洢��õ�GNSS�����Ϣ�����ýṹ��ķ�ʽ��������4����Ա��
//          time��15���ֽڵ�uint_8���飩,����:��20180706155132����ʾ2018��7��6��15:51:32
//          latitude��double���ͣ���γ����Ϣ��longitude��double����������Ϣ��
//          speed��double���ͣ����ٶȣ���λΪ����ÿ�롣
//          attitude��double���ͣ������θ߶ȣ���λΪ����
//���ܸ�Ҫ�������GNSS��λ��ص���Ϣ��
//=====================================================================
uint_8 uecom_gnssGetInfo (UecomGnssInfo *data)     // UecomGnssInfoΪ�洢gnss��Ϣ�Ľṹ������
{
	//��1�����屾����ʹ�õı���
	uint_8 i,j,ret,index;
	double a,b,c,d;
	//��2������ATָ������С��λ��Ϣ����ೢ��3��
	ret = 1;        //����Ĭ�Ϸ���1
	if(uecom_sendCmd(GPSRMC,500,3))
		goto uecom_gnssGetInfo_exit;
	//��3����ͨ��ģ�鷵�������н�����UTCʱ��
	ret = 2;      //����Ĭ�Ϸ���2
	//��3.1���ж��Ƿ�����ȷ����
    if(strstr((char*)AT_reBuf,"20000101000000"))
    	goto uecom_gnssGetInfo_exit;
    //��3.2�����ˣ�����ȷʱ�䡣��������ʱ�䣬������ṹ���time������
    i=0;    j=0;
    while(i<14 && j<30)
    {
    	if(AT_reBuf[j]>='0' &&  AT_reBuf[j] <= '9')
    		data->time[i++] = AT_reBuf[j];
    	j++;
    }
    data->time[i]=0;
    //��4���ж�λ����Ϣ�Ƿ�Ϊ��Ч���ݡ�
    //��ע����ͨ�����жϣ���˵��λ�ã��ٶȣ����ε���Ϣ�ɿ��������ʾ���ɿ�
    //          UTCʱ�䲻��Ҫͨ�����ж�Ҳ��ɿ��������ͨ�����������жϣ��Ƚϳ�ֵ����
    ret = 3;
	i = strstr(AT_reBuf,"+GPSRMC:")-(char*)AT_reBuf;
	i+=24;
    if(AT_reBuf[i]!='A')
    	goto uecom_gnssGetInfo_exit;
    //���ˣ���õĶ�λ����ϢΪ��Ч��Ϣ
    //��5����ͨ��ģ�鷵�������н�����λ����Ϣ�������봫��Ľṹ��
    //��5.1����ȡγ����Ϣ��������ṹ���latitude��Ա��
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
	//��5.2����ȡ������Ϣ��������ṹ���longitude��Ա��
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
	//��6����ͨ��ģ�鷵�������н������ٶ���Ϣ�������봫��Ľṹ��
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
	//��7����ͨ��ģ�鷵�������н�����������Ϣ�������봫��Ľṹ��
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
	//���ˣ����ݽ������
    ret = 0;
    uecom_gnssGetInfo_exit:
    return ret;
}

//====================================================================
//�������ƣ�http_get
//�������أ�0�����get����ɹ���1����ʼ��httpʧ�ܣ�2������url����ʧ�ܣ�3����������ʧ�ܣ�
//                4����������ʧ�ܣ�5����������ʧ�ܣ�6����������ʧ�ܣ�7����÷���ʧ�ܣ�
//����˵����ip:Ŀ���������ַ��port :Ŀ���ַ��url:get��������ݡ�result:get���󷵻صĽ����
//                ���鳤����Ԥ�Ʒ��صĳ��ȣ��û�Ӧ��֪�������ݣ�*1.5��������
//���ܸ�Ҫ������http��get���󣬲������ؽ���洢��result��
//�޸����ڣ���2018-03-09��WB��
//=====================================================================
uint_8 uecom_httpGet (uint_8 ip[],uint_8 port[],uint_8 url[],uint_8 result[])
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
	if(uecom_linkCS(ip, port))
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
		if(strcmp(cmd,"SEND") && strcmp(cmd,"wait") )//��SEND�������жϷ��ͳɹ�
		{
			AT_reLength = 0;   //���¿�ʼ��������
			AT_reBuf[0]=0;
//			uart_send_string(UART_2,cmd);
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
	    		AT_reBuf[0]=0;
                break;
	        }
		}
		//���ˣ�˵��δ���յ���ȷ���ء�
    	j*=2;//��ʧ�ܣ��ȴ�ʱ��ӱ�
	    AT_reLength = 0;
		AT_reBuf[0]=0;
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

//====���ú���API
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
