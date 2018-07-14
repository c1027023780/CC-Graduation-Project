using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace RemoteUpdate
{
    class Update
    {
        //插入帧,总字节486
        [StructLayoutAttribute(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        private struct insertData
        {
            public ushort frameNum;      //总帧数
            public ushort curFrame;      //当前帧数
            public byte operate;         //操作，0：复制，1：插入，2：替换（只用于main地址跳转）
            public byte insertNum;       //插入代码个数
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public ushort[] insertInfo;  //插入代码的起始地址和长度,每2个数据一组，最多20组
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 400)]
            public byte[] data;          //插入数据
        }

        //检查帧
        [StructLayoutAttribute(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        private struct checkData
        {
            public ushort frameNum;    //总帧数
            public byte loseNum;       //未接收的帧个数
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 40)]
            public ushort[] loseFrame; //未接收的帧号,最多40个
        }

        private List<hexStruct> hexList;      //传入的Hex文件链表数据
        private List<insertData> insertList;  //待发送至终端的flash插入结构体链表

        private byte[] check;             //检查帧状态，每1位表示一帧数据发送状态，1：已发送；0：未发送

        //======================================================================
        //函数名称：Update构造函数
        //函数返回：无
        //参数说明：hexList：待更新的Hex文件链表数据
        //功能概要：Update构造函数，根据传入的Hex数据构造返回终端的更新数据帧和命令帧
        //======================================================================
        public Update(List<hexStruct> hexList)
        {
            //（1）变量声明
            int i;
            ushort sum, cur;
            insertData insdata;

            //（2）获得传入的Hex文件链表数据
            this.hexList = hexList;
            //（3）初始化发送至终端的链表
            this.insertList = new List<insertData>();
            //（4）构造开始更新命令帧，数据任意,在发送至终端时命令需设为0
            insdata = new insertData();
            insdata.insertInfo = new ushort[40];
            insdata.data = new byte[400];
            this.insertList.Add(insdata);//将更新命令帧插入链表
            //（5）构造插入帧链表数据，发送时命令为1
            getBackList();
            //（6）构造更新检查帧和更新命令帧，数据任意，其对应的命令分别为2，3
            this.insertList.Add(insdata);  //添加更新检查帧
            this.insertList.Add(insdata);  //添加更新命令帧
            //（7）修改总帧数和帧号
            sum = (ushort)this.insertList.Count;
            cur = 0;
            for (i = 0; i < this.insertList.Count; i++)
            {
                insdata = this.insertList[i];
                insdata.frameNum = sum;
                insdata.curFrame = cur++;
                this.insertList.RemoveAt(i);
                this.insertList.Insert(i, insdata);
            }
            //（8）初始化帧发送标识
            check = new byte[sum];
            for (i = 0; i < check.Length; i++) check[i] = 0;
        }

        //======================================================================
        //函数名称：getNextFrame
        //函数返回：byte[]:下一个未发送至终端的更新数据帧
        //参数说明：无
        //功能概要：根据check数组来获取下一帧数据，进行组帧操作并返回
        //======================================================================
        public byte[] getNextFrame()
        {
            //（1）变量声明
            int i, j;
            byte order;
            byte[] data;
            byte[] frame;

            data = null;
            frame = null;
            order = 1;
            //（2）找到当前待更新的帧
            for (i = 0; i < check.Length; i++)
            {
                if (check[i] == 0)
                {
                    check[i] = 1;
                    break;
                }
            }
            if (i >= check.Length) return null;
            //（3）判断发送命令
            if (i == 0)                      //开始帧
                order = 0;
            else if (i == check.Length - 2)  //检查帧
                order = 2;
            else if (i == check.Length - 1)  //更新帧
                order = 3;
            else                             //数据帧
                order = 1;
            //（4）从链表取出数据,插入帧中查找帧数据
            for (j = 0; j < this.insertList.Count; j++)
            {
                if (this.insertList[j].curFrame == i)
                {
                    //结构体数据转为byte数组
                    data = structToBytes(this.insertList[j], Marshal.SizeOf(this.insertList[j]));
                    goto getNextFrame_ENCODE;
                }
            }
        //（5）组帧操作
        getNextFrame_ENCODE:
            if (order == 3) check[i] = 1;  //对于更新命令帧，不需要获取返回值
            frame = frameEncode(order, data);//组帧操作

            return frame;
        }

        //======================================================================
        //函数名称：updateRecv
        //函数返回：0:数据命令帧发送成功;
        //          1:解帧操作失败;2:当前更新发生错误，更新操作需重新开始;
        //          3:检查操作失败;4:其他未知指令
        //参数说明：frame:终端返回的帧数据
        //功能概要：数据命令帧发送至终端后，终端返回数据至frame。
        //          解析frame数据来判断终端操作状态
        //======================================================================
        public int updateRecv(byte[] frame)
        {
            //（1）变量声明
            int i;
            int rv;    //返回值
            byte order;
            byte[] data;
            ushort curframe;
            checkData ckdata;

            rv = 0;   //返回值默认成功，为0
            order = 0;
            data = null;
            curframe = 0;
            ckdata = new checkData();

            //（2）解帧操作
            if (!frameDecode(frame, ref data, ref order))
            {
                rv = 1;
                goto updateRecv_EXIT;  //解帧失败
            }
            //（3）命令判断
            switch (order)
            {
                case 1:  //（3.1）终端数据接收成功
                    //获取返回的帧号
                    curframe = BitConverter.ToUInt16(data, 0);
                    //修改发送标识
                    check[curframe] = 1;//数据发送成功
                    break;
                case 2:
                    if (data.Length == Marshal.SizeOf(ckdata))
                    {
                        //将byte数组解析为对应的检查帧结构体数据
                        ckdata = (checkData)bytesToStruct(data, typeof(checkData));
                        //根据终端返回的未接收帧号来修改check数组
                        for (i = 0; i < ckdata.loseNum; i++)
                        {
                            check[ckdata.loseFrame[i]] = 0;
                        }

                        if (ckdata.loseNum > 0)  //帧丢失
                        {
                            check[getFrameNum() - 2] = 0;  //检查帧需重发
                            rv = 2;
                        }
                    }
                    break;
                default:
                    rv = 3;
                    break;
            }
        updateRecv_EXIT:
            return rv;
        }

        //======================================================================
        //函数名称：getFrameNum
        //函数返回：返回当前更新帧个数
        //参数说明：无
        //功能概要：获取当前更新帧个数
        //======================================================================
        public int getFrameNum()
        {
            int count;

            count = this.insertList.Count;

            return count;
        }

        //======================================================================
        //函数名称：getCurCount
        //函数返回：返回当前准备更新的帧号
        //参数说明：无
        //功能概要：获取当前准备更新的帧号
        //======================================================================
        public int getCurCount()
        {
            int i;

            for (i = 0; i < check.Length; i++)
            {
                if (check[i] == 0) return i;
            }

            return -1;
        }

        //======================================================================
        //函数名称：getSendCount
        //函数返回：返回当前已更新的帧个数
        //参数说明：无
        //功能概要：获取当前已更新的帧个数
        //======================================================================
        public int getSendCount()
        {
            int i;
            int count;

            count = 0;
            for (i = 0; i < check.Length; i++)
            {
                if (check[i] == 1) count++;
            }

            return count;
        }

        //======================================================================
        //函数名称：checkClear
        //函数返回：index:清检查帧某位，可实现该帧重新发送
        //参数说明：无
        //功能概要：清空某检查帧指定位，实现该帧重新发送
        //======================================================================
        public void checkClear(int index)
        {
            if (index >= this.insertList.Count)
                index = this.insertList.Count - 1;
            else if (index < 0)
                index = 0;

            check[index] = 0;
        }

        //======================================================================
        //函数名称：isEmpty
        //函数返回：true:更新帧更新完毕;false:更新帧未更新完
        //参数说明：无
        //功能概要：判断当前更新帧是否更新完毕
        //======================================================================
        public bool isEmpty()
        {
            //（1）变量声明
            int i;
            //（2）若check数组有0值，表示当前有更新帧未发送至终端
            for (i = 0; i < check.Length; i++)
            {
                if (check[i] == 0)
                    return false;
            }

            return true;
        }

        //public bool isUpdateFrame(byte[] frame)
        //{
        //    bool rv;
        //    byte order;
        //    byte[] data;
        //    insertData insdata;
        //    checkData ckdata;

        //    rv = true;
        //    order = 0;
        //    data = null;
        //    insdata = new insertData();
        //    ckdata = new checkData();

        //    //（2）解帧操作
        //    if (!frameDecode(frame, ref data, ref order))
        //    {
        //        rv = false;
        //        goto isUpdateFrame_EXIT;  //解帧失败
        //    }
        //    //（3）判断帧长
        //    if (data.Length != Marshal.SizeOf(insdata) && data.Length != Marshal.SizeOf(ckdata))
        //    {
        //        rv = false;
        //        goto isUpdateFrame_EXIT;  //解帧失败
        //    }

        //isUpdateFrame_EXIT:
        //    return rv;
        //}



        //===========================内部函数==================================
        //======================================================================
        //函数名称：getBackList
        //函数返回：无
        //参数说明：无
        //功能概要：根据传入的Hex文件数据按照插入帧的格式构造数据帧（非命令帧）
        //======================================================================
        private void getBackList()
        {
            //（1）变量声明
            int i, j;
            ushort addr;
            ushort len;
            ushort lastAddr;
            ushort lastLen;
            ushort sumLen;
            byte[] data;
  
            insertData idata;  

            //（2）数据初始化
            idata = new insertData();
            idata.insertInfo = new ushort[40];
            idata.data = new byte[400];

            //（3）遍历Hex数据链表来构造返回插入链表
            for (i = 0; i < this.hexList.Count; i++)
            {
                //（3.1）获取本行数据基本信息
                //获取本行代码待插入的地址
                addr = this.hexList[i].address;
                //获取本行代码长度
                len = this.hexList[i].len;
                //获取本行代码数据
                data = this.hexList[i].data;

                //（3.2）进行插入帧赋值操作
                if (idata.insertNum == 0)  //判断当前行数据是否是第一个数据
                {
                    idata.operate = 1;
                    idata.insertNum = 1;
                    idata.insertInfo[0] = addr;
                    idata.insertInfo[1] = len;
                    Array.Copy(data, 0, idata.data, 0, len);
                }
                else  //其他情况
                {
                    //获取当前已插入的数据总长度
                    sumLen = 0;
                    for (j = 0; j < idata.insertNum; j++)
                        sumLen += idata.insertInfo[j * 2 + 1];
                    //判断已插入的数据是否超出上限，包括插入个数和总的插入数据长度
                    if ((idata.insertNum >= 20) || (sumLen + len > 400))  //超出界限，新建一帧数据
                    {
                        //将当前插入帧加入链表
                        this.insertList.Add(idata);
                        //新建一个插入帧并赋值
                        idata = new insertData();
                        idata.insertInfo = new ushort[40];
                        idata.data = new byte[400];
                        idata.operate = 1;
                        idata.insertNum = 1;
                        idata.insertInfo[0] = addr;
                        idata.insertInfo[1] = len;
                        Array.Copy(data, 0, idata.data, 0, len);
                    }
                    else  //未超出上限，再本插入帧中继续添加数据
                    {
                        //判断本次插入数据是否与上次的相连
                        //先取出先前一行数据信息，包括地址和长度
                        lastAddr = idata.insertInfo[(idata.insertNum - 1) * 2];
                        lastLen = idata.insertInfo[(idata.insertNum - 1) * 2 + 1];
                        if (lastAddr + lastLen == addr)  //相连，添加数据并修改上次数据长度
                        {
                            Array.Copy(data, 0, idata.data, sumLen, len);
                            idata.insertInfo[(idata.insertNum - 1) * 2 + 1] += len;
                        }
                        else  //不相连，添加新的数据
                        {
                            idata.insertInfo[idata.insertNum * 2] = addr;
                            idata.insertInfo[idata.insertNum * 2 + 1] = len;
                            idata.insertNum++;
                            Array.Copy(data, 0, idata.data, sumLen, len);
                        }
                    }
                }//end if
            }//end for
            //（4）结束遍历，把最后的插入帧加入链表
            this.insertList.Add(idata);
        }

        //======================================================================
        //函数名称：structToBytes
        //函数返回：结构体转换为对应的byte数组
        //参数说明：structObj:结构体数据;size:结构体数据字节大小
        //功能概要：将结构体数据转换为对应的byte数据
        //======================================================================
        private byte[] structToBytes(object structObj, int size)
        {
            //（1）变量声明
            byte[] bytes;
            IntPtr structPtr;

            //（2）变量空间申请
            bytes = new byte[size];
            structPtr = Marshal.AllocHGlobal(size);
            //（3）将结构体数据拷入分配号的内存空间
            Marshal.StructureToPtr(structObj, structPtr, false);
            //（4）从内存空间拷贝到byte数组
            Marshal.Copy(structPtr, bytes, 0, size);
            //（5）释放内存空间
            Marshal.FreeHGlobal(structPtr);

            return bytes;
        }

        //======================================================================
        //函数名称：bytesToStruct
        //函数返回：byte数组转换为对应的结构体
        //参数说明：bytes:字节数组;type:结构体类型
        //功能概要：将byte字节数组数据转换为对应的结构体数据
        //======================================================================
        private object bytesToStruct(byte[] bytes, Type type)
        {
            //（1）变量声明
            int size;
            object obj;
            IntPtr structPtr;

            size = Marshal.SizeOf(type);
            //（2）判断字节长度
            if (size > bytes.Length) return null;
            //（3）分配结构体内存空间
            structPtr = Marshal.AllocHGlobal(size);
            //（4）将byte数组拷贝到分配好的内存空间
            Marshal.Copy(bytes, 0, structPtr, size);
            //（5）将内存空间转换为目标结构体
            obj = Marshal.PtrToStructure(structPtr, type);
            //（6）释放内存空间
            Marshal.FreeHGlobal(structPtr);

            return obj;
        }

        //======================================================================
        //函数名称：frameEncode
        //函数返回：组帧后的字节数据
        //参数说明：order:命令字节;data:待组帧的数据
        //功能概要：按照命令字节和待组帧数据按照帧类型进行组帧操作
        //          帧头+帧长+命令+数据+校验+帧尾
        //           2B   2B   1B   nB   2B   2B
        //======================================================================
        private byte[] frameEncode(byte order, byte[] data)
        {
            //（1）变量声明
            int len;
            byte[] crc;
            byte[] frameLen;
            byte[] frame = new byte[data.Length + 9];

            len = 0;
            //（2）帧头
            frame[len++] = 0x50;
            frame[len++] = 0x50;
            //（3）帧长
            frameLen = new byte[2];
            frameLen = BitConverter.GetBytes((ushort)frame.Length);
            frame[len++] = frameLen[1];
            frame[len++] = frameLen[0];
            //（4）命令
            frame[len++] = order;
            //（5）数据
            Array.Copy(data, 0, frame, len, data.Length);
            len += data.Length;
            //（6）CRC校验码
            crc = CRC16(data);
            frame[len++] = crc[0];
            frame[len++] = crc[1];
            //（7）帧尾
            frame[len++] = 0x43;
            frame[len++] = 0x43;
            //（8）返回组帧完成后的数据
            return frame;
        }

        //======================================================================
        //函数名称：frameDecode
        //函数返回：true:解帧成功;false:解帧失败
        //参数说明：frame:待解帧的数据;data:解帧后的数据;order:命令字节
        //功能概要：将接收到的frame数据按照帧格式进行解帧操作，
        //          得到数据字节和命令字节
        //          帧头+帧长+命令+数据+校验+帧尾
        //           2B   2B   1B   nB   2B   2B
        //======================================================================
        private bool frameDecode(byte[] frame, ref byte[] data, ref byte order)
        {
            //（1）变量声明
            bool ans;
            int length;
            byte temp;
            byte[] crc;
            byte[] frameLen;

            ans = false;
            length = frame.Length;
            if (length == 0)
                goto frameDecode_EXIT;

            //（2）判断帧头
            if (frame[0] != 0x50 || frame[1] != 0x50) goto frameDecode_EXIT;
            //（3）判断帧长
            frameLen = new byte[2];
            Array.Copy(frame, 2, frameLen, 0, 2);
            temp = frameLen[0];
            frameLen[0] = frameLen[1];
            frameLen[1] = temp;
            if (length != BitConverter.ToUInt16(frameLen, 0)) goto frameDecode_EXIT;
            //（4）获取命令
            order = frame[4];
            //（6）数据解析
            data = new byte[length - 9];
            Array.Copy(frame, 5, data, 0, length - 9);
            //（7）判断CRC校验码
            crc = CRC16(data);
            if (crc[0] != frame[length - 4] || crc[1] != frame[length - 3]) goto frameDecode_EXIT;
            //（8）判断帧尾
            if (frame[length - 2] != 0x43 || frame[length - 1] != 0x43) goto frameDecode_EXIT;
            //（9）至此，数据解帧成功
            ans = true;

        frameDecode_EXIT:
            return ans;
        }

        //======================================================================
        //函数名称：CRC16
        //函数返回：进行CRC校验后的校验值
        //参数说明：data:待进行CRC校验的数据
        //功能概要：对data数据进行CRC校验，返回16位校验值
        //======================================================================
        private byte[] CRC16(byte[] data)
        {
            //（1）定义本函数使用的变量
            int len = data.Length;
            ushort crc;
            int i, j;
            byte hi;           //高位置.
            byte lo;           //低位置.
            if (len > 0)
            {
                //产生data的CRC校验码
                crc = 0xFFFF;
                for (i = 0; i < len; i++)
                {
                    crc = (ushort)(crc ^ (data[i]));
                    for (j = 0; j < 8; j++)
                    {
                        crc = (crc & 1) != 0 ? (ushort)((crc >> 1) ^ 0xA001) : (ushort)(crc >> 1);
                    }
                }
                hi = (byte)((crc & 0xFF00) >> 8);  //高位置.
                lo = (byte)(crc & 0x00FF);         //低位置.
                return new byte[] { hi, lo };
            }
            return new byte[] { 0, 0 };
        }
    }
}
