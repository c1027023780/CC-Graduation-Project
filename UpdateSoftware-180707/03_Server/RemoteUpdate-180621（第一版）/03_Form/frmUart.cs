using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace RemoteUpdate
{
    public partial class frm_uart : Form
    {
        private frm_main fmain;  //主页面变量

        private System.IO.Ports.SerialPort uartPort;  
        private Uart uart;      //uart变量
        private PCNode pcNode;  //pc节点变量
        private Update update;  //更新变量

        //======================================================================
        //函数名称：frm_uart
        //函数返回：frm_uart窗体构造函数
        //参数说明：无
        //功能概要：完成frm_uart窗体的初始化工作
        //======================================================================
        public frm_uart()
        {
            InitializeComponent();

            pcNode = new PCNode();  //pc节点初始化
        }

        //======================================================================
        //函数名称：frm_uart_Load
        //函数返回：无
        //参数说明：无
        //功能概要：窗体加载事件，窗体加载时自动调用
        //======================================================================
        private void frm_uart_Load(object sender, EventArgs e)
        {
            //（1）变量申明
            string com;

            //（1）设置允许跨线程操作UI控件
            System.Windows.Forms.Control.CheckForIllegalCrossThreadCalls = false;
            //（2）获得主页面窗体
            fmain = (frm_main)this.MdiParent;
            //（3）更新状态提示信息
            fmain.lbl_status.Text = "运行状态：frm_uart_Load函数开始，寻找终端节点...";
            //（4）调用pcNode类中的函数，寻找下位机，并更新设备连接状态信息
            if(uart != null && uart.IsOpen) uart.close();
            com = pcNode.findPCNode();
            uart = pcNode.PCNode_Uart;
            fmain.lbl_status.Text = "运行状态：找到" + com;
            lbl_uart_state.Text = com;
            //（5）绑定DataReceived事件(即串口接收中断处理函数）
            if (uart != null)
            {
                uartPort = uart.port;
                uartPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.uartPort_recvData);
                uart.setReceInt(500);  //相当于关闭串口中断
                this.btn_autoUpdate.Enabled = true;
            }
            //（6）修改其他提示信息
            fmain.lbl_protocal.Text = "协议：串口";
            fmain.lbl_protocalinfo.Text = "协议信息：端口" + com + ",波特率9600";
        }

        //======================================================================
        //函数名称：btn_uartcheck_Click
        //函数返回：无
        //参数说明：无
        //功能概要：“重新连接”按钮点击事件，实现上位机与终端的重新连接操作
        //======================================================================
        private void btn_uartcheck_Click(object sender, EventArgs e)
        {
            //（1）变量申明
            string com;

            //（2）更新状态提示信息
            fmain.lbl_status.Text = "运行状态：单机“重新连接”按钮...";
            //（3）重新遍历串口，寻找下位机
            if (uart != null && uart.IsOpen) uart.close();
            com = pcNode.findPCNode();
            uart = pcNode.PCNode_Uart;
            fmain.lbl_status.Text = "运行状态：找到" + com;
            lbl_uart_state.Text = com;
            //（4）重新绑定DataReceived事件(即串口接收中断处理函数）
            if (uart != null)
            {
                uartPort = uart.port;
                uartPort.DataReceived -= new System.IO.Ports.SerialDataReceivedEventHandler(this.uartPort_recvData);//先移除委托函数，防止多次触发
                uartPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.uartPort_recvData);
                uart.setReceInt(500);  //相当于关闭串口中断
                this.btn_autoUpdate.Enabled = true;  //使能连接终端操作
            }
            else
            {
                this.btn_autoUpdate.Enabled = false;  //使能连接终端操作
            }
            //（5）修改其他提示信息
            fmain.lbl_protocal.Text = "协议：串口";
            fmain.lbl_protocalinfo.Text = "协议信息：端口" + com + ",波特率9600";
        }

        //======================================================================
        //函数名称：btn_uartlink_Click
        //函数返回：无
        //参数说明：无
        //功能概要：“连接检测”按钮点击事件，上位机告知终端开启更新操作
        //======================================================================
        private void btn_autoUpdate_Click(object sender, EventArgs e)
        {
            //（1）变量声明
            int sum;
            int cur;
            int flag;
            byte[] startUpdate = {(byte)'u',(byte)'p',(byte)'d',(byte)'a',(byte)'t',(byte)'e'};

            flag = 0;
            //（2）若未导入hex文件或串口连接失败则退出
            if (fmain.hexList == null)
            {
                fmain.lbl_status.Text = "运行状态：Hex文件未选择";
            }
            else if (uart == null)
            {
                fmain.lbl_status.Text = "运行状态：串口未选择";
            }
            //（3）hex文件选择成功、串口选择成功则进行连接自动更新操作
            else
            {
                if (sendRecv(startUpdate, 2, 1000))
                {
                    fmain.lbl_status.Text = "运行状态：开始发送更新数据";
                    txtShow("开始发送更新数据\r\n");
                }
                update = new Update(fmain.hexList);
                update.setUserAddress(fmain.USERADDRESS);
                sum = update.getFrameNum();  //获取帧数

                //发送第一帧数据需等待较长时间
                if (sendRecv(update.getNextFrame(), 2, 2000))  
                {
                    //成功提示
                    fmain.lbl_status.Text = "运行状态：更新帧数据发送成功";
                    //获取当前已发送的帧数
                    cur = update.getCurCount();
                    this.prg_uartburn.Value = cur * 100 / sum;  //进度条显示
                    txtShow("当前第" + cur + "帧数据，总共" + sum + "帧数据\r\n");
                }
                //从第二帧数据开始等待时间可调小
                while (!update.isEmpty())
                {
                    //将帧数据发送至终端，并接收返回值
                    if (sendRecv(update.getNextFrame(), 2, 1000))
                    {
                        //成功提示
                        fmain.lbl_status.Text = "运行状态：更新帧数据发送成功";
                        //获取当前已发送的帧数
                        cur = update.getCurCount();
                        this.prg_uartburn.Value = cur * 100 / sum;  //进度条显示
                        txtShow("当前第" + cur + "帧数据，总共" + sum + "帧数据\r\n");

                        flag = 0;
                    }
                    else
                    {
                        flag++;
                        fmain.lbl_status.Text = "运行状态，更新帧数据发送失败";
                        if (flag >= 3)  //若失败次数超过3次，结束当前数据烧写操作
                        {
                            //错误提示
                            fmain.lbl_status.Text = "运行状态：数据烧录操作失败";
                            txtShow("程序烧录失败\r\n");
                            goto btn_autoUpdate_Click_EXIT;
                        }
                    }
                }
                fmain.lbl_status.Text = "运行状态：程序更新成功";
                txtShow("程序更新成功");
                this.prg_uartburn.Value = 100;
            }

                //至此，数据烧写操作成功
        btn_autoUpdate_Click_EXIT:
            return;
        }

        //======================================================================
        //函数名称：btn_uartburn_Click
        //函数返回：无
        //参数说明：无
        //功能概要：“程序烧写”按钮点击事件，上位机向终端发送更新数据，
        //          每次有效数据为256字节
        //======================================================================
        private void btn_uartburn_Click(object sender, EventArgs e)
        {
        //    //（1）变量声明
        //    int sum;
        //    int cur;
        //    int flag;
        //    byte[] sendBytes;  //每次发送的帧数据

        //    flag = 0;
        //    sum = update.getFrameNum();  //获取帧数
        //    //（2）读取hex文件中的所有帧数据
        //    while (!update.isEmpty())  //若当前还有待发送的帧数据
        //    {
        //        sendBytes = update.getNextFrame();  //读取下一帧数据
        //        //将帧数据发送至终端，并接收返回值
        //        if (sendRecv(sendBytes, 2, 500))  //数据接收成功
        //        {
        //            //成功提示
        //            fmain.lbl_status.Text = "运行状态：一帧数据发送成功";
        //            //获取当前已发送的帧数
        //            cur = update.getCurFrame();
        //            this.prg_uartburn.Value = cur * 100 / sum;  //进度条显示
        //            this.lbl_uartprg.Text = "当前第" + cur + "帧数据，总共" + sum + "帧数据";
        //            this.lbl_uartprg.Refresh();
        //            txtShow("当前第" + cur + "帧数据，总共" + sum + "帧数据\r\n");

        //            flag = 0;
        //        }
        //        else  //数据接收失败
        //        {
        //            flag++;  //失败次数加1
        //            fmain.lbl_status.Text = "运行状态，一帧数据发送失败";
        //            if (flag >= 3)  //若失败次数超过3次，结束当前数据烧写操作
        //            {
        //                //错误提示
        //                fmain.lbl_status.Text = "运行状态：数据烧录操作失败";
        //                txtShow("程序烧录失败\r\n");
        //                goto btn_uartburn_Click_EXIT;
        //            }
        //        }
        //    }
        //    //（3）所有帧数据已发送，最后发送检查帧数据
        //    sendBytes = update.getCheckFrame();  //获取检查帧数据
        //    if (sendRecv(sendBytes, 2, 500))  //发送检查帧数据，并接收返回数据
        //    {
        //        //成功提示
        //        fmain.lbl_status.Text = "运行状态：检查帧发送成功，程序烧录成功";
        //        this.lbl_uartburn.Text = "检查帧发送成功\r\n 程序烧录成功";
        //        txtShow("检查帧发送成功，程序烧录成功\r\n");
        //        //使能程序更新按钮
        //        this.btn_uartburn.Enabled = false;
        //        this.btn_uartupdate.Enabled = true;
        //    }
        //    else
        //    {
        //        //错误提示
        //        fmain.lbl_status.Text = "运行状态：检查帧发送失败，需继续程序烧录";
        //        this.lbl_uartburn.Text = "检查帧检测失败";
        //        txtShow("检查帧检测失败\r\n");
        //        goto btn_uartburn_Click_EXIT;
        //    }
        //    //至此，数据烧写操作成功
        //btn_uartburn_Click_EXIT:
        //    return;
        }

        //======================================================================
        //函数名称：btn_uartupdate_Click
        //函数返回：无
        //参数说明：无
        //功能概要：“程序更新”按钮点击事件，上位机向终端发送更新命令，
        //          无需接收返回数据
        //======================================================================
        private void btn_uartupdate_Click(object sender, EventArgs e)
        {
            ////（1）变量声明
            //byte[] sendBytes;

            ////（2）获取更新命令帧
            //sendBytes = update.getUpdateframe();
            ////（3）发送更新命令帧至终端
            //sendRecv(sendBytes, 2, 500);
            ////（4）提示更新操作成功
            //fmain.lbl_status.Text = "运行状态：更新命令帧发送成功，不接收返回数据";
            //this.lbl_uartupdate.Text = "程序更新成功";
            //txtShow("程序更新成功\r\n");
            ////（5）使能“连接检测”按钮
            //this.btn_uartupdate.Enabled = false;
        }

        //======================================================================
        //函数名称：btn_uartclear_Click
        //函数返回：无
        //参数说明：无
        //功能概要：“清空显示框”按钮点击事件，清空终端执行信息提示框
        //======================================================================
        private void btn_uartclear_Click(object sender, EventArgs e)
        {
            this.txt_uartinfo.Text = "";
        }

        private void uartPort_recvData(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {

        }

        //======================================================================
        //函数名称：sendRecv
        //函数返回：true:数据帧发送成功并接收到返回值;
        //          false:数据帧发送后未接收到返回值或返回值解析失败
        //参数说明：sendBytes:待发送的数据;cnt:重复发送次数;
        //          time:发送后等待接收返回值的时间
        //功能概要：串口发送数据，并等待接收返回值
        //======================================================================
        private bool sendRecv(byte[] sendBytes, int cnt, int time)
        {
            //（1）变量声明
            byte[] recvData;
            bool rv;
            bool sendFlag;
            string com;

            rv = true;
            //（2）更新状态栏，显示当前操作
            txtShow("\r\n正在发送...\r\n");
            //（3）发送数据帧到下位机
            uart.recvData();//在发送数据帧之前清空接收缓冲区
            sendFlag = pcNode.PCNodeSendReturn(sendBytes, out recvData, cnt, time);
            //（4）若发送失败，尝试自动重连一次,并且重发
            if (sendFlag == false)
            {
                fmain.lbl_status.Text = "运行状态：数据发送失败，重新连接终端并重发...";
                txtShow("数据发送失败，重新连接终端并重发...\r\n");
                //重连串口
                if (uart != null && uart.IsOpen) uart.close();
                com = pcNode.findPCNode();
                uart = pcNode.PCNode_Uart;
                fmain.lbl_status.Text = "运行状态：找到" + com;
                lbl_uart_state.Text = com;
                //重新发送数据
                sendFlag = pcNode.PCNodeSendReturn(sendBytes, out recvData, cnt, time);
            }
            //（5）未接收到数据则更新状态栏
            if (recvData == null || recvData.Length <= 0)
            {
                fmain.lbl_status.Text = "运行状态：无数据返回";
                txtShow("无数据返回\r\n");

                rv = false;
                goto sendRecv_EXIT;
            }
            //（6）成功接收数据，提示并进行数据解析
            txtShow("有数据返回，数据解析中...\r\n");

            if (System.Text.Encoding.Default.GetString(recvData) == "Start Update")
            {
                fmain.lbl_status.Text = "运行状态：程序进入bootloader，开始更新";
                txtShow("程序进入bootloader，开始更新\r\n");

                rv = true;
                goto sendRecv_EXIT;
            }
            else if (update != null && update.updateRecv(recvData) != 0)  //数据解析失败
            {
                fmain.lbl_status.Text = "运行状态：数据解析失败";
                txtShow("数据解析失败\r\n");

                rv = false;
                goto sendRecv_EXIT;
            }
            //（7）至此，数据接收成功
            fmain.lbl_status.Text = "运行状态：数据解析成功，准备下一帧";
            txtShow("数据解析成功，准备下一帧\r\n");

        sendRecv_EXIT:
            return rv;
        }

        //======================================================================
        //函数名称：txtShow
        //函数返回：无
        //参数说明：str:在终端执行信息提示框追加显示的内容
        //功能概要：在终端执行信息提示框txt_uartinfo中追加提示信息，
        //          并显示最新信息
        //======================================================================
        private void txtShow(string str)
        {
            this.txt_uartinfo.Text += str;
            this.txt_uartinfo.SelectionStart = this.txt_uartinfo.Text.Length;  //光标指向最后一位
            this.txt_uartinfo.ScrollToCaret();//移动到光标处
        }
    }
}
