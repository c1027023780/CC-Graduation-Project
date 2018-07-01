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
    public partial class frm_main : Form
    {
        private frm_file mfrm_file = new frm_file();  //新建frm_file窗体
        private frm_uart mfrm_uart = new frm_uart();  //新建frm_uart窗体

        public List<hexStruct> hexList;

        public int USERBASE = 0x7800;    //用户代码首地址
        public int USERADDRESS = 0x7800;  //单片机更新数据起始地址,本代码每次更新擦除所有用户代码

        //======================================================================
        //函数名称：frm_main
        //函数返回：frm_main窗体构造函数
        //参数说明：无
        //功能概要：完成frm_main窗体的初始化工作
        //======================================================================
        public frm_main()
        {
            InitializeComponent();
        }

        //======================================================================
        //函数名称：frm_main_Load
        //函数返回：无
        //参数说明：无
        //功能概要：窗体加载事件，窗体加载时自动调用
        //======================================================================
        private void frm_main_Load(object sender, EventArgs e)
        {
            //（1）设置允许跨线程操作UI控件
            System.Windows.Forms.Control.CheckForIllegalCrossThreadCalls = false;
            //（2）设置mfrm_file和mfrm_uart的父窗体为本窗体
            mfrm_file.MdiParent = this;
            mfrm_uart.MdiParent = this;
        }

        //======================================================================
        //函数名称：tsm_openhex_Click
        //函数返回：无
        //参数说明：无
        //功能概要：打开hex文件，读取并解析hex文件信息
        //======================================================================
        public void tsm_openhex_Click(object sender, EventArgs e)
        {
            //（1）文件操作页面显示
            mfrm_file.Hide();   //置顶显示
            mfrm_file.Show();
        }

        //======================================================================
        //函数名称：tsm_uart_Click
        //函数返回：无
        //参数说明：无
        //功能概要：打开uart操作界面，具体为将mfrm_uart窗体置顶并显示
        //======================================================================
        private void tsm_uart_Click(object sender, EventArgs e)
        {
            //（1）UART页面显示
            mfrm_uart.Hide();  //置顶显示
            mfrm_uart.Show();
        }

        //======================================================================
        //函数名称：tmr_tick_Tick
        //函数返回：无
        //参数说明：无
        //功能概要：每1秒执行一次，更新显示当前系统时间。
        //======================================================================
        private void tmr_tick_Tick(object sender, EventArgs e)
        {
            lbl_time.Text = System.DateTime.Now.ToString();
        }

        
    }
}
