namespace Csharp_Test_Client
{
    partial class MainForm
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            groupBox1 = new GroupBox();
            label2 = new Label();
            label1 = new Label();
            textBox2 = new TextBox();
            textBox1 = new TextBox();
            BtnConnect = new Button();
            BtnDisconnect = new Button();
            textBox3 = new TextBox();
            button3 = new Button();
            button4 = new Button();
            textBox4 = new TextBox();
            textBox5 = new TextBox();
            label3 = new Label();
            label4 = new Label();
            groupBox1.SuspendLayout();
            SuspendLayout();
            // 
            // groupBox1
            // 
            groupBox1.Controls.Add(label2);
            groupBox1.Controls.Add(label1);
            groupBox1.Controls.Add(textBox2);
            groupBox1.Controls.Add(textBox1);
            groupBox1.Font = new Font("Malgun Gothic", 10F, FontStyle.Bold);
            groupBox1.Location = new Point(12, 12);
            groupBox1.Name = "groupBox1";
            groupBox1.Size = new Size(457, 83);
            groupBox1.TabIndex = 0;
            groupBox1.TabStop = false;
            groupBox1.Text = "Socekt 더미 클라이언트 설정";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(193, 38);
            label2.Name = "label2";
            label2.Size = new Size(69, 19);
            label2.TabIndex = 3;
            label2.Text = "포트번호:";
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new Point(6, 35);
            label1.Name = "label1";
            label1.Size = new Size(69, 19);
            label1.TabIndex = 2;
            label1.Text = "서버주소:";
            // 
            // textBox2
            // 
            textBox2.Location = new Point(268, 32);
            textBox2.Name = "textBox2";
            textBox2.Size = new Size(100, 25);
            textBox2.TabIndex = 1;
            // 
            // textBox1
            // 
            textBox1.Location = new Point(77, 32);
            textBox1.Name = "textBox1";
            textBox1.Size = new Size(100, 25);
            textBox1.TabIndex = 0;
            // 
            // BtnConnect
            // 
            BtnConnect.Location = new Point(475, 12);
            BtnConnect.Name = "BtnConnect";
            BtnConnect.Size = new Size(84, 38);
            BtnConnect.TabIndex = 1;
            BtnConnect.Text = "접속 하기";
            BtnConnect.UseVisualStyleBackColor = true;
            // 
            // BtnDisconnect
            // 
            BtnDisconnect.Location = new Point(475, 56);
            BtnDisconnect.Name = "BtnDisconnect";
            BtnDisconnect.Size = new Size(84, 39);
            BtnDisconnect.TabIndex = 2;
            BtnDisconnect.Text = "접속 끊기";
            BtnDisconnect.UseVisualStyleBackColor = true;
            // 
            // textBox3
            // 
            textBox3.Location = new Point(12, 114);
            textBox3.Name = "textBox3";
            textBox3.Size = new Size(457, 23);
            textBox3.TabIndex = 3;
            // 
            // button3
            // 
            button3.Location = new Point(480, 108);
            button3.Name = "button3";
            button3.Size = new Size(79, 29);
            button3.TabIndex = 4;
            button3.Text = "에코 보내기";
            button3.UseVisualStyleBackColor = true;
            // 
            // button4
            // 
            button4.Location = new Point(480, 163);
            button4.Name = "button4";
            button4.Size = new Size(75, 23);
            button4.TabIndex = 5;
            button4.Text = "Login";
            button4.UseVisualStyleBackColor = true;
            // 
            // textBox4
            // 
            textBox4.Location = new Point(89, 160);
            textBox4.Name = "textBox4";
            textBox4.Size = new Size(100, 23);
            textBox4.TabIndex = 6;
            // 
            // textBox5
            // 
            textBox5.Location = new Point(291, 159);
            textBox5.Name = "textBox5";
            textBox5.Size = new Size(100, 23);
            textBox5.TabIndex = 7;
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new Point(18, 163);
            label3.Name = "label3";
            label3.Size = new Size(39, 15);
            label3.TabIndex = 8;
            label3.Text = "label3";
            // 
            // label4
            // 
            label4.AutoSize = true;
            label4.Location = new Point(223, 167);
            label4.Name = "label4";
            label4.Size = new Size(39, 15);
            label4.TabIndex = 9;
            label4.Text = "label4";
           
            // 
            // MainForm
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(571, 450);
            Controls.Add(label4);
            Controls.Add(label3);
            Controls.Add(textBox5);
            Controls.Add(textBox4);
            Controls.Add(button4);
            Controls.Add(button3);
            Controls.Add(textBox3);
            Controls.Add(BtnDisconnect);
            Controls.Add(BtnConnect);
            Controls.Add(groupBox1);
            Name = "MainForm";
            Text = "Form1";
            groupBox1.ResumeLayout(false);
            groupBox1.PerformLayout();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private GroupBox groupBox1;
        private TextBox textBox2;
        private TextBox textBox1;
        private Button BtnConnect;
        private Button BtnDisconnect;
        private Label label1;
        private Label label2;
        private TextBox textBox3;
        private Button button3;
        private Button button4;
        private TextBox textBox4;
        private TextBox textBox5;
        private Label label3;
        private Label label4;
    }
}
