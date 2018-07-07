namespace RemoteUpdate
{
    partial class frm_main
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
            this.components = new System.ComponentModel.Container();
            this.mns_main = new System.Windows.Forms.MenuStrip();
            this.tsm_update = new System.Windows.Forms.ToolStripMenuItem();
            this.ssr_state = new System.Windows.Forms.StatusStrip();
            this.lbl_mainstatus = new System.Windows.Forms.ToolStripStatusLabel();
            this.lbl_protocal = new System.Windows.Forms.ToolStripStatusLabel();
            this.lbl_protocalinfo = new System.Windows.Forms.ToolStripStatusLabel();
            this.lbl_curtime = new System.Windows.Forms.ToolStripStatusLabel();
            this.tmr_tick = new System.Windows.Forms.Timer(this.components);
            this.mns_main.SuspendLayout();
            this.ssr_state.SuspendLayout();
            this.SuspendLayout();
            // 
            // mns_main
            // 
            this.mns_main.BackColor = System.Drawing.SystemColors.GradientActiveCaption;
            this.mns_main.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsm_update});
            this.mns_main.Location = new System.Drawing.Point(0, 0);
            this.mns_main.Name = "mns_main";
            this.mns_main.Size = new System.Drawing.Size(984, 28);
            this.mns_main.TabIndex = 0;
            this.mns_main.Text = "menuStrip1";
            // 
            // tsm_update
            // 
            this.tsm_update.Font = new System.Drawing.Font("Microsoft YaHei UI", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.tsm_update.Name = "tsm_update";
            this.tsm_update.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.U)));
            this.tsm_update.Size = new System.Drawing.Size(97, 24);
            this.tsm_update.Text = "软件更新(&U)";
            this.tsm_update.Click += new System.EventHandler(this.tsm_update_Click);
            // 
            // ssr_state
            // 
            this.ssr_state.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lbl_mainstatus,
            this.lbl_protocal,
            this.lbl_protocalinfo,
            this.lbl_curtime});
            this.ssr_state.Location = new System.Drawing.Point(0, 557);
            this.ssr_state.Name = "ssr_state";
            this.ssr_state.Size = new System.Drawing.Size(984, 22);
            this.ssr_state.TabIndex = 2;
            this.ssr_state.Text = "statusStrip1";
            // 
            // lbl_mainstatus
            // 
            this.lbl_mainstatus.AutoSize = false;
            this.lbl_mainstatus.BorderSides = ((System.Windows.Forms.ToolStripStatusLabelBorderSides)((((System.Windows.Forms.ToolStripStatusLabelBorderSides.Left | System.Windows.Forms.ToolStripStatusLabelBorderSides.Top) 
            | System.Windows.Forms.ToolStripStatusLabelBorderSides.Right) 
            | System.Windows.Forms.ToolStripStatusLabelBorderSides.Bottom)));
            this.lbl_mainstatus.Name = "lbl_mainstatus";
            this.lbl_mainstatus.Size = new System.Drawing.Size(430, 17);
            this.lbl_mainstatus.Text = "运行状态：无操作";
            this.lbl_mainstatus.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // lbl_protocal
            // 
            this.lbl_protocal.AutoSize = false;
            this.lbl_protocal.BorderSides = ((System.Windows.Forms.ToolStripStatusLabelBorderSides)((((System.Windows.Forms.ToolStripStatusLabelBorderSides.Left | System.Windows.Forms.ToolStripStatusLabelBorderSides.Top) 
            | System.Windows.Forms.ToolStripStatusLabelBorderSides.Right) 
            | System.Windows.Forms.ToolStripStatusLabelBorderSides.Bottom)));
            this.lbl_protocal.Name = "lbl_protocal";
            this.lbl_protocal.Size = new System.Drawing.Size(100, 17);
            this.lbl_protocal.Text = "协议：";
            this.lbl_protocal.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // lbl_protocalinfo
            // 
            this.lbl_protocalinfo.AutoSize = false;
            this.lbl_protocalinfo.BorderSides = ((System.Windows.Forms.ToolStripStatusLabelBorderSides)((((System.Windows.Forms.ToolStripStatusLabelBorderSides.Left | System.Windows.Forms.ToolStripStatusLabelBorderSides.Top) 
            | System.Windows.Forms.ToolStripStatusLabelBorderSides.Right) 
            | System.Windows.Forms.ToolStripStatusLabelBorderSides.Bottom)));
            this.lbl_protocalinfo.Name = "lbl_protocalinfo";
            this.lbl_protocalinfo.Size = new System.Drawing.Size(260, 17);
            this.lbl_protocalinfo.Text = "协议信息：";
            this.lbl_protocalinfo.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // lbl_curtime
            // 
            this.lbl_curtime.AutoSize = false;
            this.lbl_curtime.BorderSides = ((System.Windows.Forms.ToolStripStatusLabelBorderSides)((((System.Windows.Forms.ToolStripStatusLabelBorderSides.Left | System.Windows.Forms.ToolStripStatusLabelBorderSides.Top) 
            | System.Windows.Forms.ToolStripStatusLabelBorderSides.Right) 
            | System.Windows.Forms.ToolStripStatusLabelBorderSides.Bottom)));
            this.lbl_curtime.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.lbl_curtime.Name = "lbl_curtime";
            this.lbl_curtime.Size = new System.Drawing.Size(180, 17);
            this.lbl_curtime.Text = "当前时间：";
            this.lbl_curtime.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // tmr_tick
            // 
            this.tmr_tick.Enabled = true;
            this.tmr_tick.Interval = 1000;
            this.tmr_tick.Tick += new System.EventHandler(this.tmr_tick_Tick);
            // 
            // frm_main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 17F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(984, 579);
            this.Controls.Add(this.ssr_state);
            this.Controls.Add(this.mns_main);
            this.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ForeColor = System.Drawing.SystemColors.WindowText;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.IsMdiContainer = true;
            this.MainMenuStrip = this.mns_main;
            this.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.MaximizeBox = false;
            this.Name = "frm_main";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "金葫芦软件更新上位机-180706";
            this.mns_main.ResumeLayout(false);
            this.mns_main.PerformLayout();
            this.ssr_state.ResumeLayout(false);
            this.ssr_state.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip mns_main;
        private System.Windows.Forms.ToolStripMenuItem tsm_update;
        private System.Windows.Forms.StatusStrip ssr_state;
        private System.Windows.Forms.ToolStripStatusLabel lbl_curtime;
        private System.Windows.Forms.Timer tmr_tick;
        public System.Windows.Forms.ToolStripStatusLabel lbl_mainstatus;
        public System.Windows.Forms.ToolStripStatusLabel lbl_protocal;
        public System.Windows.Forms.ToolStripStatusLabel lbl_protocalinfo;
    }
}