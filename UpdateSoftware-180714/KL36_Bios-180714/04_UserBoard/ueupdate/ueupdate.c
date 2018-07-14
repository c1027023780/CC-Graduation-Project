//=====================================================================
//文件名称：ueupdate.c
//功能概要：终端程序更新构件源文件
//=====================================================================

#include "ueupdate.h"
#include "string.h"

#define FrameHeadH    (0x50)         //帧头     ASCII码对应P
#define FrameHeadL    (0x50)         //帧头     ASCII码对应P
#define FrameTaiH     (0x43)         //帧尾     ASCII码对应C
#define FrameTaiL     (0x43)         //帧尾     ASCII码对应C

struct checkData ckdata;

uint_8  uSectNo;          //用户代码起始扇区
uint_16 uFrameNum;        //总帧数
uint_16 uCurFrame;        //当前接收帧号
uint_8  uBackOrder;       //返回上位机命令标志
uint_8  uMSP[4];          //更新程序主栈指针，在更新执行之前才写入Flash
uint_8  uCheck[20];       //每一位表示一帧接收状态，一共可表示20*8帧状态

//========================内部函数声明处=====================================
struct checkData ueupdate_check();
void updateEncode(uint_8 order,uint_8 *data,uint_16 dlen,uint_8 *frame,uint_16 *flen);
uint_8 updateDecode(uint_8 *frame,uint_8 *order,uint_8 *data,uint_16 *len);
uint_16 updatecrc16(uint_8 *ptr,uint_16 len);
void flagSet(uint_8 *flagBuf,uint_16 flag);
uint_8 flagGet(uint_8 *flagBuf,uint_16 flag);

//======================================================================
//函数名称：ueupdate_init
//函数参数：userSector：用户程序起始扇区号
//函数返回：无
//功能概要：实现终端程序更新功能初始化，本构件使用串口作为更新方式。
//       具体操作为串口和flash模块的初始化和帧标志初始化
//======================================================================
uint_8 ueupdate_init(uint_8 userSector)
{
	//（1）变量声明
	uint_8 i;

	//（2）初始化文件级全局变量
	uSectNo = userSector;  //设置用户程序起始扇区
	uFrameNum = 0;         //初始化总帧数为0
	uCurFrame = 0;         //初始化当前接收帧号为0
	uBackOrder = 0;        //初始化上位机命令标志为0
	for(i = 0;i < sizeof(uCheck);i++)
		uCheck[i] = 0;     //接收帧标志初始化为0
}

//======================================================================
//函数名称：ueupdate_back
//函数参数：buf:返回上位机数据;len:返回上位机数据长度
//函数返回：0：有数据返回 ; 1:无数据返回
//功能概要：根据接收到的上位机数据返回上位机终端执行状况
//      命令字                       对应的状态或操作
//  上位机->终端：
//       0            更新操作提示
//       1            写flash操作
//       2            检查帧标志
//       3            更新操作
//  终端->上位机：
//       1            终端数据接收成功，返回当前帧号
//       2            检查操作，提示上位机丢失帧
//======================================================================
uint_8 ueupdate_back(uint_8 *buf,uint_16 *len)
{
	//（1）变量声明
	uint_8 rv;
	uint_16 dlen;
	uint_8 data[100];//待回发数据

	rv = 0;
	//（2）根据返回命令来组装不同的返回数据
	switch(uBackOrder)
	{
//	case 1:  //返回上位机接收到的帧号  串口更新不使用
//		data[0] = uCurFrame;
//		data[1] = uCurFrame>>8;
//		dlen = 2;
//		break;
	case 2:  //返回检查帧数据
		dlen = sizeof(struct checkData);
		memcpy(data,&ckdata,dlen);
		break;
	default:
		rv = 1;
		goto ueupdate_back_EXIT;
	}
	//（3）根据命令字节和待传输的数据，进行组帧操作
	updateEncode(uBackOrder,data,dlen,buf,len);//返回数据使用传参的方式返回

	ueupdate_back_EXIT:
	return rv;
}

