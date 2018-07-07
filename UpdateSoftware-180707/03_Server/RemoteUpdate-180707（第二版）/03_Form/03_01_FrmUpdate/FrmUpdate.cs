using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace RemoteUpdate
{
    public partial class frm_update : Form
    {
        //（1）定义使用的局部变量
        private frm_main fmain;  //主页面变量

        private Hex hex;  //Hex文件信息

        private System.IO.Ports.SerialPort uartPort;
        private Uart uart;      //uart变量
        private PCNode pcNode;  //pc节点变量

        private Update update;  //更新变量

        //（2）定义使用的全局变量

        //======================================================================
        //函数名称：frm_update
        //函数返回：frm_update窗体构造函数
        //参数说明：无
        //功能概要：完成frm_update窗体的初始化工作
        //======================================================================
        public frm_update()
        {
            InitializeComponent();
        }

        //======================================================================
        //函数名称：frm_update_Load
        //函数返回：无
        //参数说明：无
        //功能概要：窗体加载事件，窗体加载时自动调用
        //======================================================================
        private void frm_update_Load(object sender, EventArgs e)
        {
            //（1）本窗体由frmMain创建，所以本窗体的父窗体为frmMain
            fmain = (frm_main)this.MdiParent;  //得到frmMain窗体变量
            //（2）初始化Hex变量
            hex = new Hex();

            
        }

        //======================================================================
        //函数名称：btn_fileopen_Click
        //函数返回：无
        //参数说明：无
        //功能概要：导入Hex文件并对该文件进行解析取出其有效数据
        //======================================================================
        private void btn_fileopen_Click(object sender, EventArgs e)
        {
            //（1）变量声明
            string filePath;       //文件路径
            string fileName;       //文件名
            string line;           //Hex文件行数据

            //（2）导入Hex文件
            OpenFileDialog ofd = new OpenFileDialog();  //打开文件对话框
            ofd.Filter = "hex file(*.hex)|*.hex";
            ofd.ShowDialog();
            //（3）获取文件名
            filePath = ofd.FileName;
            fileName = filePath.Substring(filePath.LastIndexOf("\\") + 1);//最后一个\\后是文件名
            if (fileName == "")
            {
                fmain.lbl_mainstatus.Text = "运行状态：未选择Hex文件";
                this.pnl_updatestyle.Enabled = false;  //Hex文件导入失败，不允许更新操作
                return;
            }
            this.lbl_filename.Text = "文件名：" + fileName;  //显示文件名
            fmain.lbl_mainstatus.Text = "运行状态：" + fileName + "文件导入成功";
            //（4）获取文件数据，并显示在文件显示框
            //（4.1）打开文件
            FileStream fs = new FileStream(filePath, FileMode.Open);
            if (fs == null)  //文件打开失败
            {
                MessageBox.Show("Hex文件路径错误!", "错误", MessageBoxButtons.OK, MessageBoxIcon.Stop);
                this.pnl_updatestyle.Enabled = false;  //Hex文件打开失败，不允许更新操作
                return;
            }
            fmain.lbl_mainstatus.Text = "运行状态：" + fileName + "文件打开成功，正在读取文件数据...";
            //（4.2）清空显示框
            this.lst_codeshow.Items.Clear();  ////清空文件显示框
            //（4.3）读取数据
            StreamReader sr = new StreamReader(fs);  //读取数据流
            hex.clear();  //先清空hex数据，准备接收hex数据
            while (true)
            {
                line = sr.ReadLine(); //读取1行数据
                if (line == null) break;  //文件读取错误，退出
                hex.addLine(line);  //获取hex文件行数据有效数据
                this.lst_codeshow.Items.Add(line);//文本显示框显示数据
            }
            sr.Close();  //关闭数据流
            fs.Close();
            fmain.lbl_mainstatus.Text = "运行状态：" + fileName + "文件数据获取成功";
            //（5）文件打开成功，允许更新操作
            this.pnl_updatestyle.Enabled = true;
        }

        //======================================================================
        //函数名称：rdo_uart_CheckedChanged
        //函数返回：无
        //参数说明：无
        //功能概要：点击“串口更新”单选按钮，显示串口更新页面并使用串口方式连接终端
        //======================================================================
        private void rdo_uart_CheckedChanged(object sender, EventArgs e)
        {
            //（1）变量申明
            string com;

            //（2）若串口更新未选中，直接退出
            if (this.rdo_uart.Checked == false)
                return;

            //（3）串口更新页面显现
            this.pnl_uart.Visible = true;
            fmain.lbl_mainstatus.Text = "运行状态：选择串口更新";
            //（4）寻找并连接终端
            if (uart != null && uart.IsOpen) uart.close();
            pcNode = new PCNode();//pc节点初始化
            com = pcNode.findPCNode();
            uart = pcNode.PCNode_Uart;
            this.lbl_uartstate.Text = com;
            //（5）绑定DataReceived事件(即串口接收中断处理函数）
            if (uart != null)
            {
                //（5.1）连接串口成功
                uartPort = uart.port;
                uartPort.DataReceived -= new System.IO.Ports.SerialDataReceivedEventHandler(this.uartPort_recvData);//先移除委托函数，防止多次触发
                uartPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.uartPort_recvData);
                uart.setReceInt(500);  //相当于关闭串口中断

                this.btn_autoUpdate.Enabled = true;  //允许串口更新操作
                //修改主页面提示信息
                fmain.lbl_mainstatus.Text = "运行状态：找到" + com;
                fmain.lbl_protocal.Text = "协议：串口";
                fmain.lbl_protocalinfo.Text = "协议信息：端口" + com + ",波特率115200";
            }
            else
            {
                //（5.2）连接串口失败
                this.btn_autoUpdate.Enabled = false;  //禁止串口更新操作
                //修改主页面提示信息
                fmain.lbl_mainstatus.Text = "运行状态：未成功找到串口";
                fmain.lbl_protocal.Text = "协议：";
                fmain.lbl_protocalinfo.Text = "协议信息：";
            }
        }

        //======================================================================
        //函数名称：rdo_uart_CheckedChanged
        //函数返回：无
        //参数说明：无
        //功能概要：点击“远程更新”单选按钮，显示远程更新页面并侦听指定端口（暂未实现）
        //======================================================================
        private void rdo_remote_CheckedChanged(object sender, EventArgs e)
        {
            //（2）若远程更新未选中，直接退出
            if (this.rdo_remote.Checked == false)
                return;

            this.pnl_uart.Visible = false;
            fmain.lbl_mainstatus.Text = "运行状态：选择远程更新";
            fmain.lbl_protocal.Text = "协议：";
            fmain.lbl_protocalinfo.Text = "协议信息：";
        }

        //======================================================================
        //函数名称：btn_uartCheck_Click
        //函数返回：无
        //参数说明：无
        //功能概要：“重新连接”按钮点击事件，重新连接终端
        //======================================================================
        private void btn_uartCheck_Click(object sender, EventArgs e)
        {
            //（1）变量申明
            string com;

            //（2）提示重新连接串口
            fmain.lbl_mainstatus.Text = "运行状态：单击“重新连接”按钮...";
            //（3）重新遍历串口，寻找终端
            if (uart != null && uart.IsOpen) uart.close();
            pcNode = new PCNode();//pc节点初始化
            com = pcNode.findPCNode();
            uart = pcNode.PCNode_Uart;
            this.lbl_uartstate.Text = com;
            //（4）绑定DataReceived事件(即串口接收中断处理函数）
            if (uart != null)
            {
                //（4.1）连接串口成功
                uartPort = uart.port;
                uartPort.DataReceived -= new System.IO.Ports.SerialDataReceivedEventHandler(this.uartPort_recvData);//先移除委托函数，防止多次触发
                uartPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.uartPort_recvData);
                uart.setReceInt(500);  //相当于关闭串口中断

                this.btn_autoUpdate.Enabled = true;  //允许串口更新操作
                //修改主页面提示信息
                fmain.lbl_mainstatus.Text = "运行状态：找到" + com;
                fmain.lbl_protocal.Text = "协议：串口";
                fmain.lbl_protocalinfo.Text = "协议信息：端口" + com + ",波特率115200";
            }
            else
            {
                //（4.2）连接串口失败
                this.btn_autoUpdate.Enabled = false;  //禁止串口更新操作
                //修改主页面提示信息
                fmain.lbl_mainstatus.Text = "运行状态：未成功找到串口";
                fmain.lbl_protocal.Text = "协议：";
                fmain.lbl_protocalinfo.Text = "协议信息：";
            }
        }

        //======================================================================
        //函数名称：btn_autoUpdate_Click
        //函数返回：无
        //参数说明：无
        //功能概要：“一键自动更新”按钮点击事件，进行程序更新操作
        //======================================================================
        private void btn_autoUpdate_Click(object sender, EventArgs e)
        {
            //（1）变量声明
            int flag;
            int cur;
            int sum;            
            byte[] startUpdate = { (byte)'u', (byte)'p', (byte)'d', (byte)'a', (byte)'t', (byte)'e' };

            flag = 0;
            //（2）若未导入Hex文件或串口连接失败则退出（防错用）
            if (hex.getHexList() == null)
            {
                fmain.lbl_mainstatus.Text = "运行状态：Hex文件未选择";
                return;
            }
            if (uart == null)
            {
                fmain.lbl_mainstatus.Text = "运行状态：串口未选择";
                return;
            }

            //（3）更新过程中不允许导入新的Hex文件、切换更新方式、重连串口等操作
            this.grp_filechoose.Enabled = false;
            this.pnl_updatestyle.Enabled = false;
            this.pnl_uart.Enabled = false;

            //（4）Hex文件选择成功、串口选择成功则进行自动更新操作
            //（4.1）发送开始更新提示
            if (uartSendRecv(startUpdate, 2, 1000))
            {
                fmain.lbl_mainstatus.Text = "运行状态：开始程序更新操作";
                txtShow("开始程序更新操作\r\n");
            }
            //（4.2）更新操作初始化
            update = new Update(hex.getHexList());
            sum = update.getFrameNum();  //获取待发送的总帧数
            //（4.3）发送第一帧数据，需等待较长时间
            if (uartSendRecv(update.getNextFrame(), 2, 2000))
            {
                //获取当前已发送的帧数
                cur = update.getCurCount();
                this.prg_update.Value = cur * 100 / sum;  //进度条显示
                //成功提示
                fmain.lbl_mainstatus.Text = "运行状态：当前第" + cur + "帧数据，总共" + sum + "帧数据";
                txtShow("当前第" + cur + "帧数据，总共" + sum + "帧数据\r\n");
            }
            //（4.4）从第二帧数据开始等待时间可调小
            while (!update.isEmpty())
            {
                //将帧数据发送至终端，并接收返回值
                if (uartSendRecv(update.getNextFrame(), 2, 1000))
                {
                    //获取当前已发送的帧数
                    cur = update.getCurCount();
                    this.prg_update.Value = cur * 100 / sum;  //进度条显示
                    //成功提示
                    fmain.lbl_mainstatus.Text = "运行状态：当前第" + cur + "帧数据，总共" + sum + "帧数据";
                    txtShow("当前第" + cur + "帧数据，总共" + sum + "帧数据\r\n");

                    flag = 0;
                }
                else
                {
                    //最后的更新命令帧无需返回数据
                    if (update.isEmpty())
                    {
                        break;//跳出循环
                    }

                    flag++;
                    fmain.lbl_mainstatus.Text = "运行状态：当前更新数据发送失败" + flag + "次";
                    txtShow("当前更新数据发送失败" + flag + "次\r\n");
                    if (flag >= 3)  //若失败次数超过3次，结束当前数据烧写操作
                    {
                        //错误提示
                        fmain.lbl_mainstatus.Text = "运行状态：数据烧录操作失败";
                        txtShow("程序烧录操作失败\r\n");
                        goto btn_autoUpdate_Click_EXIT;
                    }
                }
            }
            //（5）程序更新成功
            fmain.lbl_mainstatus.Text = "运行状态：程序更新成功";
            txtShow("程序更新成功");
            this.prg_update.Value = 100;

        btn_autoUpdate_Click_EXIT:
            //（6）程序更新操作结束，重新允许导入新的Hex文件、切换更新方式、重连串口等操作
            this.grp_filechoose.Enabled = true;
            this.pnl_updatestyle.Enabled = true;
            this.pnl_uart.Enabled = true;

            return;
        }

        //======================================================================
        //函数名称：btn_uartClear_Click
        //函数返回：无
        //参数说明：无
        //功能概要：“清空显示框”按钮点击事件，清空更新操作提示框
        //======================================================================
        private void btn_uartClear_Click(object sender, EventArgs e)
        {
            this.txt_updateinfo.Text = "";
        }







        private void uartPort_recvData(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {

        }

        //======================================================================
        //函数名称：uartSendRecv
        //函数返回：true:数据帧发送成功并接收到返回值;
        //          false:数据帧发送后未接收到返回值或返回值解析失败
        //参数说明：sendBytes:待发送的数据;cnt:重复发送次数;
        //          time:发送后等待接收返回值的时间
        //功能概要：串口发送数据，并等待接收返回值
        //======================================================================
        private bool uartSendRecv(byte[] sendBytes, int cnt, int time)
        {
            //（1）变量声明
            bool rv;
            bool sendFlag;
            byte[] recvData;  //返回数据

            rv = true;
            //（2）提示发送数据操作
            fmain.lbl_mainstatus.Text = "运行状态：正发送数据至终端...";
            txtShow("\r\n正发送数据至终端,等待返回数据...\r\n");
            //（3）发送数据帧到终端
            uart.recvData();//在发送数据帧之前清空接收缓冲区
            //数据发送并等待指定时间接收返回值
            sendFlag = pcNode.PCNodeSendReturn(sendBytes, out recvData, cnt, time);
            //（4）未接收到数据则更新状态栏
            if (recvData == null || recvData.Length <= 0)
            {
                fmain.lbl_mainstatus.Text = "运行状态：终端无数据返回";
                txtShow("终端无数据返回\r\n");

                //提示失败
                rv = false;
                goto uartSendRecv_EXIT;
            }
            //（5）成功接收数据，提示并进行数据解析
            fmain.lbl_mainstatus.Text = "运行状态：接收到终端数据返回，数据解析中...";
            txtShow("接收到终端数据返回，数据解析中...\r\n");
            //（5.1）终端接收到更新提示，跳转bootloader程序并返回Start Update
            if (System.Text.Encoding.Default.GetString(recvData) == "Start Update")
            {
                fmain.lbl_mainstatus.Text = "运行状态：程序进入bootloader，开始更新";
                txtShow("程序进入bootloader，开始更新\r\n");

                rv = true;
                goto uartSendRecv_EXIT;
            }
            //（5.2）接收到更新返回数据
            else if (update != null && update.updateRecv(recvData) != 0)  //数据解析失败
            {
                fmain.lbl_mainstatus.Text = "运行状态：数据解析失败";
                txtShow("数据解析失败\r\n");

                rv = false;
                goto uartSendRecv_EXIT;
            }
            //（6）至此，数据接收成功
            fmain.lbl_mainstatus.Text = "运行状态：数据解析成功，成功接收到终端返回";
            txtShow("数据解析成功，成功接收到终端返回\r\n");

            uartSendRecv_EXIT:
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
            this.txt_updateinfo.Text += str;
            this.txt_updateinfo.Refresh();
            this.txt_updateinfo.SelectionStart = this.txt_updateinfo.Text.Length;  //光标指向最后一位
            this.txt_updateinfo.ScrollToCaret();//移动到光标处
        }



    }
}
