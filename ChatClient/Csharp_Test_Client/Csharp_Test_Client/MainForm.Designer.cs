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
            checkBoxLocalHostIP = new CheckBox();
            label2 = new Label();
            label1 = new Label();
            textBoxPort = new TextBox();
            textBoxIP = new TextBox();
            BtnConnect = new Button();
            BtnDisconnect = new Button();
            textSendText = new TextBox();
            button_Echo = new Button();
            button4 = new Button();
            textBoxUserID = new TextBox();
            textBoxUserPW = new TextBox();
            label3 = new Label();
            label4 = new Label();
            labelStatus = new Label();
            groupBox2 = new GroupBox();
            Room = new GroupBox();
            label5 = new Label();
            textBox1 = new TextBox();
            btn_RoomEnter = new Button();
            btn_RoomLeave = new Button();
            listBoxLog = new ListBox();
            groupBox1.SuspendLayout();
            groupBox2.SuspendLayout();
            Room.SuspendLayout();
            SuspendLayout();
            // 
            // groupBox1
            // 
            groupBox1.Controls.Add(checkBoxLocalHostIP);
            groupBox1.Controls.Add(label2);
            groupBox1.Controls.Add(label1);
            groupBox1.Controls.Add(textBoxPort);
            groupBox1.Controls.Add(textBoxIP);
            groupBox1.Font = new Font("Malgun Gothic", 10F, FontStyle.Bold);
            groupBox1.Location = new Point(12, 12);
            groupBox1.Name = "groupBox1";
            groupBox1.Size = new Size(457, 83);
            groupBox1.TabIndex = 0;
            groupBox1.TabStop = false;
            groupBox1.Text = "Socekt 더미 클라이언트 설정";
            // 
            // checkBoxLocalHostIP
            // 
            checkBoxLocalHostIP.AutoSize = true;
            checkBoxLocalHostIP.Checked = true;
            checkBoxLocalHostIP.CheckState = CheckState.Checked;
            checkBoxLocalHostIP.Location = new Point(327, 54);
            checkBoxLocalHostIP.Name = "checkBoxLocalHostIP";
            checkBoxLocalHostIP.Size = new Size(121, 23);
            checkBoxLocalHostIP.TabIndex = 4;
            checkBoxLocalHostIP.Text = "localhost 사용";
            checkBoxLocalHostIP.UseVisualStyleBackColor = true;
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(181, 35);
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
            // textBoxPort
            // 
            textBoxPort.Location = new Point(256, 29);
            textBoxPort.Name = "textBoxPort";
            textBoxPort.Size = new Size(100, 25);
            textBoxPort.TabIndex = 1;
            // 
            // textBoxIP
            // 
            textBoxIP.Location = new Point(77, 32);
            textBoxIP.Name = "textBoxIP";
            textBoxIP.Size = new Size(100, 25);
            textBoxIP.TabIndex = 0;
            // 
            // BtnConnect
            // 
            BtnConnect.Location = new Point(475, 12);
            BtnConnect.Name = "BtnConnect";
            BtnConnect.Size = new Size(84, 38);
            BtnConnect.TabIndex = 1;
            BtnConnect.Text = "접속 하기";
            BtnConnect.UseVisualStyleBackColor = true;
            BtnConnect.Click += BtnConnect_Click;
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
            // textSendText
            // 
            textSendText.Location = new Point(12, 114);
            textSendText.Name = "textSendText";
            textSendText.Size = new Size(457, 23);
            textSendText.TabIndex = 3;
            // 
            // button_Echo
            // 
            button_Echo.Location = new Point(480, 108);
            button_Echo.Name = "button_Echo";
            button_Echo.Size = new Size(79, 29);
            button_Echo.TabIndex = 4;
            button_Echo.Text = "에코 보내기";
            button_Echo.UseVisualStyleBackColor = true;
            button_Echo.Click += button_Echo_Click;
            // 
            // button4
            // 
            button4.Location = new Point(337, 38);
            button4.Name = "button4";
            button4.Size = new Size(108, 28);
            button4.TabIndex = 5;
            button4.Text = "Login";
            button4.UseVisualStyleBackColor = true;
            // 
            // textBoxUserID
            // 
            textBoxUserID.Location = new Point(59, 42);
            textBoxUserID.Name = "textBoxUserID";
            textBoxUserID.Size = new Size(100, 23);
            textBoxUserID.TabIndex = 6;
            // 
            // textBoxUserPW
            // 
            textBoxUserPW.Location = new Point(228, 42);
            textBoxUserPW.Name = "textBoxUserPW";
            textBoxUserPW.Size = new Size(100, 23);
            textBoxUserPW.TabIndex = 7;
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new Point(4, 45);
            label3.Name = "label3";
            label3.Size = new Size(49, 15);
            label3.TabIndex = 8;
            label3.Text = "UserID :";
            // 
            // label4
            // 
            label4.AutoSize = true;
            label4.Location = new Point(165, 48);
            label4.Name = "label4";
            label4.Size = new Size(57, 15);
            label4.TabIndex = 9;
            label4.Text = "PassWD :";
            // 
            // labelStatus
            // 
            labelStatus.AutoSize = true;
            labelStatus.Font = new Font("Malgun Gothic", 10F);
            labelStatus.Location = new Point(22, 758);
            labelStatus.Name = "labelStatus";
            labelStatus.Size = new Size(134, 19);
            labelStatus.TabIndex = 10;
            labelStatus.Text = "서버 접속 상태 : ???";
            // 
            // groupBox2
            // 
            groupBox2.Controls.Add(button4);
            groupBox2.Controls.Add(textBoxUserPW);
            groupBox2.Controls.Add(label3);
            groupBox2.Controls.Add(label4);
            groupBox2.Controls.Add(textBoxUserID);
            groupBox2.Location = new Point(18, 156);
            groupBox2.Name = "groupBox2";
            groupBox2.Size = new Size(451, 102);
            groupBox2.TabIndex = 11;
            groupBox2.TabStop = false;
            groupBox2.Text = "로그인";
            // 
            // Room
            // 
            Room.Controls.Add(btn_RoomLeave);
            Room.Controls.Add(btn_RoomEnter);
            Room.Controls.Add(label5);
            Room.Controls.Add(textBox1);
            Room.Location = new Point(22, 278);
            Room.Name = "Room";
            Room.Size = new Size(496, 228);
            Room.TabIndex = 12;
            Room.TabStop = false;
            Room.Text = "Room";
            // 
            // label5
            // 
            label5.AutoSize = true;
            label5.Location = new Point(15, 35);
            label5.Name = "label5";
            label5.Size = new Size(98, 15);
            label5.TabIndex = 1;
            label5.Text = "Room Number : ";
            // 
            // textBox1
            // 
            textBox1.Location = new Point(119, 32);
            textBox1.Name = "textBox1";
            textBox1.Size = new Size(64, 23);
            textBox1.TabIndex = 0;
            // 
            // btn_RoomEnter
            // 
            btn_RoomEnter.Location = new Point(201, 31);
            btn_RoomEnter.Name = "btn_RoomEnter";
            btn_RoomEnter.Size = new Size(75, 23);
            btn_RoomEnter.TabIndex = 2;
            btn_RoomEnter.Text = "Enter";
            btn_RoomEnter.UseVisualStyleBackColor = true;
            // 
            // btn_RoomLeave
            // 
            btn_RoomLeave.Location = new Point(282, 31);
            btn_RoomLeave.Name = "btn_RoomLeave";
            btn_RoomLeave.Size = new Size(75, 23);
            btn_RoomLeave.TabIndex = 3;
            btn_RoomLeave.Text = "Leave";
            btn_RoomLeave.UseVisualStyleBackColor = true;
            // 
            // listBoxLog
            // 
            listBoxLog.FormattingEnabled = true;
            listBoxLog.ItemHeight = 15;
            listBoxLog.Location = new Point(18, 544);
            listBoxLog.Name = "listBoxLog";
            listBoxLog.Size = new Size(468, 199);
            listBoxLog.TabIndex = 13;
            // 
            // MainForm
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(571, 798);
            Controls.Add(listBoxLog);
            Controls.Add(Room);
            Controls.Add(groupBox2);
            Controls.Add(labelStatus);
            Controls.Add(button_Echo);
            Controls.Add(textSendText);
            Controls.Add(BtnDisconnect);
            Controls.Add(BtnConnect);
            Controls.Add(groupBox1);
            Name = "MainForm";
            Text = "Form1";
            Load += MainForm_Load;
            groupBox1.ResumeLayout(false);
            groupBox1.PerformLayout();
            groupBox2.ResumeLayout(false);
            groupBox2.PerformLayout();
            Room.ResumeLayout(false);
            Room.PerformLayout();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private GroupBox groupBox1;
        private TextBox textBoxPort;
        private TextBox textBoxIP;
        private Button BtnConnect;
        private Button BtnDisconnect;
        private Label label1;
        private Label label2;
        private TextBox textSendText;
        private Button button_Echo;
        private Button button4;
        private TextBox textBoxUserID;
        private TextBox textBoxUserPW;
        private Label label3;
        private Label label4;
        private CheckBox checkBoxLocalHostIP;
        private Label labelStatus;
        private GroupBox groupBox2;
        private GroupBox Room;
        private Label label5;
        private TextBox textBox1;
        private Button btn_RoomLeave;
        private Button btn_RoomEnter;
        private ListBox listBoxLog;
    }
}
