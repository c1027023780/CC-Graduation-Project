//===================================================================
//�ļ����ƣ�uecom.c��uecom����Դ�����ļ���
//===================================================================
#include "uecom.h"    //������Ӧ����ͷ�ļ���˵����ͷ�ļ���

//�ļ�������
static uint_8 AT_reBuf[512];     //���UE��ATָ��ķ��ؽ��
static uint_8 AT_reFlag = 0;      //ATָ��ɹ����صı�־
static uint_8 AT_reLength = 0;      //ATָ��ص����ݳ���
static uint_8 curATCommand[100] = {0};  //�洢��ǰ��AT����
static uint_8 ATSendFlag;                    //���һ��ATָ���ѷ���
static uint_8 frameHead[2] = "V!";    //֡ͷ
static uint_8 frameTail[2] = "S$";        //֡β
static uint_8 IMSI[16]={0};                   //�洢��ʼ�����ȡ����IMSI��
static uint_8 IMEI[16]={0};                  //�洢��ʼ�����ȡ����IMEI��
//�����ṩ����ı�������ͷ�ļ����������˴�����
uint_16 gRecvLength = 0;        //���յ���GPRS���ݣ�Ϊ0ʱ��ʾû�н��յ��������ʾ���յ������ݳ���
uint_8   gRecvBuf[600];          //�洢���յ���GPRS���ݣ����1000���ֽڣ���Ϊ֡�ṹռ��Լ24���ֽڣ�


//======================�ڲ���������====================================
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
//======================�ڲ�������������==================================

//========================��������======================================
    //������ʹ�õ���ATָ��
    char AT[]   = "AT\r\n";               //������GPRSģ��ͨѶ��ָ��
    char IPR[] = "AT+IPR=115200\r\n";     //��ģ�鲨��������Ϊ115200
    char CGSN[] = "AT+CGSN\r\n";          //��ȡ�豸��IMEI��ָ��
    char CGCLASS_B[] ="AT+CGCLASS=\"B\"\r\n";  //��������������ΪB��
    char CGDCONT[] =                           //��������������Ϊnet������������
            "AT+CGDCONT=1, \"IP\", \"CMNET\"\r\n";
    char CGACT_1[] ="AT+CGACT=1\r\n";      //����PDP�������վ�������ip��ַ
    char CGACT_0[] ="AT+CGACT=0\r\n";      //�ر�PDP
    char QILOCIP[] ="AT+QILOCIP\r\n";      //��ȡ���䵽��IP��ַ
    char QICLOSE[] = "AT+QICLOSE\r\n";
    char CIPCLOSE_0[] = "AT+CIPCLOSE=0\r\n";
    char CIPCLOSE_1[] = "AT+CIPCLOSE=1\r\n";
    char CIPCLOSE_2[] = "AT+CIPCLOSE=2\r\n";
    char CIMI[] = "AT+CIMI\r\n";       //��ȡ�豸��IMSI��ָ��
    char CCED_1[] = "AT+CCED=0,1\r\n";  //��ȡ��ǰС����Ϣ��2018-03-09����
    char ATE_0[] = "ATE0\r\n";          //�رջ���
    char CSQ[] = "AT+CSQ\r\n";          //����ź�ǿ��
    //AT�����Ƿ�ִ�гɹ��ı�־
    char OK[] = "OK";
    char ERROR[] = "ERROR";
//========================������������====================================

