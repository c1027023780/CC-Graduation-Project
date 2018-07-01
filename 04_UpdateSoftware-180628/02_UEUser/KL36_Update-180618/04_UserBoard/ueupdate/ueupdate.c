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

struct copyData cpdata;
struct insertData insdata;
struct checkData ckdata;
struct updateInfo uInfo;

uint_16 updateLen;       //需要更新的数据长度
uint_16 updateFlag;      //接收帧标识，0：无，1：复制帧，2：插入帧
uint_16 frameNum;        //总帧数
uint_8 check[50];       //每一位表示一帧接收状态，一共可表示50*8帧状态

//========================内部函数声明处=====================================
struct checkData ueupdate_check();
void updateEncode(uint_8 order,uint_8 *data,uint_16 dlen,uint_8 *frame,uint_16 *flen);
uint_8 updateDecode(uint_8 *frame,uint_8 *order,uint_8 *data,uint_16 *len);
uint_16 updatecrc16(uint_8 *ptr,uint_16 len);
void flagSet(uint_8 *flagBuf,uint_16 flag);
uint_8 flagGet(uint_8 *flagBuf,uint_16 flag);

//======================================================================
//函数名称：ueupdate_init
//函数参数：无
//函数返回：无
//功能概要：实现终端程序更新功能初始化，本构件使用串口作为更新方式。
//       具体操作为串口和flash模块的初始化和帧标志初始化
//======================================================================
uint_8 ueupdate_init()
{
	//（1）变量声明
	uint_8 i;

	//（2）外设初始化，包括串口和flash
	//uart_init(UART_2,9600);
	flash_init();

	//（3）串口中断使能
	//uart_enable_re_int(UART_2);

	//（4）接收帧标志初始化为0
	for(i = 0;i < 50;i++)
		check[i] = 0;
}

//======================================================================
//函数名称：ueupdate_send
//函数参数：order:命令字节
//函数返回：0：终端更新信息发送至上位机成功;1:传入的命令字节错误
//功能概要：根据传入的命令字节来组装对应的数据，并发送至上位机告知当前更新状态
//      命令字                       对应的状态或操作
//  上位机->终端：
//       0            更新操作提示
//       1            写flash操作
//       2            检查帧标志
//       3            更新操作
//  终端->上位机：
//       4            接收上位机命令成功
//       5            当前更新操作发生错误
//       6            检查缺失数据
//       7            其他
//======================================================================
uint_8 ueupdate_send(uint_8 order,uint_8 *senddata,uint_16 *sendLen)
{
	//（1）参数声明
	uint_16 i;
	uint_8 rv;
	uint_16 dlen;
	uint_8 data[500];//待回发数据
	uint_16 flen;

	rv = 0;//返回值默认0，成功
	//（2）根据命令字节来判断返还给上位机的数据
	switch(order)
	{
	case 4:  //接收数据成功，原始数据返回（命令4）
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
	case 5:  //当前更新过程发生错误，重新开始（命令5）
		cpdata.curFrame = 0;  //提示服务器程序，重新开始
		dlen = sizeof(struct copyData);
		memcpy(data,&cpdata,dlen);
		break;
	case 6:  //检查时缺失帧（命令6）
		//从结构体cdata中取出数据
		dlen = sizeof(struct checkData);
		memcpy(data,&ckdata,dlen);
		break;
	default:  //其他未知命令
		rv = 1;  //设置返回值，并退出
		goto ueupdate_send_EXIT;
	}
	//（3）根据命令字节和待传输的数据，进行组帧操作
	updateEncode(order,data,dlen,senddata,sendLen);
	//（4）使用串口方式将组帧后的数据发送至上位机
	//这里不使用uart_send_string函数是因为发送的数据中可能有0x0
//	for(i = 0;i < flen;i++)
//		uart_send1(UART_2,frame[i]);
	//（5）至此，ueupdate_send函数执行成功
	ueupdate_send_EXIT:
	return rv;
}

