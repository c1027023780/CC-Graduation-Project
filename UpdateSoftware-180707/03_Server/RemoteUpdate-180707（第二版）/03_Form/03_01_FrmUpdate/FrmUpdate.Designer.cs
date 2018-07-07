namespace RemoteUpdate
{
    partial class frm_update
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.grp_openfile = new System.Windows.Forms.GroupBox();
            this.grp_filechoose = new System.Windows.Forms.GroupBox();
            this.lbl_filename = new System.Windows.Forms.Label();
            this.btn_fileOpen = new System.Windows.Forms.Button();
            this.grp_fileshow = new System.Windows.Forms.GroupBox();
            this.lst_codeshow = new System.Windows.Forms.ListBox();
            this.lbl_codeshow = new System.Windows.Forms.Label();
            this.grp_updatestyle = new System.Windows.Forms.GroupBox();
            this.rdo_uart = new System.Windows.Forms.RadioButton();
            this.pnl_updatestyle = new System.Windows.Forms.Panel();
            this.rdo_remote = new System.Windows.Forms.RadioButton();
            this.lbl_uartstate = new System.Windows.Forms.Label();
            this.pnl_uart = new System.Windows.Forms.Panel();
            this.btn_uartCheck = new System.Windows.Forms.Button();
            this.grp_update = new System.Windows.Forms.GroupBox();
            this.grp_updateinfo = new System.Windows.Forms.GroupBox();
            this.txt_updateinfo = new System.Windows.Forms.TextBox();
            this.btn_uartClear = new System.Windows.Forms.Button();
            this.grp_updateauto = new System.Windows.Forms.GroupBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.btn_autoUpdate = new System.Windows.Forms.Button();
            this.lbl_prgstate = new System.Windows.Forms.Label();
            this.prg_update = new System.Windows.Forms.ProgressBar();
            this.grp_openfile.SuspendLayout();
            this.grp_filechoose.SuspendLayout();
            this.grp_fileshow.SuspendLayout();
            this.grp_updatestyle.SuspendLayout();
            this.pnl_updatestyle.SuspendLayout();
            this.pnl_uart.SuspendLayout();
            this.grp_update.SuspendLayout();
            this.grp_updateinfo.SuspendLayout();
            this.grp_updateauto.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // grp_openfile
            // 
            this.grp_openfile.Controls.Add(this.grp_fileshow);
            this.grp_openfile.Controls.Add(this.grp_filechoose);
            this.grp_openfile.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.grp_openfile.Location = new System.Drawing.Point(12, 12);
            this.grp_openfile.Name = "grp_openfile";
            this.grp_openfile.Size = new System.Drawing.Size(366, 462);
            this.grp_openfile.TabIndex = 0;
            this.grp_openfile.TabStop = false;
            this.grp_openfile.Text = "导入Hex文件";
            // 
            // grp_filechoose
            // 
            this.grp_filechoose.Controls.Add(this.btn_fileOpen);
            this.grp_filechoose.Controls.Add(this.lbl_filename);
            this.grp_filechoose.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.grp_filechoose.Location = new System.Drawing.Point(6, 22);
            this.grp_filechoose.Name = "grp_filechoose";
            this.grp_filechoose.Size = new System.Drawing.Size(354, 66);
            this.grp_filechoose.TabIndex = 0;
            this.grp_filechoose.TabStop = false;
            this.grp_filechoose.Text = "文件名";
            // 
            // lbl_filename
            // 
            this.lbl_filename.AutoSize = true;
            this.lbl_filename.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.lbl_filename.Location = new System.Drawing.Point(6, 28);
            this.lbl_filename.Name = "lbl_filename";
            this.lbl_filename.Size = new System.Drawing.Size(74, 21);
            this.lbl_filename.TabIndex = 1;
            this.lbl_filename.Text = "文件名：";
            // 
            // btn_fileOpen
            // 
            this.btn_fileOpen.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.btn_fileOpen.Location = new System.Drawing.Point(264, 17);
            this.btn_fileOpen.Name = "btn_fileOpen";
            this.btn_fileOpen.Size = new System.Drawing.Size(84, 42);
            this.btn_fileOpen.TabIndex = 2;
            this.btn_fileOpen.Text = "选择文件";
            this.btn_fileOpen.UseVisualStyleBackColor = true;
            this.btn_fileOpen.Click += new System.EventHandler(this.btn_fileopen_Click);
            // 
            // grp_fileshow
            // 
            this.grp_fileshow.Controls.Add(this.lbl_codeshow);
            this.grp_fileshow.Controls.Add(this.lst_codeshow);
            this.grp_fileshow.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.grp_fileshow.Location = new System.Drawing.Point(6, 94);
            this.grp_fileshow.Name = "grp_fileshow";
            this.grp_fileshow.Size = new System.Drawing.Size(354, 362);
            this.grp_fileshow.TabIndex = 2;
            this.grp_fileshow.TabStop = false;
            this.grp_fileshow.Text = "Hex文件显示";
            // 
            // lst_codeshow
            // 
            this.lst_codeshow.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.lst_codeshow.FormattingEnabled = true;
            this.lst_codeshow.ItemHeight = 17;
            this.lst_codeshow.Location = new System.Drawing.Point(6, 39);
            this.lst_codeshow.Name = "lst_codeshow";
            this.lst_codeshow.Size = new System.Drawing.Size(342, 310);
            this.lst_codeshow.TabIndex = 0;
            // 
            // lbl_codeshow
            // 
            this.lbl_codeshow.AutoSize = true;
            this.lbl_codeshow.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.lbl_codeshow.Location = new System.Drawing.Point(111, 14);
            this.lbl_codeshow.Name = "lbl_codeshow";
            this.lbl_codeshow.Size = new System.Drawing.Size(90, 22);
            this.lbl_codeshow.TabIndex = 2;
            this.lbl_codeshow.Text = "文件显示框";
            // 
            // grp_updatestyle
            // 
            this.grp_updatestyle.Controls.Add(this.pnl_uart);
            this.grp_updatestyle.Controls.Add(this.pnl_updatestyle);
            this.grp_updatestyle.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.grp_updatestyle.Location = new System.Drawing.Point(384, 12);
            this.grp_updatestyle.Name = "grp_updatestyle";
            this.grp_updatestyle.Size = new System.Drawing.Size(568, 88);
            this.grp_updatestyle.TabIndex = 1;
            this.grp_updatestyle.TabStop = false;
            this.grp_updatestyle.Text = "更新方式";
            // 
            // rdo_uart
            // 
            this.rdo_uart.AutoSize = true;
            this.rdo_uart.Location = new System.Drawing.Point(3, 3);
            this.rdo_uart.Name = "rdo_uart";
            this.rdo_uart.Size = new System.Drawing.Size(92, 26);
            this.rdo_uart.TabIndex = 0;
            this.rdo_uart.TabStop = true;
            this.rdo_uart.Text = "串口更新";
            this.rdo_uart.UseVisualStyleBackColor = true;
            this.rdo_uart.CheckedChanged += new System.EventHandler(this.rdo_uart_CheckedChanged);
            // 
            // pnl_updatestyle
            // 
            this.pnl_updatestyle.Controls.Add(this.rdo_remote);
            this.pnl_updatestyle.Controls.Add(this.rdo_uart);
            this.pnl_updatestyle.Enabled = false;
            this.pnl_updatestyle.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.pnl_updatestyle.Location = new System.Drawing.Point(18, 35);
            this.pnl_updatestyle.Name = "pnl_updatestyle";
            this.pnl_updatestyle.Size = new System.Drawing.Size(211, 36);
            this.pnl_updatestyle.TabIndex = 1;
            // 
            // rdo_remote
            // 
            this.rdo_remote.AutoSize = true;
            this.rdo_remote.Location = new System.Drawing.Point(117, 3);
            this.rdo_remote.Name = "rdo_remote";
            this.rdo_remote.Size = new System.Drawing.Size(92, 26);
            this.rdo_remote.TabIndex = 1;
            this.rdo_remote.TabStop = true;
            this.rdo_remote.Text = "远程更新";
            this.rdo_remote.UseVisualStyleBackColor = true;
            this.rdo_remote.CheckedChanged += new System.EventHandler(this.rdo_remote_CheckedChanged);
            // 
            // lbl_uartstate
            // 
            this.lbl_uartstate.Font = new System.Drawing.Font("微软雅黑", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.lbl_uartstate.Location = new System.Drawing.Point(9, 17);
            this.lbl_uartstate.Name = "lbl_uartstate";
            this.lbl_uartstate.Size = new System.Drawing.Size(250, 25);
            this.lbl_uartstate.TabIndex = 2;
            this.lbl_uartstate.Text = "当前无可用串口";
            this.lbl_uartstate.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // pnl_uart
            // 
            this.pnl_uart.Controls.Add(this.btn_uartCheck);
            this.pnl_uart.Controls.Add(this.lbl_uartstate);
            this.pnl_uart.Location = new System.Drawing.Point(235, 18);
            this.pnl_uart.Name = "pnl_uart";
            this.pnl_uart.Size = new System.Drawing.Size(327, 63);
            this.pnl_uart.TabIndex = 2;
            this.pnl_uart.Visible = false;
            // 
            // btn_uartCheck
            // 
            this.btn_uartCheck.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.btn_uartCheck.Location = new System.Drawing.Point(265, 3);
            this.btn_uartCheck.Name = "btn_uartCheck";
            this.btn_uartCheck.Size = new System.Drawing.Size(59, 56);
            this.btn_uartCheck.TabIndex = 3;
            this.btn_uartCheck.Text = "重新连接";
            this.btn_uartCheck.UseVisualStyleBackColor = true;
            this.btn_uartCheck.Click += new System.EventHandler(this.btn_uartCheck_Click);
            // 
            // grp_update
            // 
            this.grp_update.Controls.Add(this.groupBox1);
            this.grp_update.Controls.Add(this.grp_updateauto);
            this.grp_update.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.grp_update.Location = new System.Drawing.Point(384, 106);
            this.grp_update.Name = "grp_update";
            this.grp_update.Size = new System.Drawing.Size(272, 368);
            this.grp_update.TabIndex = 2;
            this.grp_update.TabStop = false;
            this.grp_update.Text = "更新操作";
            // 
            // grp_updateinfo
            // 
            this.grp_updateinfo.Controls.Add(this.btn_uartClear);
            this.grp_updateinfo.Controls.Add(this.txt_updateinfo);
            this.grp_updateinfo.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.grp_updateinfo.Location = new System.Drawing.Point(662, 106);
            this.grp_updateinfo.Name = "grp_updateinfo";
            this.grp_updateinfo.Size = new System.Drawing.Size(290, 368);
            this.grp_updateinfo.TabIndex = 3;
            this.grp_updateinfo.TabStop = false;
            this.grp_updateinfo.Text = "更新执行信息";
            // 
            // txt_updateinfo
            // 
            this.txt_updateinfo.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.txt_updateinfo.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.txt_updateinfo.Location = new System.Drawing.Point(7, 15);
            this.txt_updateinfo.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.txt_updateinfo.Multiline = true;
            this.txt_updateinfo.Name = "txt_updateinfo";
            this.txt_updateinfo.ReadOnly = true;
            this.txt_updateinfo.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txt_updateinfo.Size = new System.Drawing.Size(274, 303);
            this.txt_updateinfo.TabIndex = 1;
            // 
            // btn_uartClear
            // 
            this.btn_uartClear.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.btn_uartClear.Location = new System.Drawing.Point(7, 328);
            this.btn_uartClear.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.btn_uartClear.Name = "btn_uartClear";
            this.btn_uartClear.Size = new System.Drawing.Size(274, 32);
            this.btn_uartClear.TabIndex = 3;
            this.btn_uartClear.Text = "清空显示框";
            this.btn_uartClear.UseVisualStyleBackColor = true;
            this.btn_uartClear.Click += new System.EventHandler(this.btn_uartClear_Click);
            // 
            // grp_updateauto
            // 
            this.grp_updateauto.Controls.Add(this.btn_autoUpdate);
            this.grp_updateauto.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.grp_updateauto.Location = new System.Drawing.Point(6, 22);
            this.grp_updateauto.Name = "grp_updateauto";
            this.grp_updateauto.Size = new System.Drawing.Size(260, 222);
            this.grp_updateauto.TabIndex = 0;
            this.grp_updateauto.TabStop = false;
            this.grp_updateauto.Text = "自动更新";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.prg_update);
            this.groupBox1.Controls.Add(this.lbl_prgstate);
            this.groupBox1.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.groupBox1.Location = new System.Drawing.Point(6, 250);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(260, 112);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "自动更新";
            // 
            // btn_autoUpdate
            // 
            this.btn_autoUpdate.Enabled = false;
            this.btn_autoUpdate.Font = new System.Drawing.Font("微软雅黑", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.btn_autoUpdate.Location = new System.Drawing.Point(61, 78);
            this.btn_autoUpdate.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.btn_autoUpdate.Name = "btn_autoUpdate";
            this.btn_autoUpdate.Size = new System.Drawing.Size(136, 57);
            this.btn_autoUpdate.TabIndex = 1;
            this.btn_autoUpdate.Text = "一键自动更新";
            this.btn_autoUpdate.UseVisualStyleBackColor = true;
            this.btn_autoUpdate.Click += new System.EventHandler(this.btn_autoUpdate_Click);
            // 
            // lbl_prgstate
            // 
            this.lbl_prgstate.AutoSize = true;
            this.lbl_prgstate.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.lbl_prgstate.Location = new System.Drawing.Point(59, 19);
            this.lbl_prgstate.Name = "lbl_prgstate";
            this.lbl_prgstate.Size = new System.Drawing.Size(138, 22);
            this.lbl_prgstate.TabIndex = 6;
            this.lbl_prgstate.Text = "更新状态进度条：";
            // 
            // prg_update
            // 
            this.prg_update.Location = new System.Drawing.Point(7, 60);
            this.prg_update.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.prg_update.Name = "prg_update";
            this.prg_update.Size = new System.Drawing.Size(246, 38);
            this.prg_update.TabIndex = 7;
            // 
            // frm_update
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 17F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(964, 486);
            this.ControlBox = false;
            this.Controls.Add(this.grp_updateinfo);
            this.Controls.Add(this.grp_update);
            this.Controls.Add(this.grp_updatestyle);
            this.Controls.Add(this.grp_openfile);
            this.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "frm_update";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "软件更新";
            this.Load += new System.EventHandler(this.frm_update_Load);
            this.grp_openfile.ResumeLayout(false);
            this.grp_filechoose.ResumeLayout(false);
            this.grp_filechoose.PerformLayout();
            this.grp_fileshow.ResumeLayout(false);
            this.grp_fileshow.PerformLayout();
            this.grp_updatestyle.ResumeLayout(false);
            this.pnl_updatestyle.ResumeLayout(false);
            this.pnl_updatestyle.PerformLayout();
            this.pnl_uart.ResumeLayout(false);
            this.grp_update.ResumeLayout(false);
            this.grp_updateinfo.ResumeLayout(false);
            this.grp_updateinfo.PerformLayout();
            this.grp_updateauto.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox grp_openfile;
        private System.Windows.Forms.GroupBox grp_filechoose;
        private System.Windows.Forms.Label lbl_filename;
        private System.Windows.Forms.Button btn_fileOpen;
        private System.Windows.Forms.GroupBox grp_fileshow;
        private System.Windows.Forms.ListBox lst_codeshow;
        private System.Windows.Forms.Label lbl_codeshow;
        private System.Windows.Forms.GroupBox grp_updatestyle;
        private System.Windows.Forms.RadioButton rdo_uart;
        private System.Windows.Forms.Panel pnl_updatestyle;
        private System.Windows.Forms.RadioButton rdo_remote;
        private System.Windows.Forms.Panel pnl_uart;
        private System.Windows.Forms.Button btn_uartCheck;
        private System.Windows.Forms.Label lbl_uartstate;
        private System.Windows.Forms.GroupBox grp_update;
        private System.Windows.Forms.GroupBox grp_updateinfo;
        private System.Windows.Forms.Button btn_uartClear;
        private System.Windows.Forms.TextBox txt_updateinfo;
        private System.Windows.Forms.GroupBox grp_updateauto;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ProgressBar prg_update;
        private System.Windows.Forms.Label lbl_prgstate;
        private System.Windows.Forms.Button btn_autoUpdate;
    }
}