namespace RemoteUpdate
{
    partial class frm_uart
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
            this.grp_com = new System.Windows.Forms.GroupBox();
            this.btn_uartcheck = new System.Windows.Forms.Button();
            this.lbl_uart_state = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.btn_uartclear = new System.Windows.Forms.Button();
            this.txt_uartinfo = new System.Windows.Forms.TextBox();
            this.grp_uartoperate = new System.Windows.Forms.GroupBox();
            this.prg_uartburn = new System.Windows.Forms.ProgressBar();
            this.btn_autoUpdate = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.grp_com.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.grp_uartoperate.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.SuspendLayout();
            // 
            // grp_com
            // 
            this.grp_com.Controls.Add(this.btn_uartcheck);
            this.grp_com.Controls.Add(this.lbl_uart_state);
            this.grp_com.Font = new System.Drawing.Font("微软雅黑", 10.5F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.grp_com.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.grp_com.Location = new System.Drawing.Point(16, 3);
            this.grp_com.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.grp_com.Name = "grp_com";
            this.grp_com.Padding = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.grp_com.Size = new System.Drawing.Size(1147, 108);
            this.grp_com.TabIndex = 0;
            this.grp_com.TabStop = false;
            this.grp_com.Text = "终端连接状态";
            // 
            // btn_uartcheck
            // 
            this.btn_uartcheck.Font = new System.Drawing.Font("微软雅黑", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.btn_uartcheck.Location = new System.Drawing.Point(1059, 18);
            this.btn_uartcheck.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.btn_uartcheck.Name = "btn_uartcheck";
            this.btn_uartcheck.Size = new System.Drawing.Size(80, 80);
            this.btn_uartcheck.TabIndex = 1;
            this.btn_uartcheck.Text = "重新连接";
            this.btn_uartcheck.UseVisualStyleBackColor = true;
            this.btn_uartcheck.Click += new System.EventHandler(this.btn_uartcheck_Click);
            // 
            // lbl_uart_state
            // 
            this.lbl_uart_state.AutoSize = true;
            this.lbl_uart_state.Font = new System.Drawing.Font("微软雅黑", 21.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.lbl_uart_state.Location = new System.Drawing.Point(413, 37);
            this.lbl_uart_state.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.lbl_uart_state.Name = "lbl_uart_state";
            this.lbl_uart_state.Size = new System.Drawing.Size(220, 39);
            this.lbl_uart_state.TabIndex = 0;
            this.lbl_uart_state.Text = "当前无可用串口";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.btn_uartclear);
            this.groupBox2.Controls.Add(this.txt_uartinfo);
            this.groupBox2.Font = new System.Drawing.Font("微软雅黑", 10.5F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.groupBox2.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.groupBox2.Location = new System.Drawing.Point(796, 110);
            this.groupBox2.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Padding = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.groupBox2.Size = new System.Drawing.Size(367, 527);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "终端执行信息";
            // 
            // btn_uartclear
            // 
            this.btn_uartclear.Font = new System.Drawing.Font("微软雅黑", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.btn_uartclear.Location = new System.Drawing.Point(19, 472);
            this.btn_uartclear.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.btn_uartclear.Name = "btn_uartclear";
            this.btn_uartclear.Size = new System.Drawing.Size(340, 45);
            this.btn_uartclear.TabIndex = 2;
            this.btn_uartclear.Text = "清空显示框";
            this.btn_uartclear.UseVisualStyleBackColor = true;
            this.btn_uartclear.Click += new System.EventHandler(this.btn_uartclear_Click);
            // 
            // txt_uartinfo
            // 
            this.txt_uartinfo.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.txt_uartinfo.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.txt_uartinfo.Location = new System.Drawing.Point(19, 27);
            this.txt_uartinfo.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.txt_uartinfo.Multiline = true;
            this.txt_uartinfo.Name = "txt_uartinfo";
            this.txt_uartinfo.ReadOnly = true;
            this.txt_uartinfo.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txt_uartinfo.Size = new System.Drawing.Size(339, 432);
            this.txt_uartinfo.TabIndex = 0;
            // 
            // grp_uartoperate
            // 
            this.grp_uartoperate.Controls.Add(this.groupBox4);
            this.grp_uartoperate.Controls.Add(this.groupBox3);
            this.grp_uartoperate.Controls.Add(this.groupBox1);
            this.grp_uartoperate.Font = new System.Drawing.Font("微软雅黑", 10.5F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.grp_uartoperate.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.grp_uartoperate.Location = new System.Drawing.Point(16, 110);
            this.grp_uartoperate.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.grp_uartoperate.Name = "grp_uartoperate";
            this.grp_uartoperate.Padding = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.grp_uartoperate.Size = new System.Drawing.Size(772, 527);
            this.grp_uartoperate.TabIndex = 2;
            this.grp_uartoperate.TabStop = false;
            this.grp_uartoperate.Text = "串口操作";
            // 
            // prg_uartburn
            // 
            this.prg_uartburn.Location = new System.Drawing.Point(236, 27);
            this.prg_uartburn.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.prg_uartburn.Name = "prg_uartburn";
            this.prg_uartburn.Size = new System.Drawing.Size(231, 38);
            this.prg_uartburn.TabIndex = 2;
            // 
            // btn_autoUpdate
            // 
            this.btn_autoUpdate.Enabled = false;
            this.btn_autoUpdate.Font = new System.Drawing.Font("微软雅黑", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.btn_autoUpdate.Location = new System.Drawing.Point(31, 46);
            this.btn_autoUpdate.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.btn_autoUpdate.Name = "btn_autoUpdate";
            this.btn_autoUpdate.Size = new System.Drawing.Size(136, 57);
            this.btn_autoUpdate.TabIndex = 0;
            this.btn_autoUpdate.Text = "一键自动更新";
            this.btn_autoUpdate.UseVisualStyleBackColor = true;
            this.btn_autoUpdate.Click += new System.EventHandler(this.btn_autoUpdate_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.btn_autoUpdate);
            this.groupBox1.Font = new System.Drawing.Font("微软雅黑", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.groupBox1.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.groupBox1.Location = new System.Drawing.Point(7, 27);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(200, 151);
            this.groupBox1.TabIndex = 3;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "自动更新";
            // 
            // groupBox3
            // 
            this.groupBox3.Font = new System.Drawing.Font("微软雅黑", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.groupBox3.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.groupBox3.Location = new System.Drawing.Point(7, 184);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(200, 335);
            this.groupBox3.TabIndex = 4;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "手动更新";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label1.Location = new System.Drawing.Point(71, 32);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(138, 22);
            this.label1.TabIndex = 5;
            this.label1.Text = "更新状态进度条：";
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.prg_uartburn);
            this.groupBox4.Controls.Add(this.label1);
            this.groupBox4.Font = new System.Drawing.Font("微软雅黑", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.groupBox4.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.groupBox4.Location = new System.Drawing.Point(213, 440);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(552, 77);
            this.groupBox4.TabIndex = 6;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "更新状态条";
            // 
            // frm_uart
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1179, 643);
            this.Controls.Add(this.grp_uartoperate);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.grp_com);
            this.Font = new System.Drawing.Font("微软雅黑", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.Name = "frm_uart";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "frmUart";
            this.Load += new System.EventHandler(this.frm_uart_Load);
            this.grp_com.ResumeLayout(false);
            this.grp_com.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.grp_uartoperate.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox grp_com;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button btn_uartclear;
        private System.Windows.Forms.TextBox txt_uartinfo;
        private System.Windows.Forms.GroupBox grp_uartoperate;
        private System.Windows.Forms.Label lbl_uart_state;
        private System.Windows.Forms.Button btn_uartcheck;
        private System.Windows.Forms.Button btn_autoUpdate;
        private System.Windows.Forms.ProgressBar prg_uartburn;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox4;

    }
}