namespace RemoteUpdate
{
    partial class frm_main
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.文件ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aaToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tsm_openhex = new System.Windows.Forms.ToolStripMenuItem();
            this.tsm_opens19 = new System.Windows.Forms.ToolStripMenuItem();
            this.bbToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.应用ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tsm_uart = new System.Windows.Forms.ToolStripMenuItem();
            this.tsm_nbiot = new System.Windows.Forms.ToolStripMenuItem();
            this.帮助ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.使用说明ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.程序说明ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.版本ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.fileSystemWatcher1 = new System.IO.FileSystemWatcher();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.lbl_status = new System.Windows.Forms.ToolStripStatusLabel();
            this.lbl_protocal = new System.Windows.Forms.ToolStripStatusLabel();
            this.lbl_protocalinfo = new System.Windows.Forms.ToolStripStatusLabel();
            this.lbl_time = new System.Windows.Forms.ToolStripStatusLabel();
            this.tmr_cal = new System.Windows.Forms.Timer(this.components);
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.fileSystemWatcher1)).BeginInit();
            this.statusStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.BackColor = System.Drawing.SystemColors.GradientInactiveCaption;
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.文件ToolStripMenuItem,
            this.应用ToolStripMenuItem,
            this.帮助ToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Padding = new System.Windows.Forms.Padding(8, 3, 0, 3);
            this.menuStrip1.Size = new System.Drawing.Size(1184, 30);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // 文件ToolStripMenuItem
            // 
            this.文件ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.aaToolStripMenuItem,
            this.bbToolStripMenuItem});
            this.文件ToolStripMenuItem.Font = new System.Drawing.Font("Microsoft YaHei UI", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.文件ToolStripMenuItem.Name = "文件ToolStripMenuItem";
            this.文件ToolStripMenuItem.Size = new System.Drawing.Size(66, 24);
            this.文件ToolStripMenuItem.Text = "文件(&F)";
            // 
            // aaToolStripMenuItem
            // 
            this.aaToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsm_openhex,
            this.tsm_opens19});
            this.aaToolStripMenuItem.Name = "aaToolStripMenuItem";
            this.aaToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
            this.aaToolStripMenuItem.Size = new System.Drawing.Size(180, 24);
            this.aaToolStripMenuItem.Text = "打开(&O)";
            this.aaToolStripMenuItem.Click += new System.EventHandler(this.aaToolStripMenuItem_Click);
            // 
            // tsm_openhex
            // 
            this.tsm_openhex.Name = "tsm_openhex";
            this.tsm_openhex.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift) 
            | System.Windows.Forms.Keys.O)));
            this.tsm_openhex.Size = new System.Drawing.Size(200, 24);
            this.tsm_openhex.Text = "HEX";
            this.tsm_openhex.Click += new System.EventHandler(this.tsm_openhex_Click);
            // 
            // tsm_opens19
            // 
            this.tsm_opens19.Name = "tsm_opens19";
            this.tsm_opens19.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Alt) 
            | System.Windows.Forms.Keys.O)));
            this.tsm_opens19.Size = new System.Drawing.Size(200, 24);
            this.tsm_opens19.Text = "S19";
            // 
            // bbToolStripMenuItem
            // 
            this.bbToolStripMenuItem.Name = "bbToolStripMenuItem";
            this.bbToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.E)));
            this.bbToolStripMenuItem.Size = new System.Drawing.Size(180, 24);
            this.bbToolStripMenuItem.Text = "退出(&E)";
            // 
            // 应用ToolStripMenuItem
            // 
            this.应用ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsm_uart,
            this.tsm_nbiot});
            this.应用ToolStripMenuItem.Font = new System.Drawing.Font("Microsoft YaHei UI", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.应用ToolStripMenuItem.Name = "应用ToolStripMenuItem";
            this.应用ToolStripMenuItem.Size = new System.Drawing.Size(68, 24);
            this.应用ToolStripMenuItem.Text = "协议(&P)";
            // 
            // tsm_uart
            // 
            this.tsm_uart.Name = "tsm_uart";
            this.tsm_uart.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.U)));
            this.tsm_uart.Size = new System.Drawing.Size(199, 24);
            this.tsm_uart.Text = "串口(&U)";
            this.tsm_uart.Click += new System.EventHandler(this.tsm_uart_Click);
            // 
            // tsm_nbiot
            // 
            this.tsm_nbiot.Name = "tsm_nbiot";
            this.tsm_nbiot.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.N)));
            this.tsm_nbiot.Size = new System.Drawing.Size(199, 24);
            this.tsm_nbiot.Text = "NB-IoT(&N)";
            // 
            // 帮助ToolStripMenuItem
            // 
            this.帮助ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.使用说明ToolStripMenuItem,
            this.程序说明ToolStripMenuItem,
            this.版本ToolStripMenuItem});
            this.帮助ToolStripMenuItem.Font = new System.Drawing.Font("Microsoft YaHei UI", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.帮助ToolStripMenuItem.Name = "帮助ToolStripMenuItem";
            this.帮助ToolStripMenuItem.Size = new System.Drawing.Size(70, 24);
            this.帮助ToolStripMenuItem.Text = "帮助(&H)";
            // 
            // 使用说明ToolStripMenuItem
            // 
            this.使用说明ToolStripMenuItem.Name = "使用说明ToolStripMenuItem";
            this.使用说明ToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Y)));
            this.使用说明ToolStripMenuItem.Size = new System.Drawing.Size(214, 24);
            this.使用说明ToolStripMenuItem.Text = "使用说明(&Y)";
            // 
            // 程序说明ToolStripMenuItem
            // 
            this.程序说明ToolStripMenuItem.Name = "程序说明ToolStripMenuItem";
            this.程序说明ToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.M)));
            this.程序说明ToolStripMenuItem.Size = new System.Drawing.Size(214, 24);
            this.程序说明ToolStripMenuItem.Text = "程序说明(&M)";
            // 
            // 版本ToolStripMenuItem
            // 
            this.版本ToolStripMenuItem.Name = "版本ToolStripMenuItem";
            this.版本ToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.B)));
            this.版本ToolStripMenuItem.Size = new System.Drawing.Size(214, 24);
            this.版本ToolStripMenuItem.Text = "版本(&B)";
            // 
            // fileSystemWatcher1
            // 
            this.fileSystemWatcher1.EnableRaisingEvents = true;
            this.fileSystemWatcher1.SynchronizingObject = this;
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lbl_status,
            this.lbl_protocal,
            this.lbl_protocalinfo,
            this.lbl_time});
            this.statusStrip1.Location = new System.Drawing.Point(0, 679);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Padding = new System.Windows.Forms.Padding(1, 0, 15, 0);
            this.statusStrip1.Size = new System.Drawing.Size(1184, 22);
            this.statusStrip1.TabIndex = 2;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // lbl_status
            // 
            this.lbl_status.AutoSize = false;
            this.lbl_status.Name = "lbl_status";
            this.lbl_status.Size = new System.Drawing.Size(550, 17);
            this.lbl_status.Text = "当前状态：无操作";
            this.lbl_status.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // lbl_protocal
            // 
            this.lbl_protocal.AutoSize = false;
            this.lbl_protocal.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.lbl_protocal.Name = "lbl_protocal";
            this.lbl_protocal.Size = new System.Drawing.Size(120, 17);
            this.lbl_protocal.Text = "协议：未选择";
            this.lbl_protocal.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // lbl_protocalinfo
            // 
            this.lbl_protocalinfo.AutoSize = false;
            this.lbl_protocalinfo.Name = "lbl_protocalinfo";
            this.lbl_protocalinfo.Size = new System.Drawing.Size(320, 17);
            this.lbl_protocalinfo.Text = "协议信息：暂无";
            this.lbl_protocalinfo.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // lbl_time
            // 
            this.lbl_time.AutoSize = false;
            this.lbl_time.Font = new System.Drawing.Font("Microsoft YaHei UI", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.lbl_time.ForeColor = System.Drawing.SystemColors.HotTrack;
            this.lbl_time.Name = "lbl_time";
            this.lbl_time.Size = new System.Drawing.Size(180, 17);
            this.lbl_time.Text = "当前时间";
            this.lbl_time.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.lbl_time.TextDirection = System.Windows.Forms.ToolStripTextDirection.Horizontal;
            // 
            // tmr_cal
            // 
            this.tmr_cal.Enabled = true;
            this.tmr_cal.Interval = 1000;
            this.tmr_cal.Tick += new System.EventHandler(this.tmr_tick_Tick);
            // 
            // frm_main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.ClientSize = new System.Drawing.Size(1184, 701);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.menuStrip1);
            this.Font = new System.Drawing.Font("微软雅黑", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ForeColor = System.Drawing.Color.Black;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.IsMdiContainer = true;
            this.MainMenuStrip = this.menuStrip1;
            this.Margin = new System.Windows.Forms.Padding(5, 4, 5, 4);
            this.MaximizeBox = false;
            this.Name = "frm_main";
            this.Text = "MCU软件远程更新上位机-180410";
            this.Load += new System.EventHandler(this.frm_main_Load);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.fileSystemWatcher1)).EndInit();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem 文件ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aaToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem bbToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 应用ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem tsm_uart;
        private System.Windows.Forms.ToolStripMenuItem tsm_nbiot;
        private System.Windows.Forms.ToolStripMenuItem tsm_openhex;
        private System.Windows.Forms.ToolStripMenuItem tsm_opens19;
        private System.Windows.Forms.ToolStripMenuItem 帮助ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 使用说明ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 程序说明ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 版本ToolStripMenuItem;
        private System.IO.FileSystemWatcher fileSystemWatcher1;
        public System.Windows.Forms.StatusStrip statusStrip1;
        public System.Windows.Forms.ToolStripStatusLabel lbl_status;
        public System.Windows.Forms.ToolStripStatusLabel lbl_protocal;
        public System.Windows.Forms.ToolStripStatusLabel lbl_protocalinfo;
        public System.Windows.Forms.ToolStripStatusLabel lbl_time;
        private System.Windows.Forms.Timer tmr_cal;

    }
}