//���ݺ�UART_UE����ʹ�õ��жϺ���
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
//�������ƣ�uecom_switch
//�������أ� 0�������ɹ���1������ʧ��
//����˵���� state:UEģ�鹩��״̬����ȡֵΪUECOM_OFF���رա�
//        UECOM_ON���رա�UECOM_REBOOT��������
//���ܸ�Ҫ������ͨ��ģ�鹩��״̬
//�ڲ����ã�gpio_init��Delay_ms
//�޸����ڣ�2017.09.25,WB,CC
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
//�������ƣ�uecom_init
//�������أ� 0���ɹ���1����UEģ�鴮��ͨѶʧ�ܣ�2��UEģ��Ĳ���������ʧ�ܣ�3���رջ���ʧ�ܣ�
//        4��IMEI��ѯʧ�ܣ�5��IMSI��ѯʧ�ܣ�6������ģ��Ĺ�������ΪB��ʧ�ܣ�
//        7������������ʽΪnetʧ�ܣ�8������PDPʧ�ܣ�9�����ģ��ֵõ�IP��ַʧ�ܣ�
//        10��ATָ���OK�����ǲ�û�л�ȡ��IP��ַ��11:����TCP����ʧ��
//����˵���� dest���������ݵĵ�ַ���ܳ�74�ֽڣ�IMEI,15���ֽڣ�0~14��
//             IMSI,15���ֽڣ�15~29��firmVer��25���ֽڣ�30~54��
//             signalPower��2���ֽڣ�55~56��bsNum��2���ֽڣ�57~58��
//             UEIP��15���ֽڣ�59~73
//       IP:�����������IP��ַ
//       PORT:����������Ķ˿ں�
//���ܸ�Ҫ��uecomģ���ʼ���������������MCU��ͨѶģ��Ļ���ͨѶ����֮��ִ�и��ź�����
//        ���������봫���IP��ַ�Ͷ˿ںŽ���TCP���ӡ���ִ�б������е���������֮ǰ����
//        ���ñ���������uecom_fast_init������
//�ڲ����ã�uart_init��uecom_sendCmd��uecom_internal_getIMEI��
//       uecom_internal_getIMSI�� uecom_link��uecom_config
//�޸����ڣ�2017.08.11,QHJ,CC��2017.09.25��WB��
//====================================================================
uint_8 uecom_init(uint_8 *dest,uint_8 *IP,uint_8* PORT)
{
    //��1�����������͸���ֵ
    uint_16 i;
    uint_8 ret_val ;
    uint_8 flag;
    //��2��ʹ�ô�����ͨ��ģ��ͨ�ţ����ȳ�ʼ������
    uart_init(UART_UE, 115200);   //��ʼ��UEģ�鴮�ڣ�������Ϊ115200
    uart_enable_re_int(UART_UE);  //��UEģ�鴮���ж�
    uecom_switch(UECOM_REBOOT);   //��UEģ�����¹���
    //��3��������GPRSģ���ͨѶ
    if(uecom_sendCmd((uint_8 *)AT,AT_reBuf,1000)
        	&& uecom_sendCmd((uint_8 *)AT,AT_reBuf,2000)
    	    && uecom_sendCmd((uint_8 *)AT,AT_reBuf,2000))
            goto uecom_init_err1;     //UEģ��ͨѶʧ��
    //��4������UEģ��Ĳ�����Ϊ115200
    if(uecom_sendCmd((uint_8 *)IPR,AT_reBuf,1000))
        goto uecom_init_err2;     //UEģ��Ĳ���������ʧ��
    //��5���رջ��ԣ�����AT����֮��UEģ�鲻���ԭ��������أ�
    if(uecom_sendCmd((uint_8 *)ATE_0,AT_reBuf,1000))
        goto uecom_init_err3;     //�رջ���ʧ��
    //��6����ѯģ���IMEI,����IMEI�ŷ���dest�����ǰ15���ֽ�
    if(uecom_internal_getIMEI(dest))
        goto uecom_init_err4;     //IMEI��ѯʧ��
    memcpy(IMEI,dest,15);      //����ȡ����IMEI�Ŵ洢��ȫ�ֱ���IMEI��
    //��7����ѯģ���IMSI,����IMSI�ŷ���dest�����15-29�±�Ŀռ���
    if(uecom_internal_getIMSI(dest+15))
        goto uecom_init_err5;      //IMSI��ѯʧ��
    memcpy(IMSI,dest+15,15); //����ȡ����IMSI�Ŵ洢��ȫ�ֱ���IMSI�С�
    //��8�����վ�������������ź�����
    flag = uecom_link(dest+30);
    if(flag) goto uecom_init_err6;  //���ź�����ʧ��
    //��9������TCP����
    flag = uecom_config(IP,PORT);
    if(flag) goto uecom_init_err7;  //����TCP����ʧ��

    //���ˣ�û��ʧ���˳����ɹ���
    ret_val = 0;
    goto uecom_init_exit;      //�ɹ���ret_val=0

     //�����˳�
uecom_init_err1:
    ret_val = 1;             //ATָ���ʧ��
    goto uecom_init_exit;
uecom_init_err2:
    ret_val = 2;             //UEģ��Ĳ���������ʧ��
    goto uecom_init_exit;
uecom_init_err3:
    ret_val = 3;             //�رջ���ʧ��
    goto uecom_init_exit;
uecom_init_err4:
    ret_val = 4;             //IMEI��ѯʧ��
    goto uecom_init_exit;
uecom_init_err5:
    ret_val = 5;              //IMSI��ѯʧ��
    goto uecom_init_exit;
uecom_init_err6:
    ret_val = 5 + flag;       //���ź�����ʧ��
    goto uecom_init_exit;
uecom_init_err7:
    ret_val = 10 + flag;      //����TCP����ʧ��
    goto uecom_init_exit;
    //�˳���
uecom_init_exit:
    return ret_val;
}

