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

struct checkData ckdata;

uint_8  uSectNo;          //�û�������ʼ����
uint_16 uFrameNum;        //��֡��
uint_16 uCurFrame;        //��ǰ����֡��
uint_8  uBackOrder;       //������λ�������־
uint_8  uMSP[4];          //���³�����ջָ�룬�ڸ���ִ��֮ǰ��д��Flash
uint_8  uCheck[20];       //ÿһλ��ʾһ֡����״̬��һ���ɱ�ʾ20*8֡״̬

//========================�ڲ�����������=====================================
struct checkData ueupdate_check();
void updateEncode(uint_8 order,uint_8 *data,uint_16 dlen,uint_8 *frame,uint_16 *flen);
uint_8 updateDecode(uint_8 *frame,uint_8 *order,uint_8 *data,uint_16 *len);
uint_16 updatecrc16(uint_8 *ptr,uint_16 len);
void flagSet(uint_8 *flagBuf,uint_16 flag);
uint_8 flagGet(uint_8 *flagBuf,uint_16 flag);

//======================================================================
//�������ƣ�ueupdate_init
//����������userSector���û�������ʼ������
//�������أ���
//���ܸ�Ҫ��ʵ���ն˳�����¹��ܳ�ʼ����������ʹ�ô�����Ϊ���·�ʽ��
//       �������Ϊ���ں�flashģ��ĳ�ʼ����֡��־��ʼ��
//======================================================================
uint_8 ueupdate_init(uint_8 userSector)
{
	//��1����������
	uint_8 i;

	//��2����ʼ���ļ���ȫ�ֱ���
	uSectNo = userSector;  //�����û�������ʼ����
	uFrameNum = 0;         //��ʼ����֡��Ϊ0
	uCurFrame = 0;         //��ʼ����ǰ����֡��Ϊ0
	uBackOrder = 0;        //��ʼ����λ�������־Ϊ0
	for(i = 0;i < sizeof(uCheck);i++)
		uCheck[i] = 0;     //����֡��־��ʼ��Ϊ0
}

//======================================================================
//�������ƣ�ueupdate_back
//����������buf:������λ������;len:������λ�����ݳ���
//�������أ�0�������ݷ��� ; 1:�����ݷ���
//���ܸ�Ҫ�����ݽ��յ�����λ�����ݷ�����λ���ն�ִ��״��
//      ������                       ��Ӧ��״̬�����
//  ��λ��->�նˣ�
//       0            ���²�����ʾ
//       1            дflash����
//       2            ���֡��־
//       3            ���²���
//  �ն�->��λ����
//       1            �ն����ݽ��ճɹ������ص�ǰ֡��
//       2            ����������ʾ��λ����ʧ֡
//======================================================================
uint_8 ueupdate_back(uint_8 *buf,uint_16 *len)
{
	//��1����������
	uint_8 rv;
	uint_16 dlen;
	uint_8 data[100];//���ط�����

	rv = 0;
	//��2�����ݷ�����������װ��ͬ�ķ�������
	switch(uBackOrder)
	{
//	case 1:  //������λ�����յ���֡��  ���ڸ��²�ʹ��
//		data[0] = uCurFrame;
//		data[1] = uCurFrame>>8;
//		dlen = 2;
//		break;
	case 2:  //���ؼ��֡����
		dlen = sizeof(struct checkData);
		memcpy(data,&ckdata,dlen);
		break;
	default:
		rv = 1;
		goto ueupdate_back_EXIT;
	}
	//��3�����������ֽںʹ���������ݣ�������֡����
	updateEncode(uBackOrder,data,dlen,buf,len);//��������ʹ�ô��εķ�ʽ����

	ueupdate_back_EXIT:
	return rv;
}