//======================================================================
//函数名称：ueupdate_recv
//函数参数：buf:接收到的上位机数据;len:接收到的上位机数据长度
//函数返回：0：上位机数据接收并操作成功;
//       1:解帧失败;2:总帧数错误;3:提示代码更新完毕;4:未知命令
//功能概要：接收上位机发送的数据，并进行数据解析，按照上位机命令进行对应的操作。
//      命令字                       对应的状态或操作
//  上位机->终端：
//       0            更新操作提示
//       1            写flash操作
//       2            检查帧标志
//       3            更新操作
//  终端->上位机：
//       1            终端数据接收成功，返回当前帧号
//       2            检查操作，提示上位机丢失帧
//======================================================================
uint_8 ueupdate_recv(uint_8 *buf,uint_16 len)
{
	//（1）变量声明
	uint_8 i;
	uint_8 rv;    //返回值
	uint_8 order;    //命令
	uint_16 sect;   //扇区号
	uint_16 offset; //偏移量
	uint_16 opeTo;
	uint_16 opeLen;
	uint_16 opeSumLen;
	uint_16 dlen;
	uint_8 opeData[500];
	struct insertData insdata;

	rv = 0;
	uBackOrder = 0;  //默认不返回数据
	//（2）进行数据解帧操作
	if(updateDecode(buf,&order,opeData,&dlen))
	{
		rv = 1;                   //解帧失败
		goto ueupdate_recv_EXIT;
	}
	//（3）根据命令进行操作
	switch(order)
	{
	case 0:  //（3.1）开始更新命令，擦除更新代码区数据
		//擦除用户程序flash
		for(i = uSectNo;i < 64;i++) flash_erase(i);
		//初始化帧标识
		for(i = 0;i < sizeof(uCheck);i++) uCheck[i] = 0;
		//接收到开始更新帧
		if(dlen == sizeof(struct insertData))
		{
			//将接收到的开始更新帧数据解析成对应的insertData结构体数据
			memcpy(&insdata,opeData,dlen);//insertData结构体变量赋值
			//从结构体数据中获取当前更新总帧数
			uFrameNum = insdata.frameNum;
			//修改接收帧标识
			uCurFrame = insdata.curFrame;
			flagSet(uCheck,insdata.curFrame);
			//提示上位机开始更新命令帧接收成功
			uBackOrder = 1;
		}
		break;
	case 1:  //（3.2）写flash命令，向待更新区域写入数据
		//进行是否符合结构体判断
		if(dlen == sizeof(struct insertData))  //接收到insertData数据
		{
			memcpy(&insdata,opeData,dlen);//insertData结构体变量赋值
			//判断总帧数是否一致
			if(insdata.frameNum != uFrameNum)
			{
				rv = 2;                   //总帧数错误
				goto ueupdate_recv_EXIT;
			}
			//insertData帧operate必为1（插入）
			if(insdata.operate == 1)  //插入操作
			{
				opeSumLen = 0;  //记录插入数据长度
				for(i = 0;i < insdata.insertNum;i++)
				{
					if(insdata.curFrame == 1 && i == 0)  //得到第一帧数据帧，取出msp，直到最后更新才写入Flash
					{
						memcpy(uMSP,insdata.data,4);
						opeTo = insdata.insertInfo[i*2] + 4;
						opeLen = insdata.insertInfo[i*2+1] - 4;
						opeSumLen += 4;
					}
					else  //其他情况正常操作
					{
						opeTo = insdata.insertInfo[i*2];
						opeLen = insdata.insertInfo[i*2+1];
					}
					//进行复制操作
					sect = opeTo/1024;    //扇区号
					offset = opeTo%1024;  //偏移量
					flash_write(sect,offset,opeLen,insdata.data+opeSumLen);
					opeSumLen += opeLen;
				}
				//修改帧标识
				uCurFrame = insdata.curFrame;
				flagSet(uCheck,insdata.curFrame);
				//提示上位数据/命令帧接收成功
				uBackOrder = 1;
			}
		}
		break;
	case 2:  //（3.3）更新前的检查操作
		if(dlen == sizeof(struct insertData))
		{
			memcpy(&insdata,opeData,dlen);//insertData结构体变量赋值
			flagSet(uCheck,insdata.curFrame);
			ckdata = ueupdate_check();

			uBackOrder = 2;  //返回检查帧数据，提示上位机丢失帧
		}
		break;
	case 3:  //（3.4）更新程序,
		if(ckdata.loseNum == 0)
		{
			flash_write(uSectNo,0,4,uMSP);
			rv = 3;  //提示代码更新完毕
		}
		break;
	default:
		rv = 4;  //命令错误
		goto ueupdate_recv_EXIT;
	}
	//（4）至此，接收数据成功
	ueupdate_recv_EXIT:
	return rv;
}

//================================以下为内部函数============================
//======================================================================
//函数名称：ueupdate_check
//函数参数：无
//函数返回：checkData结构体
//功能概要：软件更新前的检查操作，若还有帧数据未更新，将具体信息写入checkData结构体供外部函数判断
//======================================================================
struct checkData ueupdate_check()
{
	//（1）变量声明
	uint_16 i;
	struct checkData cdata;

	//（2）初始化未更新帧数为0
	cdata.loseNum = 0;
	for(i = 0;i < uFrameNum - 1;i++)
	{
		if(flagGet(uCheck,i) == 0)  //记录未接收到的帧数据信息
		{
			cdata.loseFrame[cdata.loseNum] = i;
			cdata.loseNum++;
		}
	}
	//（3）返回结构体数据，供外部使用
	return cdata;
}

