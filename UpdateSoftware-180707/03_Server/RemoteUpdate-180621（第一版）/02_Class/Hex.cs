﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RemoteUpdate
{
    public struct hexStruct
    {     //hex文本行有效数据格式
        public byte len;          //数据长度
        public ushort address;    //数据地址
        public byte type;         //数据类型
        public byte[] data;       //数据
        public byte check;        //校验和
    };
    class Hex
    {
        public List<hexStruct> list;

        private int userBase;//用户代码首地址，若不满足则不能更新程序

        public Hex()
        {
            list = new List<hexStruct>();
            userBase = 0;
        }

        public int UserBase
        {
            get { return userBase; }
            set { userBase = value; }
        }

        //======================================================================
        //函数名称：addLine
        //函数返回：0：添加hex文件行数据成功；1：起始标识错误；
        //          2：hex行数据长度错误；3：数据类型错误；4：校验和错误
        //参数说明：line：hex文件行数据
        //功能概要：根据传入的hex文件行数据，获取各个有效数据存入链表
        //======================================================================
        public int addLine(string line)
        {
            //（1）变量定义
            int i;
            int rv;              //返回值
            byte dataLen;        //数据长度
            ushort dataAddress;  //起始地址
            byte dataType;       //数据类型
            byte[] data;         //有效数据
            byte dataCheck;      //数据校验和   
            byte hexSum;         //数据累加和
            hexStruct hs;        //hex文本行数据

            rv = 0;  //默认成功，返回值为0
            //（2）起始标识判断
            if (line[0] != ':')
            {
                rv = 1;     //若起始标识不为“：”，返回错误
                goto hexLineCheck_EXIT;
            }

            //（3）数据长度判断
            //Hex文件一行总长=1(:)+2(数据长度)+4(起始地址)+2(数据类型)+xx(数据)+2(检验和)
            //               =11+xx
            dataLen = byte.Parse(line.Substring(1, 2), System.Globalization.NumberStyles.HexNumber);//获取数据长度
            if (line.Length != 2 * dataLen + 11 || dataLen == 0)  //hex结束行为:00000001FF，不采集该行数据
            {
                rv = 2;     //Hex行数据长度不满足，返回错误
                goto hexLineCheck_EXIT;
            }
            hs.len = dataLen;  //赋值

            //（4）获取本行数据起始地址
            dataAddress = ushort.Parse(line.Substring(3, 4), System.Globalization.NumberStyles.HexNumber);//获取起始地址
            hs.address = dataAddress;  //赋值

            //（5）数据类型判断
            dataType = byte.Parse(line.Substring(7, 2), System.Globalization.NumberStyles.HexNumber);//获取数据类型
            if (dataType < 0 || dataType > 1)  //有效数据类型为0-5，但在此处只获取类型为0或1的数据
            {
                rv = 3;     //数据类型只为0或1，不满足时返回错误
                goto hexLineCheck_EXIT;
            }
            hs.type = dataType;  //赋值

            //（6）有效数据获取
            data = new byte[dataLen];
            for (i = 0; i < dataLen; i++)  //循环获取有效数据，每次得到一个byte数据
                data[i] = byte.Parse(line.Substring(2 * i + 9, 2), System.Globalization.NumberStyles.HexNumber);
            hs.data = data;  //赋值

            //（7）检验和判断
            dataCheck = byte.Parse(line.Substring(line.Length - 2, 2), System.Globalization.NumberStyles.HexNumber);//获取数据校验和
            hexSum = 0;
            for (i = 1; i < line.Length - 2; i += 2)  //计算校验和之前的所有数据的累加和
            {
                hexSum += byte.Parse(line.Substring(i, 2), System.Globalization.NumberStyles.HexNumber);
            }
            if (dataCheck != (byte)(0x100 - hexSum))  //判断检验和是否等于0x100-累加和
            {
                rv = 4;  //不满足校验和，返回错误
                goto hexLineCheck_EXIT;
            }
            hs.check = dataCheck;  //赋值
            //至此，一行hex文件数据校验完毕，准备添加如链表
            //（8）将hex文件行数据添加进链表
            list.Add(hs);

        hexLineCheck_EXIT:
            return rv;
        }

        //======================================================================
        //函数名称：clear
        //函数返回：无
        //参数说明：无
        //功能概要：清空hex数据
        //======================================================================
        public void clear()
        {
            list.Clear();
        }

        public List<hexStruct> getHexList()
        {
            return this.list;
        }

        //======================================================================
        //函数名称：hexDataCal
        //函数返回：hex文件中所有有效byte数据
        //参数说明：无
        //功能概要：计算
        //======================================================================
        public byte[] hexDataCal()
        {
            //（1）变量定义
            int i;
            byte[] data;
            int dataLen;     
            hexStruct lastHS;

            //（2）若程序地址小于0x2000，提示报错
            if (list[0].address < userBase)
            {
                data = null;
                goto hexDataCal_EXIT;
            }

            //（3）确定hex文件有效数据的总大小
            lastHS = list[list.Count - 1];  //找到最后一个数据
            dataLen = lastHS.address + lastHS.len - userBase;//最后一个数据的起始地址+数据长度=总长度
            data = new byte[dataLen];   //为data申请空间
            //（4）flash中默认为0xff
            for (i = 0; i < dataLen; i++)
                data[i] = 0xff;

            //（5）遍历链表，提取数据
            i = 0;
            foreach (hexStruct hs in list)
            {
                //遍历链表中的有效数据，复制进全局变量
                for (i = 0; i < hs.len; i++)
                {
                    //更新程序的起始地址为0x2000
                    data[hs.address + i - userBase] = hs.data[i];
                }
            }

            hexDataCal_EXIT:
            return data;
        }
    }
}
