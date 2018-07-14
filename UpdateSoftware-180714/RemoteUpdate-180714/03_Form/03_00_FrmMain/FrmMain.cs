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
    public partial class frm_main : Form
    {
        //（1）定义使用的局部变量
        private frm_update mfrm_update;  //frm_update窗体

        //（2）定义使用的全局变量

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
        }

        //======================================================================
        //函数名称：tsm_update_Click
        //函数返回：无
        //参数说明：无
        //功能概要：打开软件更新页面
        //======================================================================
        private void tsm_update_Click(object sender, EventArgs e)
        {
            //（1）软件更新页面是否已打开
            if (mfrm_update == null || mfrm_update.IsDisposed)
            {
                //新建frm_update窗体
                mfrm_update = new frm_update();
                //设置mfrm_update的父窗体为本窗体
                mfrm_update.MdiParent = this;
            }
            //（2）显示软件更新页面
            mfrm_update.Hide();  //置顶显示
            mfrm_update.Show();
            //（3）提示
            lbl_mainstatus.Text = "运行状态：打开软件更新页面";
        }

        //======================================================================
        //函数名称：tmr_tick_Tick
        //函数返回：无
        //参数说明：无
        //功能概要：每1秒执行一次，更新显示当前系统时间
        //======================================================================
        private void tmr_tick_Tick(object sender, EventArgs e)
        {
            lbl_curtime.Text = "当前时间：" + System.DateTime.Now.ToString();
        }
    }
}
