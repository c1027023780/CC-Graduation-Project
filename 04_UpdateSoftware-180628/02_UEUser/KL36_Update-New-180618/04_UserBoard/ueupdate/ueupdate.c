//=====================================================================
//�ļ����ƣ�ueupdate.c
//���ܸ�Ҫ���ն˳�����¹���Դ�ļ�
//=====================================================================

#include "ueupdate.h"
#include "string.h"

#define FrameHeadH    (0x50)         //֡ͷ     ASCII���ӦP
#define FrameHeadL    (0x50)         //֡ͷ     ASCII���ӦP
#define FrameTaiH     (0x43)         //֡β     ASCII���ӦC
#define FrameTaiL     (0x43)         //֡β     ASCII���ӦC

struct copyData cpdata;
struct insertData insdata;
struct checkData ckdata;
struct updateInfo uInfo;

uint_16 updateLen;       //��Ҫ���µ����ݳ���
uint_16 updateFlag;      //����֡��ʶ��0���ޣ�1������֡��2������֡
uint_16 frameNum;        //��֡��
uint_8 check[50];       //ÿһλ��ʾһ֡����״̬��һ���ɱ�ʾ50*8֡״̬

//========================�ڲ�����������=====================================
struct checkData ueupdate_check();
void updateEncode(uint_8 order,uint_8 *data,uint_16 dlen,uint_8 *frame,uint_16 *flen);
uint_8 updateDecode(uint_8 *frame,uint_8 *order,uint_8 *data,uint_16 *len);
uint_16 updatecrc16(uint_8 *ptr,uint_16 len);
void flagSet(uint_8 *flagBuf,uint_16 flag);
uint_8 flagGet(uint_8 *flagBuf,uint_16 flag);

//======================================================================
//�������ƣ�ueupdate_init
//������������
//�������أ���
//���ܸ�Ҫ��ʵ���ն˳�����¹��ܳ�ʼ����������ʹ�ô�����Ϊ���·�ʽ��
//       �������Ϊ���ں�flashģ��ĳ�ʼ����֡��־��ʼ��
//======================================================================
uint_8 ueupdate_init()
{
	//��1����������
	uint_8 i;

	//��2�������ʼ�����������ں�flash
	//uart_init(UART_2,9600);
	flash_init();

	//��3�������ж�ʹ��
	//uart_enable_re_int(UART_2);

	//��4������֡��־��ʼ��Ϊ0
	for(i = 0;i < 50;i++)
		check[i] = 0;
}

//======================================================================
//�������ƣ�ueupdate_send
//����������order:�����ֽ�
//�������أ�0���ն˸�����Ϣ��������λ���ɹ�;1:����������ֽڴ���
//���ܸ�Ҫ�����ݴ���������ֽ�����װ��Ӧ�����ݣ�����������λ����֪��ǰ����״̬
//      ������                       ��Ӧ��״̬�����
//  ��λ��->�նˣ�
//       0            ���²�����ʾ
//       1            дflash����
//       2            ���֡��־
//       3            ���²���
//  �ն�->��λ����
//       4            ������λ������ɹ�
//       5            ��ǰ���²�����������
//       6            ���ȱʧ����
//       7            ����
//======================================================================
uint_8 ueupdate_send(uint_8 order,uint_8 *senddata,uint_16 *sendLen)
{
	//��1����������
	uint_16 i;
	uint_8 rv;
	uint_16 dlen;
	uint_8 data[500];//���ط�����
	uint_16 flen;

	rv = 0;//����ֵĬ��0���ɹ�
	//��2�����������ֽ����жϷ�������λ��������
	switch(order)
	{
	case 4:  //�������ݳɹ���ԭʼ���ݷ��أ�����4��
		if(updateFlag == 1)
		{
			dlen = sizeof(struct copyData);
			memcpy(data,&cpdata,dlen);
		}
		else if(updateFlag == 2)
		{
			dlen = sizeof(struct insertData);
			memcpy(data,&insdata,dlen);
		}
		break;
	case 5:  //��ǰ���¹��̷����������¿�ʼ������5��
		cpdata.curFrame = 0;  //��ʾ�������������¿�ʼ
		dlen = sizeof(struct copyData);
		memcpy(data,&cpdata,dlen);
		break;
	case 6:  //���ʱȱʧ֡������6��
		//�ӽṹ��cdata��ȡ������
		dlen = sizeof(struct checkData);
		memcpy(data,&ckdata,dlen);
		break;
	default:  //����δ֪����
		rv = 1;  //���÷���ֵ�����˳�
		goto ueupdate_send_EXIT;
	}
	//��3�����������ֽںʹ���������ݣ�������֡����
	updateEncode(order,data,dlen,senddata,sendLen);
	//��4��ʹ�ô��ڷ�ʽ����֡������ݷ�������λ��
	//���ﲻʹ��uart_send_string��������Ϊ���͵������п�����0x0
//	for(i = 0;i < flen;i++)
//		uart_send1(UART_2,frame[i]);
	//��5�����ˣ�ueupdate_send����ִ�гɹ�
	ueupdate_send_EXIT:
	return rv;
}

