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
    public partial class frm_file : Form
    {
        private frm_main fmain;  //主页面变量
        Hex hex = new Hex();

        //======================================================================
        //函数名称：frm_file
        //函数返回：frm_file窗体构造函数
        //参数说明：无
        //功能概要：完成frm_file窗体的初始化工作
        //======================================================================
        public frm_file()
        {
            InitializeComponent();
        }

        //======================================================================
        //函数名称：frm_file_Load
        //函数返回：无
        //参数说明：无
        //功能概要：窗体加载事件，窗体加载时自动调用
        //======================================================================
        private void frm_file_Load(object sender, EventArgs e)
        {
            //（1）设置机器码显示框（ListView）列宽随窗体的宽度改变
            lvw_codeparsing.AutoResizeColumns(ColumnHeaderAutoResizeStyle.HeaderSize);
            //（2）本窗体由frmMain创建，所以本窗体的父窗体为frmMain
            fmain = (frm_main)this.MdiParent;  //得到frmMain窗体变量
        }

        //======================================================================
        //函数名称：btn_refile_Click
        //函数返回：无
        //参数说明：无
        //功能概要：“选择文件”按钮对应的点击事件，直接调用frmMain中的
        //          hexFileOpen函数
        //======================================================================
        private void btn_refile_Click(object sender, EventArgs e)
        {
            hexFileOpen();
            fmain.hexList = hex.getHexList();
        }

        //======================================================================
        //函数名称：hexFileOpen
        //函数返回：无
        //参数说明：无
        //功能概要：导入Hex文件并对该文件进行解析取出其有效数据
        //======================================================================
        public void hexFileOpen()
        {
            //（1）变量定义
            int i, j;
            int flashAddress;      //用于机器码显示框开头显示falsh地址
            string filePath;       //文件路径
            string fileName;       //文件名
            string line;           //HEX文件行数据
            string temp;           //中间临时变量
            byte[] hexData;


            //（3）导入hex文件
            OpenFileDialog ofd = new OpenFileDialog();  //打开文件对话框
            ofd.Filter = "hex file(*.hex)|*.hex";
            ofd.ShowDialog();
            //（4）获取文件名
            filePath = ofd.FileName;
            fileName = filePath.Substring(filePath.LastIndexOf("\\") + 1);//最后一个\\后是文件名
            if (fileName == "")
            {
                fmain.lbl_status.Text = "运行状态：未选择文件";
                return;
            }
            this.lbl_filename.Text = "文件名：" + fileName;  //显示文件名
            fmain.lbl_status.Text = "运行状态：hex文件选择成功";
            //（5）获取文件数据，并显示在文件显示框
            //（5.1）打开文件
            FileStream fs = new FileStream(filePath, FileMode.Open);
            if (fs == null)  //文件打开失败
            {
                MessageBox.Show("HEX文件路径错误!", "错误", MessageBoxButtons.OK, MessageBoxIcon.Stop);
                return;
            }
            fmain.lbl_status.Text = "运行状态：" + fileName + "文件打开成功";
            //（5.2）清空显示框
            this.lst_codeshow.Items.Clear();  //清空文件显示框
            this.lvw_codeparsing.Items.Clear();//清空机器码解析显示框
            //（5.3）读取数据
            StreamReader sr = new StreamReader(fs);  //读取数据流
            hex.clear();//先清空hex数据，准备接收hex数据
            while (true)
            {
                line = sr.ReadLine(); //读取1行数据
                if (line == null) break;  //文件读取错误，退出
                hex.addLine(line);  //获取hex文件行数据有效数据
                this.lst_codeshow.Items.Add(line);//文本显示框显示数据
            }
            fmain.lbl_status.Text = "运行状态：" + fileName + "文件数据获取成功";
            sr.Close();  //关闭数据流
            fs.Close();
            //（6）获取hex文件中所有的有效数据
            hex.UserBase = fmain.USERBASE;
            hexData = hex.hexDataCal();
            if (hexData == null)
            {
                fmain.lbl_status.Text = "运行状态：程序数据起始位置小于0x7800，不满足本工程要求";
                return;
            }
            //（7）将有效数据显示在机器码解析显示框
            for (i = 0; i < hexData.Length / 16; i++)
            {
                flashAddress = i * 16;  //每行显示16个数据，可修改
                //每行先显示falsh地址
                temp = Convert.ToString(flashAddress + hex.UserBase, 16).PadLeft(4, '0').ToUpper();
                ListViewItem item = this.lvw_codeparsing.Items.Add(temp + "h");
                //再获取16字节的数据显示
                for (j = 0; j < 16; j++)
                {
                    temp = Convert.ToString(hexData[flashAddress + j], 16).PadLeft(2, '0').ToUpper();
                    item.SubItems.Add(temp);
                }
            }
            if (hexData.Length % 16 != 0)
            {
                flashAddress = i * 16;  //每行显示16个数据，可修改
                //每行先显示falsh地址
                temp = Convert.ToString(flashAddress + hex.UserBase, 16).PadLeft(4, '0').ToUpper();
                ListViewItem item = this.lvw_codeparsing.Items.Add(temp + "h");
                //再获取16字节的数据显示
                for (j = 0; j < hexData.Length % 16; j++)
                {
                    temp = Convert.ToString(hexData[flashAddress + j], 16).PadLeft(2, '0').ToUpper();
                    item.SubItems.Add(temp);
                }
                for (j = hexData.Length % 16; j < 16; j++)
                {
                    temp = "XX";
                    item.SubItems.Add(temp);
                }
            }
            //状态提示
            fmain.lbl_status.Text = "运行状态：" + fileName + "文件数据获取并解析成功";
        }
    }
}