//====================================================================
//�������ƣ�uecom_fast_init
//�������أ� 0���ɹ���1����UEģ�鴮��ͨѶʧ�ܣ�2��UEģ��Ĳ���������ʧ�ܣ�3���رջ���ʧ�ܣ�
//����˵���� ��
//���ܸ�Ҫ��uecomģ����ٳ�ʼ�������������MCU��ͨѶģ��Ļ���ͨѶ���ܣ���ִ�б������е�
//        ��������֮ǰ������ñ���������uecom_init������
//�ڲ����ã�uart_init��uecom_sendCmd
//�޸����ڣ�2017.09.28��WB��
//====================================================================
uint_8 uecom_fast_init(void)
{
    //��1�����������͸���ֵ
    uint_16 i;
    uint_8 ret_val ;
    uint_8 flag;
    //��2��ʹ�ô�����ͨ��ģ��ͨ�ţ����ȳ�ʼ������
    uart_init(UART_UE, 115200);             //��ʼ��UEģ�鴮�ڣ�������Ϊ115200
    uart_enable_re_int(UART_UE);         //��UEģ�鴮���ж�
    uecom_switch(UECOM_REBOOT);  //��UEģ�����¹���
    //��3��������GPRSģ���ͨѶ
    if(uecom_sendCmd((uint_8 *)AT,AT_reBuf,1000)
    	&& uecom_sendCmd((uint_8 *)AT,AT_reBuf,1000))
            goto uecom_init_err1;     //UEģ��ͨѶʧ��
    //��4������UEģ��Ĳ�����Ϊ115200
    if(uecom_sendCmd((uint_8 *)IPR,AT_reBuf,1000))
        goto uecom_init_err2;     //UEģ��Ĳ���������ʧ��
    //��5���رջ��ԣ�����AT����֮��UEģ�鲻���ԭ��������أ�
    if(uecom_sendCmd((uint_8 *)ATE_0,AT_reBuf,1000))
        goto uecom_init_err3;     //�رջ���ʧ��
    //��6����ѯģ���IMEI,����IMEI�ŷ���dest�����ǰ15���ֽ�
    if(uecom_internal_getIMEI(IMEI))
        goto uecom_init_err4;     //IMEI��ѯʧ��
    IMEI[15] = 0;
    //��7����ѯģ���IMSI,����IMSI�ŷ���dest�����15-29�±�Ŀռ���
    if(uecom_internal_getIMSI(IMSI))
        goto uecom_init_err5;      //IMSI��ѯʧ��
    IMSI[15] = 0;
    ret_val = 0;
    goto uecom_init_exit;
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

//====================================================================
//�������ƣ�uecom_deInit
//�������أ���
//����˵������
//���ܸ�Ҫ��UE����ʼ�����رմ����жϣ�ȡ�����ڵ����Ÿ��ã����ر�UEģ��Ĺ��硣
//�ڲ����ã�uart_enable_re_int��uecom_switch
//�޸����ڣ�2017.09.22,QHJ,CC��2017.09.25��WB��
//====================================================================
void uecom_deInit(void)
{
    uart_disable_re_int(UART_UE);      //�ر�UEģ�鴮���ж�
    PORTE_PCR0 = PORT_PCR_MUX(0x0);    //�ر�UART1_TXD
    PORTE_PCR1 = PORT_PCR_MUX(0x0);    //�ر�UART1_RXD
    uecom_switch(UECOM_OFF);           //�ر�UEģ��Ĺ���
}

//====================================================================
//�������ƣ�uecom_send
//�������أ�  0�����ͳɹ���1������ʧ�ܣ�2�����س�ʱ������δ��ȷ��������ģʽ��
//����˵���� data:���������ݻ��������������
//                   length:���������ݵĳ���
//���ܸ�Ҫ��������ͨ���Ѿ�������TCP/UDPͨ�����ͳ�ȥ
//�ڲ����ã�uint_to_str��uart_send_string��Delay_ms��uart_sendN��
//       uart_init��uart_enable_re_int��uart_disable_re_int��
//       uart_re1��uart_send1��uart_enable_re_int
//�޸����ڣ�2017.08.11,QHJ,CC��2017.09.25��WB��
//====================================================================
uint_8 uecom_send(uint_16 length, uint_8 *data)
{
    //��1����������������ֵ
    uint_8 ch,flag,k,l;
    uint_8 ret_val;
    char dataLen[5]="";
    char QISEND[20] = "";   //���÷��͵��ֽ���
    char buf[50];
    uint_8 frame[1024];
    uint_16 frameLen;
    frameEncode(IMSI,data, length,frame,&frameLen);
    //��3������Ҫ���͵����ݳ��ȷ���UEģ�飬����������ģʽ
    uint_to_str(frameLen,dataLen);  //�����������ݵĳ���ת��Ϊ�ַ�����ʽ
    strcat(QISEND,"AT+QISEND=");    //��ȡ��������ģʽ��ATָ���ַ���
    strcat(QISEND, dataLen);
    strcat(QISEND, "\r\n");
    uart_send_string(UART_UE, QISEND);         //ָ�����͵������ֽڸ����������뷢��ģʽ
    Delay_ms(150);                                             //�ȴ�����ģʽ�ɹ�����
    uart_sendN(UART_UE,frameLen,frame);  //��������
    if(uecom_sendCmd("\r\n",AT_reBuf,5000))
    {
        ret_val = 1;
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
    if(IMEI[0]==0)
    	return 1;
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
    if(IMSI[0]==0)
    	return 1;
    for(i=0;i<15&&IMSI[i];i++)
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
    uint_8 i,ret_val;
    //��2����ȡ�ź�ǿ��
    if( uecom_sendCmd((uint_8 *)CSQ, AT_reBuf,3000))
    {
        ret_val = 1;
        goto uecom_getSignalPower_exit;
    }

    //��3����ȡ���ź�ǿ�ȣ�תΪint_16��ʽ��������*signalPower
    i =0;
    while(AT_reBuf[i++] != ':')
    {
        if(i >= 200)
        {
            ret_val = 1;
            goto uecom_getSignalPower_exit;
        }
    }
    i++;//�Թ��ո�
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
    if(uecom_sendCmd((uint_8 *)QILOCIP,AT_reBuf,2000))
        goto uecom_getUEIP_err1;

    //��4����ȡ��õ�IP��ַ
    i = 0;
    k = 0;   //��¼���յ���ip��ַ��λ������ֹ����OK��ȴû��ip��ַ�����

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
        goto uecom_getUEIP_err2;    //��Ȼ����OK�����ǣ���û�л�ȡ��IP��ַ
    //���ˣ�û��ʧ���˳����ɹ���
    ret_val = 0;
    goto uecom_getUEIP_exit;        //�ɹ���ret_val=0

uecom_getUEIP_err1:
    ret_val = 1;
    goto uecom_getUEIP_exit;//���ģ��ֵõ�IP��ַʧ��
uecom_getUEIP_err2:
    ret_val = 2;
    goto uecom_getUEIP_exit;//ATָ���OK�����ǲ�û�л�ȡ��IP��ַ
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
//====================================================================
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
//�޸����ڣ���2018-03-09������WB����
//=====================================================================
uint_8 uecom_getCurCommunity(uint_8 *data)
{
    //��1��������ʱ����
	uint_8 i, j,*p,ret;
	//��2�����ͻ�ȡС����Ϣ�����෢���Σ�
	 if(uecom_sendCmd(CCED_1,AT_reBuf,3000)
			 && uecom_sendCmd(CCED_1,AT_reBuf,3000))
	 {
	    	//���data���飬����δ���յ�����
	    	for(i=0;i<25;i++)
	    		data[i]=0;
	        ret = 1;              //����ֵ��Ϊ1
		    goto uecom_getCurCommunity_exit;   //�˳�����
	 }
    //��4���������ص����飬������data������
    p = strstr(AT_reBuf,":");   //����ַ�':'���ڵ�λ�ã����Ϊ��������ݡ�
    i = 0;
    j = 0;
    //ȡ��5������֮ǰ�����ݲ�����data������
    while(i<5)
    {
        p++;
        if(*p == ',')
            i++;
        data[j++] = *p;
    }
    //���һ������ɾ����������Ϊ�ַ���������
    data[j-1]=0;
    ret = 0;          //����ֵ��Ϊ0
    //��5���˳�����
    uecom_getCurCommunity_exit:
    return ret;
}

//------����Ϊ���������õ��ڲ�����--------------------------------------------
//====================================================================
//�������ƣ�uecom_sendCmd
//�������أ�0��ָ��ͳɹ�;1��ָ���ʧ��
//����˵����cmd����Ҫ���͵�ATָ����׵�ַ;buf[]:����ַ������ATָ��ؽ��,�����Ҫ200
//        ���ֽ�
//���ܸ�Ҫ������ATָ���ȡ������Ϣ����෢���������ÿ���������ȴ�3�롣
//�ڲ����ã�strTrim
//�޸����ڣ�2017.06.30��WYH��2017.08.11,QHJ,CC����2017.09.25��WB��
//====================================================================
uint_8 uecom_sendCmd(uint_8 *cmd,uint_8 buf[],uint_16 maxDelayMs)
{
    //��1�����������͸���ֵ
	uint_8 ret;
	uint_16 i,j;
	strcpy(curATCommand,cmd);   //������������ǰ����
	uart_send_string(UART_UE,cmd); //ͨ�����ڷ���ATָ��
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
	Delay_ms(100);      //��ֹ����͹���Ƶ��
	AT_reLength = 0;
	AT_reFlag = 0;
	curATCommand[0]=0;
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
    uint_8 i,k;

    //��2����ȡIMEI��
    dest[15] = 0;        //�ַ����Ľ�β��
    if(uecom_sendCmd((uint_8 *)CGSN,AT_reBuf,1000)
    		&& uecom_sendCmd((uint_8 *)CGSN,AT_reBuf,1000))
    {
		ret_val = 1;
		goto uecom_internal_getIMEI_exit;  //��ȡIMEI��ʧ��
    }
    //��3����IMEI�Ŵ洢��dest������
    i = 0;
    k = 0;
    while(AT_reBuf[i]!='\n'&&i++<200);
    if(i>=200)
    {
        ret_val = 2;
        goto uecom_internal_getIMEI_exit;     //����IMEI��ʧ��
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
    uint_8 i,k;

    dest[14] = 0;

    //��2����ȡIMSI��
	if(uecom_sendCmd((uint_8 *)CIMI,AT_reBuf,2000)
			&& uecom_sendCmd((uint_8 *)CIMI,AT_reBuf,2000))
	{
			ret_val = 1;
			goto uecom_internal_getIMSI_exit;  //��ȡIMSI��ʧ��
	}
    //��3����IMSI�Ŵ洢��dest������
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
    //�˳���
uecom_internal_getIMSI_exit:
    return ret_val;
}

//=====================================================================
//�������ƣ�uecom_link
//�������أ�0���ɹ����վ�������ӣ�
//      1������ģ��Ĺ�������ΪB��ʧ�ܣ�2������������ʽΪnetʧ�ܣ�
//      3������PDPʧ�ܣ�
//      4�����ģ��ֵõ�IP��ַʧ�ܣ�5��ATָ���OK�����ǲ�û�л�ȡ
//         ��IP��ַ
//����˵����*dest����Ż�վ����ģ���ip��ַ����ʽΪ�ַ������м���"."
//         ���ӣ�destָ�����������Ҫ��15���ֽ�
//���ܸ�Ҫ���������վ������
//�ڲ����ã�uecom_sendCmd��Delay_ms��uart_send_string��uecom_getUEIP
//�޸����ڣ�2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_link(uint_8 *dest)
{
    //��1����������������ֵ
    uint_8 i,ret_val,flag;

    //��2������ģ��Ĺ�������ΪB��
    if(uecom_sendCmd((uint_8 *)CGCLASS_B,AT_reBuf,1000)
    		&& uecom_sendCmd((uint_8 *)CGCLASS_B,AT_reBuf,1000))
        goto uecom_link_err1;      //����ģ��Ĺ�������ΪB��ʧ��
    //��3������������ʽΪnet
    if(uecom_sendCmd((uint_8 *)CGDCONT,AT_reBuf,1000)
    		&& uecom_sendCmd((uint_8 *)CGDCONT,AT_reBuf,1000))
        goto uecom_link_err2;      //����������ʽΪnetʧ��
    //��4������PDP(���2��)�������վ�������ip��ַ
    if(uecom_sendCmd((uint_8 *)CGACT_1,AT_reBuf,4000))
    {
    	uecom_sendCmd((uint_8 *)CGACT_0,AT_reBuf,1000);
    	Delay_ms(500);
        if(uecom_sendCmd((uint_8 *)CGACT_1,AT_reBuf,4000))
            goto uecom_link_err3;//����PDPʧ��
    }
    //��5�����ģ��ֵõ�IP��ַ
    flag = uecom_getUEIP(dest);
    if(flag) goto uecom_link_err4;//���ģ��ֵõ�IP��ַʧ��

    //���ˣ�û��ʧ���˳����ɹ���
    ret_val = 0;
    goto uecom_link_exit;
    //�����˳�
uecom_link_err1:
    ret_val = 1;            //����ģ��Ĺ�������ΪB��ʧ��
    goto uecom_link_exit;
uecom_link_err2:
    ret_val = 2;             //����������ʽΪnetʧ��
    goto uecom_link_exit;
uecom_link_err3:
    ret_val = 3;             //����PDPʧ��
    goto uecom_link_exit;
uecom_link_err4:
    ret_val = 3 + flag;      //���ģ��ֵõ�IP��ַʧ��
    goto uecom_link_exit;
    //�˳���
uecom_link_exit:
    return ret_val;
}

//=====================================================================
//�������ƣ�uecom_config
//�������أ�  0���ɹ�����TCP���ӣ�
//       1������TCP����ʧ��
//����˵���� IP:�����ӷ�������IP��ַ
//        port:�����ӷ������Ķ˿ں�
//���ܸ�Ҫ����ָ���ķ������˿ڽ���TCP����
//�ڲ����ã�uecom_sendCmd��uart_send_string��Delay_ms
//�޸����ڣ�2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_config(uint_8 *ip,uint_8* port)
{
    //��1�����������͸���ֵ
    uint_8 i;
    uint_8 flag;
    uint_8 ret_val;
    uint_8 QIOPEN[50] = "";    //����TCP���ӵĵ�ַ�Ͷ˿�
    uint_8 loc;
    //��2���齨ATָ��
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
    //ip��ַ���5λ
    for(i=0; i<5; i++)
    {
        if(port[i] == 0)
        {
            break;
        }
        QIOPEN[loc+i] = port[i];
    }
    strcat((char *)QIOPEN,"\"\r\n");
    //��3������TCP����
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
    //�˳���
uecom_config_exit:
    return ret_val;
}

//====================================================================
//�������ƣ�delay_ms
//�������أ���
//����˵������
//���ܸ�Ҫ����ʱ - ���뼶
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
////			wdog_feed();           //�����Ź�ι����
////	#endif
//  }
//}

//====================================================================
//�������ƣ�uint_to_str
//�������أ���
//����˵����ch:��ת����������        *str�����ת������ַ���
//���ܸ�Ҫ�����޷�������ת��Ϊ�ַ���
//====================================================================
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

//====================================================================
//�������ƣ�frameEncode
//���ܸ�Ҫ��ʵ�ִ��������ݵ���֡,�����������ݼ���֡ͷ��֡����֡β�Լ�У����Ϣ
//       frame=֡ͷ+IMSI+֡��+data+CRCУ����+֡β
//����˵����imsi���������ݵ��豸��IMSI��
//       data:����֡������ͷָ��
//       dlen:����֡�����ݳ���
//       frame:��֮֡��Ĵ���������֡
//       framelen:��֡������ݳ���
//�������أ�0���ɹ�
//====================================================================
void frameEncode(uint_8 *imsi,uint_8 *data,uint_16 dlen,
                 uint_8 *frame,uint_16 *framelen)
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

//===================================================================
//�������ƣ�crc16
//���ܸ�Ҫ�������ݽ���16λ��CRCУ�飬����У���Ľ��ֵ
//����˵����ptr:��ҪУ������ݻ�����
//       len:��Ҫ��������ݳ���
//�������أ�����õ���У��ֵ
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
//�������ƣ�uecom_recv
//�������أ�0�����յ���ͨ��TCP/UDP���������ݣ�1��������֡��2�������ݴ���
//����˵����*data:�����յ������ݴ�ŵ������鴮��;
//                *dataLen:�洢���յ������ݳ���
//���ܸ�Ҫ�������յ������ݽ��н�֡��������Ч���ݴ���data����֮�У��ɴ��ڵ�uart�жϵ��á�
//�ڲ����ã�uart_init��uart_disable_re_int,uart_re1��
//�޸����ڣ�2017.08.11,QHJ,CC��2017.09.25��WB��
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
      (index == 1 && ch != frameHead[1]))  //δ���յ����ݻ���δ����֡ͷ
    {
        index = 0;
        length = 0;
        ret_val = 2;     //�������ݴ���
        goto uecom_recv_exit;
    }
    data[index++] = ch;//��������
    if(index == 19)//��ȡ��Ч���ݳ���
    {
        length = ((uint_16)data[17]<<8) + data[18];
    }
    //���յ������ݴﵽһ֡���ȡ�23Ϊ֡ͷ֡β�ȳ���
    if(length != 0 && index >= length+23)
    {
        //CRCУ��
        crc = crc16(data+2,length+17);
        CRC[0] = (crc>>8)&0xff;
        CRC[1] = crc&0xff;

        if(data[index-2]!=frameTail[0] ||
        		data[index-1]!= frameTail[1]  //δ����֡β
            || CRC[0] != data[length+19] ||
			CRC[1] != data[length+20])        //CRC�������
        {
            index = 0;
            length = 0;
            ret_val = 2;
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
        goto uecom_recv_exit;
    }
    *dataLen = 0;//������֡
    ret_val = 1;
uecom_recv_exit:
    return ret_val;
}

//===================================================================
//�������ƣ�uecom_ATret
//���ܸ�Ҫ�����ж��д���AT���ص�ָ��
//����˵����ch�����ڽ��յ�������
//�������أ���
//===================================================================
//����AT����
void uecom_ATret(uint_8 ch)
{
    //��1�����������͸���ֵ
	if(curATCommand[0] == 0)       //����ǰ����AT�����ģʽ
	{
		return;
	}
	if(AT_reLength >= 510)             //�����յ������ݳ���510���������½���
	{
		AT_reLength = 0;
	}
	AT_reBuf[AT_reLength++] = ch;    //�洢���յ������ݵ�������AT_reBuf��
	AT_reBuf[AT_reLength] = 0;
	//��������AT_reBuf�н��յ�OK����ERROR
	if( (AT_reBuf[AT_reLength-2]=='O' && AT_reBuf[AT_reLength-1]=='K')
			|| (AT_reLength > 4 && AT_reBuf[AT_reLength-5]=='E'
			&&  AT_reBuf[AT_reLength-4]=='R'  &&  AT_reBuf[AT_reLength-3]=='R'
			&&  AT_reBuf[AT_reLength-2]=='O' &&  AT_reBuf[AT_reLength-1]=='R'))
	{
		curATCommand[0] = 0;         //����AT����
		AT_reFlag = AT_reLength;      //��־�Ѿ����յ�����
	}
	return ;
}
//===================================================================
//�������ƣ�uecom_interrupt
//���ܸ�Ҫ�����ж���Ӧ����Ĳ���
//����˵������
//�������أ���
//===================================================================
void uecom_interrupt(void)
{
	uint_8 ch,flag;
	 DISABLE_INTERRUPTS;
	if(uart_get_re_int(UART_UE))
	{
		uart_disable_re_int(UART_UE);    //�ش������ж�
		ch = uart_re1 (UART_UE, &flag);    //���ý���һ���ֽڵĺ�����������ж�λ
		if(flag)
		{
//			uart_send1(UART_2,ch);
			uecom_ATret(ch);                               //����AT����ص�����
			uecom_recv(ch,&gRecvLength,gRecvBuf); //����GPRS���յ�������
		}
		uart_enable_re_int(UART_UE);    //���������ж�
	}
    ENABLE_INTERRUPTS;
}
