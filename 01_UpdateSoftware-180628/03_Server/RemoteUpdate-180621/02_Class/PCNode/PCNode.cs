using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace RemoteUpdate
{
    public partial class PCNode
    {
        /// ------------------------------------------------------------------------------
        /// <summary>                                                           
        /// 类          :Find_PC_Node:KW01寻找PC节点工具                                     
        /// 类   功   能:通过串口向PC节点发送握手包,确认连接              
        /// 类中函数包含:  
        ///             (1)shake:握手函数,通过串口向PC节点发握手信号,确认是否连接
        ///             (2)flashWrite:flash写函数
        /// </summary>                                              
        /// ------------------------------------------------------------------------------
        /// 
        //串口操作工具类
        private Uart uart1;

        public Uart PCNode_Uart
        {
            get { return this.uart1; }
        }
        /// ------------------------------------------------------------------------------
        /// <summary>  
        /// 名    称:(0)Find_PC_Node:构造函数 
        /// 功    能: 创建KW01寻节点实例                                  
        /// 内部参数: uart1：  串口工具类
        /// </summary>                                                                     
        /// <param name="uart1">串口工具类</param>                                          
        /// <returns>true_串口初始化成功 false_初始化失败</returns>                  
        /// ------------------------------------------------------------------------------
        public PCNode()
        {
            uart1 = null;
        }


        /// ------------------------------------------------------------------------------
        /// <summary>  
        /// 功    能:寻找PCNode。                                  
        /// 形    参:无
        /// <returns>无</returns>                  
        /// -----------------------------------------------------------------------------
        public string findPCNode()
        {
            int i;
            string s1;
            string recvstr;
            int[] uartNoArray;   //
            System.IO.Ports.SerialPort uartport;
            //1.临时变量
      
            bool result = false;     //
            byte[] recvData = null;  //串口接收返回的信息


            byte[] shakeframe = { (byte)'s', (byte)'h', (byte)'a', (byte)'k', (byte)'e' };

            byte[] frame = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

            //初始化内部调用函数临时变量
            s1 = string.Empty;    //初始化临时字符串（置空）

            //创建_port1对象
            uartport = new System.IO.Ports.SerialPort();
            //查询所有串口，串口数在uartNoArray[0]中

            uartNoArray = Uart.findCOM();
            //没有找到串口情况
            if (uartNoArray[0] == 0) return s1 = "没有找到USB串口";
           
            //有找到串口情况，遍历搜寻串口
            for (i = uartNoArray[0]; i >= 1; i--)
            {
                string comName = "COM" + uartNoArray[i].ToString();  //串口名称
                uart1 = new Uart(uartport, comName, 9600); //初始化串口工具类
                //2.打开串口
                result = uart1.open();
                if (!result) continue;
                uart1.setReceInt(200);  //设置触发DataRecieved事件的字节数（目的是关闭该事件）
                uart1.sendData(frame, frame.Length);  //预发送数据，防止下位机组帧出错
                System.Threading.Thread.Sleep(100);   //线程休眠0.1s
                result = uart1.sendData(shakeframe, shakeframe.Length); //发送握手数据
                if (!result) { uart1.close(); continue; }
                System.Threading.Thread.Sleep(100);   //线程休眠0.1s
                recvData = uart1.recvData();          //接收数据 
                if (recvData == null || recvData.Length == 0) 
                   { uart1.close(); continue; }

                //将握手成功,数组转换成字符串,返回
                //recvstr = System.Text.Encoding.Default.GetString(recvData);
                //if (recvstr != "shake")
                //{
                //    uart1.close(); continue;
                //}

                s1 = comName;
                break;
             }
            if (i == 0)
            {
                s1 = "有USB串口，但未连上终端";
                uart1 = null;
            }
             
             return s1;
        }

        public bool PCNodeSendReturn(byte[] SendByteArray, out byte[] RecvData, int cnt, int time)
        {
            int i;
            bool sendflag=false;
            RecvData = null;
            try
            {

                for (i = 0; i < cnt; i++)
                {
                    uart1.setReceInt(500);   //测试串口接收中断使用
                    sendflag=uart1.sendData(SendByteArray, SendByteArray.Length);  // 通过串口发送数据
                    System.Threading.Thread.Sleep(time);
                    RecvData = uart1.recvData();                          // 通过串口接收数据
                    if (RecvData.Length >= 1) break;
                }
               
            }
            catch
            {
                
                return false;  //产生错误,返回false
            }
            return sendflag;    // 返回接收数据数据的首地址
        }

    }
}        