//======================================================================
//�������ƣ�ueupdate_recv
//����������buf:���յ�����λ������;len:���յ�����λ�����ݳ���
//�������أ�0����λ�����ݽ��ղ������ɹ�;
//       1:���ݽ�֡ʧ��;2:���յ������ݲ����Ͻṹ��;3:��֡������;
//       4:����δ���յ�֡;5:����δ֪����
//���ܸ�Ҫ��������λ�����͵����ݣ����������ݽ�����������λ��������ж�Ӧ�Ĳ�����
//      ������                       ��Ӧ��״̬�����
//  ��λ��->�նˣ�
//       0            ���²�����ʾ
//       1            дflash����
//       2            ���֡��־
//       3            ���²���
//  �ն�->��λ����
//       4            ������λ������ɹ�
//       5            ��ǰ���²�����������
//       6            ���ȱʧ����
//       7            ����
//======================================================================
uint_8 ueupdate_recv(uint_8 *buf,uint_16 len)
{
	//��1����������
	uint_16 i;
	uint_8 rv;    //����ֵ
	uint_8 order;    //����
	uint_16 sect;   //������
	uint_16 offset; //ƫ����

	uint_16 opeFrom;
	uint_16 opeTo;
	uint_16 opeLen;
	uint_16 opeSumLen;
	uint_8 opeData[1024];

	uint_16 dlen;
	uint_8 ulen;  //flash������

	uint_16 userBase;
	uint_16 userAddress;
	uint_16 updateAddress;
	uint_8 sectorNum;

	rv = 0;
	//���ٺ궨���ʹ��
	userBase = USERBASE;
	userAddress = USERADDRESS;
	updateAddress = UPDATEADDRESS;
	sectorNum = SECTORNUM;

	//��2�����н�֡����
	if(updateDecode(buf,&order,opeData,&dlen))
	{
		rv = 1;                   //��֡ʧ��
		goto ueupdate_recv_EXIT;
	}
	//��3������������в���
	switch(order)
	{
	case 0:  //��3.1����ʼ��������������´���������
		//��3.1.1���������´�����flash
		for(i = updateAddress/1024;i < sectorNum;i++)
		{
			flash_erase(i);
			Delay_ms(50);
		}
		//��3.1.2����ʼ��֡��ʶ
		for(i = 0;i < 50;i++) check[i] = 0;
		//��3.1.3�����յ���ʼ����֡
		if(dlen == sizeof(struct copyData))
		{
			//�����յ��Ŀ�ʼ����֡���ݽ����ɶ�Ӧ��copyData�ṹ������
			memcpy(&cpdata,opeData,dlen);//copyData�ṹ�������ֵ

			updateFlag = 1;//���յ�copyData����
			updateLen = 0; //Ĭ�ϸ������ݳ���Ϊ0�������յ�֡���ݵ���

			uInfo.updateNum = 0;
			uInfo.updateFlag = 0;
			//�����֡��
			frameNum = cpdata.frameNum;
			//�޸Ľ���֡��ʶ
			flagSet(check,cpdata.curFrame);
		}
		break;
	case 1:  //��3.2��дflash��������������д������
		//��3.2.1�������Ƿ���Ͻṹ���ж�
		//���յ�copyData����
		if(dlen == sizeof(struct copyData))
		{
			memcpy(&cpdata,opeData,dlen);//copyData�ṹ�������ֵ
			//�ж���֡���Ƿ�һ��
			if(cpdata.frameNum != frameNum)
			{
				rv = 3;                   //��֡������
				goto ueupdate_recv_EXIT;
			}
			//����֡��operate��Ϊ0
			if(cpdata.operate == 0)
			{
				//���������еĸ��Ʋ���
				for(i = 0;i < cpdata.copyNum;i++)
				{
					//ȡ���������ݣ�ÿ3������һ��from,to,len
					opeFrom = cpdata.data[i*3];
					opeTo = cpdata.data[i*3+1];
					opeLen = cpdata.data[i*3+2];
					//�������ݳ���
					updateLen += opeLen;
					while(opeLen > 0)
					{
						if(opeLen > 1024)//��һ�θ��Ʋ��������ݳ��ȴ���500����ֲ𲢽��ж�β���
						{
							//��ԭ��ַȡ������
							flash_read_physical(opeData,opeFrom,1024);
							Delay_ms(10);
							//���и��Ʋ���
							sect = (updateAddress-userAddress+opeTo)/1024;
							offset = (updateAddress-userAddress+opeTo)%1024;
							flash_write(sect,offset,1024,opeData);
							//Ϊ�´β����޸ı���
							opeFrom += 1024;
							opeTo += 1024;
							opeLen -= 1024;
						}
						else
						{
							//��ԭ��ַȡ������
							flash_read_physical(opeData,opeFrom,opeLen);
							Delay_ms(10);
							//���и��Ʋ���
							sect = (updateAddress-userAddress+opeTo)/1024;
							offset = (updateAddress-userAddress+opeTo)%1024;
							flash_write(sect,offset,opeLen,opeData);
							opeLen = 0;
						}
					}//end while
				}//end for
				//��ǰ����֡Ϊ����֡
				updateFlag = 1;
				//�޸�֡��ʶ
				flagSet(check,cpdata.curFrame);
			}
		}
		//���յ�insertData����
		else if(dlen == sizeof(struct insertData))
		{
			memcpy(&insdata,opeData,dlen);//insertData�ṹ�������ֵ
			//�ж���֡���Ƿ�һ��
			if(insdata.frameNum != frameNum)
			{
				rv = 3;                   //��֡������
				goto ueupdate_recv_EXIT;
			}
			//insertData֡operate��Ϊ1�����룩��2���滻��
			if(insdata.operate == 1)  //�������
			{
				opeSumLen = 0;  //��¼�������ݳ���
				for(i = 0;i < insdata.insertNum;i++)
				{
					opeTo = insdata.insertInfo[i*2];
					opeLen = insdata.insertInfo[i*2+1];
					//���и��Ʋ���
					sect = (updateAddress-userAddress+opeTo)/1024;
					offset = (updateAddress-userAddress+opeTo)%1024;
					flash_write(sect,offset,opeLen,insdata.data+opeSumLen);
					opeSumLen += opeLen;
				}
				updateLen += opeSumLen;  //�������ݳ���
				//��ǰ����֡Ϊ����֡
				updateFlag = 2;
				//��3.2.5���޸�֡��ʶ
				flagSet(check,insdata.curFrame);
			}
			else if(insdata.operate == 2)  //�滻���ݣ�ֻ����תmain������
			{
				opeTo = insdata.insertInfo[0];
				opeLen = insdata.insertInfo[1];
				//�����滻����
				sect = opeTo/1024;
				offset = opeTo%1024;

				//ʹ�õ�63�������main������ת����
				flash_erase(63);
				Delay_ms(10);
				flash_read_logic(opeData,sect,0,1024);
				memcpy(opeData+offset,insdata.data,opeLen);
				//����תmain������������д��63����
				flash_write(63,0,1024,opeData);

				//����bootloader����63����
				uInfo.updateData[uInfo.updateNum*3] = 63;     //from
				uInfo.updateData[uInfo.updateNum*3+1] = sect;     //to
				uInfo.updateData[uInfo.updateNum*3+2] = 1;  //1������
				uInfo.updateNum++;
				//��ǰ����֡Ϊ����֡
				updateFlag = 2;
				//��3.2.5���޸�֡��ʶ
				flagSet(check,insdata.curFrame);
			}
		}
		else
		{
			rv = 2;                   //���յ������ݲ����Ͻṹ��
			goto ueupdate_recv_EXIT;
		}
		break;
	case 2:  //��3.3������ǰ�ļ�����
		if(dlen == sizeof(struct insertData))
		{
			memcpy(&insdata,opeData,dlen);//insertData�ṹ�������ֵ
			flagSet(check,insdata.curFrame);
			ckdata = ueupdate_check();
			//����ǰ����֡����δ���յ�������λ����ʾ
			if(ckdata.loseNum > 0)
			{
				rv = 4;                   //����δ���յ�֡
				goto ueupdate_recv_EXIT;
			}
			//��ǰ����֡Ϊ����֡
			updateFlag = 2;
		}
		break;
	case 3:  //��3.4�����³���,
		//���7����д����±�־
		flash_erase(7);
		Delay_ms(10);
		uInfo.updateFlag = 3;
		uInfo.updateData[uInfo.updateNum*3] = updateAddress/1024;     //from
		uInfo.updateData[uInfo.updateNum*3+1] = userAddress/1024;     //to
		if(updateLen%1024 == 0)  //������������
			ulen = updateLen/1024;
		else
			ulen = updateLen/1024+1;
		uInfo.updateData[uInfo.updateNum*3+2] = ulen;
		uInfo.updateNum++;

		flash_write(7,0,sizeof(struct updateInfo),(uint_8*)&uInfo);
		//��λ
		NVIC_SystemReset();
		break;
	default:
		rv = 5;  //�������
		goto ueupdate_recv_EXIT;
	}
	//��4�����ˣ��������ݳɹ�
	ueupdate_recv_EXIT:
	return rv;
}