//======================================================================
//函数名称：ueupdate_recv
//函数参数：buf:接收到的上位机数据;len:接收到的上位机数据长度
//函数返回：0：上位机数据接收并操作成功;
//       1:数据解帧失败;2:接收到的数据不符合结构体;3:总帧数错误;
//       4:还有未接收的帧;5:命令未知错误
//功能概要：接收上位机发送的数据，并进行数据解析，按照上位机命令进行对应的操作。
//      命令字                       对应的状态或操作
//  上位机->终端：
//       0            更新操作提示
//       1            写flash操作
//       2            检查帧标志
//       3            更新操作
//  终端->上位机：
//       4            接收上位机命令成功
//       5            当前更新操作发生错误
//       6            检查缺失数据
//       7            其他
//======================================================================
uint_8 ueupdate_recv(uint_8 *buf,uint_16 len)
{
	//（1）变量声明
	uint_16 i;
	uint_8 rv;    //返回值
	uint_8 order;    //命令
	uint_16 sect;   //扇区号
	uint_16 offset; //偏移量

	uint_16 opeFrom;
	uint_16 opeTo;
	uint_16 opeLen;
	uint_16 opeSumLen;
	uint_8 opeData[1024];

	uint_16 dlen;
	uint_8 ulen;  //flash扇区数

	uint_16 userBase;
	uint_16 userAddress;
	uint_16 updateAddress;
	uint_8 sectorNum;

	rv = 0;
	//减少宏定义的使用
	userBase = USERBASE;
	userAddress = USERADDRESS;
	updateAddress = UPDATEADDRESS;
	sectorNum = SECTORNUM;

	//（2）进行解帧操作
	if(updateDecode(buf,&order,opeData,&dlen))
	{
		rv = 1;                   //解帧失败
		goto ueupdate_recv_EXIT;
	}
	//（3）根据命令进行操作
	switch(order)
	{
	case 0:  //（3.1）开始更新命令，擦除更新代码区数据
		//（3.1.1）擦除更新代码区flash
		for(i = updateAddress/1024;i < sectorNum;i++)
		{
			flash_erase(i);
			Delay_ms(50);
		}
		//（3.1.2）初始化帧标识
		for(i = 0;i < 50;i++) check[i] = 0;
		//（3.1.3）接收到开始更新帧
		if(dlen == sizeof(struct copyData))
		{
			//将接收到的开始更新帧数据解析成对应的copyData结构体数据
			memcpy(&cpdata,opeData,dlen);//copyData结构体变量赋值

			updateFlag = 1;//接收到copyData数据
			updateLen = 0; //默认更新数据长度为0，当接收到帧数据递增

			uInfo.updateNum = 0;
			uInfo.updateFlag = 0;
			//获得总帧数
			frameNum = cpdata.frameNum;
			//修改接收帧标识
			flagSet(check,cpdata.curFrame);
		}
		break;
	case 1:  //（3.2）写flash命令，向待更新区域写入数据
		//（3.2.1）进行是否符合结构体判断
		//接收到copyData数据
		if(dlen == sizeof(struct copyData))
		{
			memcpy(&cpdata,opeData,dlen);//copyData结构体变量赋值
			//判断总帧数是否一致
			if(cpdata.frameNum != frameNum)
			{
				rv = 3;                   //总帧数错误
				goto ueupdate_recv_EXIT;
			}
			//复制帧的operate必为0
			if(cpdata.operate == 0)
			{
				//遍历待进行的复制操作
				for(i = 0;i < cpdata.copyNum;i++)
				{
					//取出复制数据，每3个数据一组from,to,len
					opeFrom = cpdata.data[i*3];
					opeTo = cpdata.data[i*3+1];
					opeLen = cpdata.data[i*3+2];
					//更新数据长度
					updateLen += opeLen;
					while(opeLen > 0)
					{
						if(opeLen > 1024)//若一次复制操作中数据长度大于500，需分拆并进行多次操作
						{
							//从原地址取出数据
							flash_read_physical(opeData,opeFrom,1024);
							Delay_ms(10);
							//进行复制操作
							sect = (updateAddress-userAddress+opeTo)/1024;
							offset = (updateAddress-userAddress+opeTo)%1024;
							flash_write(sect,offset,1024,opeData);
							//为下次操作修改变量
							opeFrom += 1024;
							opeTo += 1024;
							opeLen -= 1024;
						}
						else
						{
							//从原地址取出数据
							flash_read_physical(opeData,opeFrom,opeLen);
							Delay_ms(10);
							//进行复制操作
							sect = (updateAddress-userAddress+opeTo)/1024;
							offset = (updateAddress-userAddress+opeTo)%1024;
							flash_write(sect,offset,opeLen,opeData);
							opeLen = 0;
						}
					}//end while
				}//end for
				//当前接收帧为复制帧
				updateFlag = 1;
				//修改帧标识
				flagSet(check,cpdata.curFrame);
			}
		}
		//接收到insertData数据
		else if(dlen == sizeof(struct insertData))
		{
			memcpy(&insdata,opeData,dlen);//insertData结构体变量赋值
			//判断总帧数是否一致
			if(insdata.frameNum != frameNum)
			{
				rv = 3;                   //总帧数错误
				goto ueupdate_recv_EXIT;
			}
			//insertData帧operate必为1（插入）或2（替换）
			if(insdata.operate == 1)  //插入操作
			{
				opeSumLen = 0;  //记录插入数据长度
				for(i = 0;i < insdata.insertNum;i++)
				{
					opeTo = insdata.insertInfo[i*2];
					opeLen = insdata.insertInfo[i*2+1];
					//进行复制操作
					sect = (updateAddress-userAddress+opeTo)/1024;
					offset = (updateAddress-userAddress+opeTo)%1024;
					flash_write(sect,offset,opeLen,insdata.data+opeSumLen);
					opeSumLen += opeLen;
				}
				updateLen += opeSumLen;  //更新数据长度
				//当前接收帧为插入帧
				updateFlag = 2;
				//（3.2.5）修改帧标识
				flagSet(check,insdata.curFrame);
			}
			else if(insdata.operate == 2)  //替换操纵（只有跳转main函数）
			{
				opeTo = insdata.insertInfo[0];
				opeLen = insdata.insertInfo[1];
				//进行替换操作
				sect = opeTo/1024;
				offset = opeTo%1024;

				//使用第63扇区存放main函数跳转操作
				flash_erase(63);
				Delay_ms(10);
				flash_read_logic(opeData,sect,0,1024);
				memcpy(opeData+offset,insdata.data,opeLen);
				//将跳转main函数步骤数据写入63扇区
				flash_write(63,0,1024,opeData);

				//设置bootloader更新63扇区
				uInfo.updateData[uInfo.updateNum*3] = 63;     //from
				uInfo.updateData[uInfo.updateNum*3+1] = sect;     //to
				uInfo.updateData[uInfo.updateNum*3+2] = 1;  //1个扇区
				uInfo.updateNum++;
				//当前接收帧为插入帧
				updateFlag = 2;
				//（3.2.5）修改帧标识
				flagSet(check,insdata.curFrame);
			}
		}
		else
		{
			rv = 2;                   //接收到的数据不符合结构体
			goto ueupdate_recv_EXIT;
		}
		break;
	case 2:  //（3.3）更新前的检查操作
		if(dlen == sizeof(struct insertData))
		{
			memcpy(&insdata,opeData,dlen);//insertData结构体变量赋值
			flagSet(check,insdata.curFrame);
			ckdata = ueupdate_check();
			//若当前还有帧数据未接收到，向上位机提示
			if(ckdata.loseNum > 0)
			{
				rv = 4;                   //还有未接收的帧
				goto ueupdate_recv_EXIT;
			}
			//当前接收帧为插入帧
			updateFlag = 2;
		}
		break;
	case 3:  //（3.4）更新程序,
		//向第7扇区写入更新标志
		flash_erase(7);
		Delay_ms(10);
		uInfo.updateFlag = 3;
		uInfo.updateData[uInfo.updateNum*3] = updateAddress/1024;     //from
		uInfo.updateData[uInfo.updateNum*3+1] = userAddress/1024;     //to
		if(updateLen%1024 == 0)  //计算扇区个数
			ulen = updateLen/1024;
		else
			ulen = updateLen/1024+1;
		uInfo.updateData[uInfo.updateNum*3+2] = ulen;
		uInfo.updateNum++;

		flash_write(7,0,sizeof(struct updateInfo),(uint_8*)&uInfo);
		//复位
		NVIC_SystemReset();
		break;
	default:
		rv = 5;  //命令错误
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
	for(i = 0;i < frameNum - 1;i++)
	{
		if(flagGet(check,i) == 0)  //记录未接收到的帧数据信息
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

	flagBuf[flag] = 1;

//	i = flag/8;  //标志所处字节数
//	j = flag%8;  //字节内位置
//
//	flagBuf[i] &= (1 << j);  //设置标志位
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

//	i = flag/8;  //标志所处字节数
//	j = flag%8;  //字节内位置

	//flagBuf[i] & (1 << j);
	return flagBuf[flag];
}