//======================================================================
//函数名称：updateEncode
//函数参数：oeder:命令字节;data:待组帧数据;dlen:待组帧数据长度;
//       frame:组帧后的数据;flen:组帧后的数据长度
//函数返回：无
//功能概要：按照十六进制数据帧格式进行组帧操作
//备注：       十六进制数据帧格式
//             帧头 +帧长 + 命令 + 数据 + 校验 + 帧尾
//             2B  2B   1B   nB   2B   2B
//======================================================================
void updateEncode(uint_8 order,uint_8 *data,uint_16 dlen,uint_8 *frame,uint_16 *flen)
{
	//（1）变量声明
	uint_16 crc;  //CRC校验码
	uint_16 len;  //帧长

	len = 0;
	//（2）帧头操作
	frame[len++] = FrameHeadH;
	frame[len++] = FrameHeadL;
	//（3）帧长操作
	frame[len++] = (dlen + 9)>>8;
	frame[len++] = dlen + 9;
	//（4）命令操作
	frame[len++] = order;
	//（5）数据操作
	memcpy(frame+len,data,dlen);
	len += dlen;
	//（6）CRC校验码操作
	crc = updatecrc16(data,dlen);
	frame[len++] = crc>>8;
	frame[len++] = crc;
	//（7）帧尾操作
	frame[len++] = FrameTaiH;
	frame[len++] = FrameTaiL;
	//（8）帧长赋值
	*flen = len;
}

//======================================================================
//函数名称：updateDecode
//函数参数：frame:待解帧数据;order:解帧后的命令字节;data:解帧后的有效数据;len解帧后的有效数据长度
//函数返回：0:解帧成功;
//       1:帧头不对;2:CRC校验错误;3:帧尾不对
//功能概要：按照十六进制数据帧格式进行数据解帧操作
//备注：       十六进制数据帧格式
//             帧头 +帧长 + 命令 + 数据 + 校验 + 帧尾
//             2B  2B   1B   nB   2B   2B
//======================================================================
uint_8 updateDecode(uint_8 *frame,uint_8 *order,uint_8 *data,uint_16 *len)
{
	//（1）变量声明
	uint_8 rv;       //返回值
	uint_8 CRC[2];
	uint_16 crc;     //16位CRC校验码
	uint_16 frameLen;//帧长

	rv = 0;  //默认成功，返回值为0
	//（2）校验帧头
	if(frame[0] != FrameHeadH || frame[1] != FrameHeadL)//帧头不对
	{
		rv = 1;
		goto updateDecode_EXIT;
	}
	//（3）获取帧长
	frameLen = (frame[2]<<8) | frame[3];
	//（4）获取帧命令
	*order = frame[4];
	//（5）CRC校验
	*len = frameLen-9;  //获取有效数据长度
	crc = updatecrc16(frame+5,*len);
	CRC[0] = crc>>8;
	CRC[1] = crc;
	if(CRC[0] != frame[frameLen-4] || CRC[1] != frame[frameLen-3])//CRC校验错误
	{
		rv = 2;
		goto updateDecode_EXIT;
	}
	//（6）校验帧尾
	if(frame[frameLen-2] != FrameTaiH || frame[frameLen-1] != FrameTaiL)//帧尾不对
	{
		rv = 3;
		goto updateDecode_EXIT;
	}
	//（7）至此，所有校验完成，进行赋值操作
	memcpy(data,frame+5,*len);
	data[*len] = 0;

	updateDecode_EXIT:
	return rv;
}

//======================================================================
//函数名称：updatecrc16
//函数参数：ptr:待校验的数据;len:待校验的数据长度
//函数返回：16位CRC校验码
//功能概要：计算一段数据的CRC校验码
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
//函数名称：flagSet
//函数参数：flagBuf:标志数据;flag:标志位数
//函数返回：无
//功能概要：按照位的方式设置标志，如flagBuf[0]中的8位分别代表0~7的状态，
//       flagBuf[1]中的8位分别代表8~15的状态，...
//======================================================================
void flagSet(uint_8 *flagBuf,uint_16 flag)
{
	uint_16 i;
	uint_8 j;

	i = flag/8;  //标志所处字节数
	j = flag%8;  //字节内位置

	flagBuf[i] |= (1 << j);  //设置标志位
}

//======================================================================
//函数名称：flagGet
//函数参数：flagBuf:标志数据;flag:标志位数
//函数返回：第flag位的标志状态
//功能概要：获取flagBuf地址开始的第flag位状态
//======================================================================
uint_8 flagGet(uint_8 *flagBuf,uint_16 flag)
{
	uint_16 i;
	uint_8 j;

	i = flag/8;  //标志所处字节数
	j = flag%8;  //字节内位置

	return flagBuf[i] & (1 << j);
}