//================================����Ϊ�ڲ�����============================
//======================================================================
//�������ƣ�ueupdate_check
//������������
//�������أ�checkData�ṹ��
//���ܸ�Ҫ���������ǰ�ļ�������������֡����δ���£���������Ϣд��checkData�ṹ�幩�ⲿ�����ж�
//======================================================================
struct checkData ueupdate_check()
{
	//��1����������
	uint_16 i;
	struct checkData cdata;

	//��2����ʼ��δ����֡��Ϊ0
	cdata.loseNum = 0;
	for(i = 0;i < frameNum - 1;i++)
	{
		if(flagGet(check,i) == 0)  //��¼δ���յ���֡������Ϣ
		{
			cdata.loseFrame[cdata.loseNum] = i;
			cdata.loseNum++;
		}
	}
	//��3�����ؽṹ�����ݣ����ⲿʹ��
	return cdata;
}

//======================================================================
//�������ƣ�updateEncode
//����������oeder:�����ֽ�;data:����֡����;dlen:����֡���ݳ���;
//       frame:��֡�������;flen:��֡������ݳ���
//�������أ���
//���ܸ�Ҫ������ʮ����������֡��ʽ������֡����
//��ע��       ʮ����������֡��ʽ
//             ֡ͷ +֡�� + ���� + ���� + У�� + ֡β
//             2B  2B   1B   nB   2B   2B
//======================================================================
void updateEncode(uint_8 order,uint_8 *data,uint_16 dlen,uint_8 *frame,uint_16 *flen)
{
	//��1����������
	uint_16 crc;  //CRCУ����
	uint_16 len;  //֡��

	len = 0;
	//��2��֡ͷ����
	frame[len++] = FrameHeadH;
	frame[len++] = FrameHeadL;
	//��3��֡������
	frame[len++] = (dlen + 9)>>8;
	frame[len++] = dlen + 9;
	//��4���������
	frame[len++] = order;
	//��5�����ݲ���
	memcpy(frame+len,data,dlen);
	len += dlen;
	//��6��CRCУ�������
	crc = updatecrc16(data,dlen);
	frame[len++] = crc>>8;
	frame[len++] = crc;
	//��7��֡β����
	frame[len++] = FrameTaiH;
	frame[len++] = FrameTaiL;
	//��8��֡����ֵ
	*flen = len;
}