//======================================================================
//�������ƣ�ueupdate_recv
//����������buf:���յ�����λ������;len:���յ�����λ�����ݳ���
//�������أ�0����λ�����ݽ��ղ������ɹ�;
//       1:��֡ʧ��;2:��֡������;3:��ʾ����������;4:δ֪����
//���ܸ�Ҫ��������λ�����͵����ݣ����������ݽ�����������λ��������ж�Ӧ�Ĳ�����
//      ������                       ��Ӧ��״̬�����
//  ��λ��->�նˣ�
//       0            ���²�����ʾ
//       1            дflash����
//       2            ���֡��־
//       3            ���²���
//  �ն�->��λ����
//       1            �ն����ݽ��ճɹ������ص�ǰ֡��
//       2            ����������ʾ��λ����ʧ֡
//======================================================================
uint_8 ueupdate_recv(uint_8 *buf,uint_16 len)
{
	//��1����������
	uint_8 i;
	uint_8 rv;    //����ֵ
	uint_8 order;    //����
	uint_16 sect;   //������
	uint_16 offset; //ƫ����
	uint_16 opeTo;
	uint_16 opeLen;
	uint_16 opeSumLen;
	uint_16 dlen;
	uint_8 opeData[500];
	struct insertData insdata;

	rv = 0;
	uBackOrder = 0;  //Ĭ�ϲ���������
	//��2���������ݽ�֡����
	if(updateDecode(buf,&order,opeData,&dlen))
	{
		rv = 1;                   //��֡ʧ��
		goto ueupdate_recv_EXIT;
	}
	//��3������������в���
	switch(order)
	{
	case 0:  //��3.1����ʼ��������������´���������
		//�����û�����flash
		for(i = uSectNo;i < 64;i++) flash_erase(i);
		//��ʼ��֡��ʶ
		for(i = 0;i < sizeof(uCheck);i++) uCheck[i] = 0;
		//���յ���ʼ����֡
		if(dlen == sizeof(struct insertData))
		{
			//�����յ��Ŀ�ʼ����֡���ݽ����ɶ�Ӧ��insertData�ṹ������
			memcpy(&insdata,opeData,dlen);//insertData�ṹ�������ֵ
			//�ӽṹ�������л�ȡ��ǰ������֡��
			uFrameNum = insdata.frameNum;
			//�޸Ľ���֡��ʶ
			uCurFrame = insdata.curFrame;
			flagSet(uCheck,insdata.curFrame);
			//��ʾ��λ����ʼ��������֡���ճɹ�
			uBackOrder = 1;
		}
		break;
	case 1:  //��3.2��дflash��������������д������
		//�����Ƿ���Ͻṹ���ж�
		if(dlen == sizeof(struct insertData))  //���յ�insertData����
		{
			memcpy(&insdata,opeData,dlen);//insertData�ṹ�������ֵ
			//�ж���֡���Ƿ�һ��
			if(insdata.frameNum != uFrameNum)
			{
				rv = 2;                   //��֡������
				goto ueupdate_recv_EXIT;
			}
			//insertData֡operate��Ϊ1�����룩
			if(insdata.operate == 1)  //�������
			{
				opeSumLen = 0;  //��¼�������ݳ���
				for(i = 0;i < insdata.insertNum;i++)
				{
					if(insdata.curFrame == 1 && i == 0)  //�õ���һ֡����֡��ȡ��msp��ֱ�������²�д��Flash
					{
						memcpy(uMSP,insdata.data,4);
						opeTo = insdata.insertInfo[i*2] + 4;
						opeLen = insdata.insertInfo[i*2+1] - 4;
						opeSumLen += 4;
					}
					else  //���������������
					{
						opeTo = insdata.insertInfo[i*2];
						opeLen = insdata.insertInfo[i*2+1];
					}
					//���и��Ʋ���
					sect = opeTo/1024;    //������
					offset = opeTo%1024;  //ƫ����
					flash_write(sect,offset,opeLen,insdata.data+opeSumLen);
					opeSumLen += opeLen;
				}
				//�޸�֡��ʶ
				uCurFrame = insdata.curFrame;
				flagSet(uCheck,insdata.curFrame);
				//��ʾ��λ����/����֡���ճɹ�
				uBackOrder = 1;
			}
		}
		break;
	case 2:  //��3.3������ǰ�ļ�����
		if(dlen == sizeof(struct insertData))
		{
			memcpy(&insdata,opeData,dlen);//insertData�ṹ�������ֵ
			flagSet(uCheck,insdata.curFrame);
			ckdata = ueupdate_check();

			uBackOrder = 2;  //���ؼ��֡���ݣ���ʾ��λ����ʧ֡
		}
		break;
	case 3:  //��3.4�����³���,
		if(ckdata.loseNum == 0)
		{
			flash_write(uSectNo,0,4,uMSP);
			rv = 3;  //��ʾ����������
		}
		break;
	default:
		rv = 4;  //�������
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
	for(i = 0;i < uFrameNum - 1;i++)
	{
		if(flagGet(uCheck,i) == 0)  //��¼δ���յ���֡������Ϣ
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
	data[*len] = 0;

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

	i = flag/8;  //��־�����ֽ���
	j = flag%8;  //�ֽ���λ��

	flagBuf[i] |= (1 << j);  //���ñ�־λ
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

	i = flag/8;  //��־�����ֽ���
	j = flag%8;  //�ֽ���λ��

	return flagBuf[i] & (1 << j);
}