//======================================================================
//�������ƣ�updateDecode
//����������frame:����֡����;order:��֡��������ֽ�;data:��֡�����Ч����;len��֡�����Ч���ݳ���
//�������أ�0:��֡�ɹ�;
//       1:֡ͷ����;2:CRCУ�����;3:֡β����
//���ܸ�Ҫ������ʮ����������֡��ʽ�������ݽ�֡����
//��ע��       ʮ����������֡��ʽ
//             ֡ͷ +֡�� + ���� + ���� + У�� + ֡β
//             2B  2B   1B   nB   2B   2B
//======================================================================
uint_8 updateDecode(uint_8 *frame,uint_8 *order,uint_8 *data,uint_16 *len)
{
	//��1����������
	uint_8 rv;       //����ֵ
	uint_8 CRC[2];
	uint_16 crc;     //16λCRCУ����
	uint_16 frameLen;//֡��

	rv = 0;  //Ĭ�ϳɹ�������ֵΪ0
	//��2��У��֡ͷ
	if(frame[0] != FrameHeadH || frame[1] != FrameHeadL)//֡ͷ����
	{
		rv = 1;
		goto updateDecode_EXIT;
	}
	//��3����ȡ֡��
	frameLen = (frame[2]<<8) | frame[3];
	//��4����ȡ֡����
	*order = frame[4];
	//��5��CRCУ��
	*len = frameLen-9;  //��ȡ��Ч���ݳ���
	crc = updatecrc16(frame+5,*len);
	CRC[0] = crc>>8;
	CRC[1] = crc;
	if(CRC[0] != frame[frameLen-4] || CRC[1] != frame[frameLen-3])//CRCУ�����
	{
		rv = 2;
		goto updateDecode_EXIT;
	}
	//��6��У��֡β
	if(frame[frameLen-2] != FrameTaiH || frame[frameLen-1] != FrameTaiL)//֡β����
	{
		rv = 3;
		goto updateDecode_EXIT;
	}
	//��7�����ˣ�����У����ɣ����и�ֵ����
	memcpy(data,frame+5,*len);

	updateDecode_EXIT:
	return rv;
}

//======================================================================
//�������ƣ�updatecrc16
//����������ptr:��У�������;len:��У������ݳ���
//�������أ�16λCRCУ����
//���ܸ�Ҫ������һ�����ݵ�CRCУ����
//======================================================================
uint_16 updatecrc16(uint_8 *ptr,uint_16 len)
{
	uint_16 i,j,tmp,crc16;

	crc16 = 0xffff;
	for(i = 0;i < len;i++)
	{
		crc16 = ptr[i]^crc16;
		for(j = 0;j < 8;j++)
		{
			tmp = crc16 & 0x0001;
			crc16 = crc16>>1;
			if(tmp)
				crc16 = crc16^0xa001;
		}
	}

	return crc16;
}

//======================================================================
//�������ƣ�flagSet
//����������flagBuf:��־����;flag:��־λ��
//�������أ���
//���ܸ�Ҫ������λ�ķ�ʽ���ñ�־����flagBuf[0]�е�8λ�ֱ����0~7��״̬��
//       flagBuf[1]�е�8λ�ֱ����8~15��״̬��...
//======================================================================
void flagSet(uint_8 *flagBuf,uint_16 flag)
{
	uint_16 i;
	uint_8 j;

	flagBuf[flag] = 1;

//	i = flag/8;  //��־�����ֽ���
//	j = flag%8;  //�ֽ���λ��
//
//	flagBuf[i] &= (1 << j);  //���ñ�־λ
}

//======================================================================
//�������ƣ�flagGet
//����������flagBuf:��־����;flag:��־λ��
//�������أ���flagλ�ı�־״̬
//���ܸ�Ҫ����ȡflagBuf��ַ��ʼ�ĵ�flagλ״̬
//======================================================================
uint_8 flagGet(uint_8 *flagBuf,uint_16 flag)
{
	uint_16 i;
	uint_8 j;

//	i = flag/8;  //��־�����ֽ���
//	j = flag%8;  //�ֽ���λ��

	//flagBuf[i] & (1 << j);
	return flagBuf[flag];
}
