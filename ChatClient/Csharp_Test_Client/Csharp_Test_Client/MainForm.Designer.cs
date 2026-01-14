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
            Btn_Echo = new Button();
            Btn_Login = new Button();
            textBoxUserID = new TextBox();
            textBoxUserPW = new TextBox();
            label3 = new Label();
            label4 = new Label();
            labelStatus = new Label();
            groupBox2 = new GroupBox();
            Room = new GroupBox();
            label7 = new Label();
            listBoxRoomUserList = new ListBox();
            listBoxRoomChatMsg = new ListBox();
            btn_RoomLeave = new Button();
            btn_RoomEnter = new Button();
            label5 = new Label();
            textBoxRoomNumber = new TextBox();
            listBoxLog = new ListBox();
            groupBox3 = new GroupBox();
            BtnMultiLogin = new Button();
            BtnMultiDisConnect = new Button();
            BtnMultiConnect = new Button();
            label6 = new Label();
            textBoxClientNum = new TextBox();
            textBoxRoomSendMsg = new TextBox();
            btnRoomChat = new Button();
            groupBox1.SuspendLayout();
            groupBox2.SuspendLayout();
            Room.SuspendLayout();
            groupBox3.SuspendLayout();
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
            groupBox1.Size = new Size(451, 83);
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
            BtnDisconnect.Click += BtnDisconnect_Click;
            // 
            // textSendText
            // 
            textSendText.Location = new Point(12, 107);
            textSendText.Name = "textSendText";
            textSendText.Size = new Size(457, 23);
            textSendText.TabIndex = 3;
            // 
            // Btn_Echo
            // 
            Btn_Echo.Location = new Point(475, 101);
            Btn_Echo.Name = "Btn_Echo";
            Btn_Echo.Size = new Size(79, 29);
            Btn_Echo.TabIndex = 4;
            Btn_Echo.Text = "에코 보내기";
            Btn_Echo.UseVisualStyleBackColor = true;
            Btn_Echo.Click += button_Echo_Click;
            // 
            // Btn_Login
            // 
            Btn_Login.Location = new Point(337, 38);
            Btn_Login.Name = "Btn_Login";
            Btn_Login.Size = new Size(108, 28);
            Btn_Login.TabIndex = 5;
            Btn_Login.Text = "Login";
            Btn_Login.UseVisualStyleBackColor = true;
            Btn_Login.Click += BtnLogin_Click;
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
            labelStatus.Location = new Point(18, 1033);
            labelStatus.Name = "labelStatus";
            labelStatus.Size = new Size(134, 19);
            labelStatus.TabIndex = 10;
            labelStatus.Text = "서버 접속 상태 : ???";
            // 
            // groupBox2
            // 
            groupBox2.Controls.Add(Btn_Login);
            groupBox2.Controls.Add(textBoxUserPW);
            groupBox2.Controls.Add(label3);
            groupBox2.Controls.Add(label4);
            groupBox2.Controls.Add(textBoxUserID);
            groupBox2.Location = new Point(12, 136);
            groupBox2.Name = "groupBox2";
            groupBox2.Size = new Size(451, 102);
            groupBox2.TabIndex = 11;
            groupBox2.TabStop = false;
            groupBox2.Text = "로그인";
            // 
            // Room
            // 
            Room.Controls.Add(btnRoomChat);
            Room.Controls.Add(textBoxRoomSendMsg);
            Room.Controls.Add(label7);
            Room.Controls.Add(listBoxRoomUserList);
            Room.Controls.Add(listBoxRoomChatMsg);
            Room.Controls.Add(btn_RoomLeave);
            Room.Controls.Add(btn_RoomEnter);
            Room.Controls.Add(label5);
            Room.Controls.Add(textBoxRoomNumber);
            Room.Location = new Point(13, 465);
            Room.Name = "Room";
            Room.Size = new Size(603, 396);
            Room.TabIndex = 12;
            Room.TabStop = false;
            Room.Text = "Room";
            // 
            // label7
            // 
            label7.AutoSize = true;
            label7.Location = new Point(18, 64);
            label7.Name = "label7";
            label7.Size = new Size(59, 15);
            label7.TabIndex = 6;
            label7.Text = "User List :";
            // 
            // listBoxRoomUserList
            // 
            listBoxRoomUserList.FormattingEnabled = true;
            listBoxRoomUserList.ItemHeight = 15;
            listBoxRoomUserList.Location = new Point(18, 82);
            listBoxRoomUserList.Name = "listBoxRoomUserList";
            listBoxRoomUserList.Size = new Size(245, 244);
            listBoxRoomUserList.TabIndex = 5;
            // 
            // listBoxRoomChatMsg
            // 
            listBoxRoomChatMsg.FormattingEnabled = true;
            listBoxRoomChatMsg.ItemHeight = 15;
            listBoxRoomChatMsg.Location = new Point(282, 82);
            listBoxRoomChatMsg.Name = "listBoxRoomChatMsg";
            listBoxRoomChatMsg.Size = new Size(253, 244);
            listBoxRoomChatMsg.TabIndex = 4;
            // 
            // btn_RoomLeave
            // 
            btn_RoomLeave.Location = new Point(282, 31);
            btn_RoomLeave.Name = "btn_RoomLeave";
            btn_RoomLeave.Size = new Size(75, 23);
            btn_RoomLeave.TabIndex = 3;
            btn_RoomLeave.Text = "Leave";
            btn_RoomLeave.UseVisualStyleBackColor = true;
            btn_RoomLeave.Click += btn_RoomLeave_Click;
            // 
            // btn_RoomEnter
            // 
            btn_RoomEnter.Location = new Point(201, 31);
            btn_RoomEnter.Name = "btn_RoomEnter";
            btn_RoomEnter.Size = new Size(75, 23);
            btn_RoomEnter.TabIndex = 2;
            btn_RoomEnter.Text = "Enter";
            btn_RoomEnter.UseVisualStyleBackColor = true;
            btn_RoomEnter.Click += btn_RoomEnter_Click;
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
            // textBoxRoomNumber
            // 
            textBoxRoomNumber.Location = new Point(119, 32);
            textBoxRoomNumber.Name = "textBoxRoomNumber";
            textBoxRoomNumber.Size = new Size(64, 23);
            textBoxRoomNumber.TabIndex = 0;
            textBoxRoomNumber.Text = "0";
            // 
            // listBoxLog
            // 
            listBoxLog.FormattingEnabled = true;
            listBoxLog.ItemHeight = 15;
            listBoxLog.Location = new Point(18, 867);
            listBoxLog.Name = "listBoxLog";
            listBoxLog.Size = new Size(468, 154);
            listBoxLog.TabIndex = 13;
            // 
            // groupBox3
            // 
            groupBox3.Controls.Add(BtnMultiLogin);
            groupBox3.Controls.Add(BtnMultiDisConnect);
            groupBox3.Controls.Add(BtnMultiConnect);
            groupBox3.Controls.Add(label6);
            groupBox3.Controls.Add(textBoxClientNum);
            groupBox3.Location = new Point(8, 244);
            groupBox3.Name = "groupBox3";
            groupBox3.Size = new Size(546, 215);
            groupBox3.TabIndex = 14;
            groupBox3.TabStop = false;
            groupBox3.Text = "테스트";
            // 
            // BtnMultiLogin
            // 
            BtnMultiLogin.Location = new Point(10, 116);
            BtnMultiLogin.Name = "BtnMultiLogin";
            BtnMultiLogin.Size = new Size(233, 36);
            BtnMultiLogin.TabIndex = 4;
            BtnMultiLogin.Text = "모든 테스트 클라이언트 로그인하기";
            BtnMultiLogin.UseVisualStyleBackColor = true;
            // 
            // BtnMultiDisConnect
            // 
            BtnMultiDisConnect.Location = new Point(225, 73);
            BtnMultiDisConnect.Name = "BtnMultiDisConnect";
            BtnMultiDisConnect.Size = new Size(246, 37);
            BtnMultiDisConnect.TabIndex = 3;
            BtnMultiDisConnect.Text = "모든 테스트 클라이언트 접속 종료하기";
            BtnMultiDisConnect.UseVisualStyleBackColor = true;
            // 
            // BtnMultiConnect
            // 
            BtnMultiConnect.Location = new Point(14, 73);
            BtnMultiConnect.Name = "BtnMultiConnect";
            BtnMultiConnect.Size = new Size(205, 37);
            BtnMultiConnect.TabIndex = 2;
            BtnMultiConnect.Text = "모든 테스트 클라이언트 접속하기";
            BtnMultiConnect.UseVisualStyleBackColor = true;
            BtnMultiConnect.Click += BtnMultiConnect_Click;
            // 
            // label6
            // 
            label6.AutoSize = true;
            label6.Location = new Point(9, 36);
            label6.Name = "label6";
            label6.Size = new Size(158, 15);
            label6.TabIndex = 1;
            label6.Text = "테스트 클라이언트 인원 수 :";
            // 
            // textBoxClientNum
            // 
            textBoxClientNum.Location = new Point(170, 28);
            textBoxClientNum.Name = "textBoxClientNum";
            textBoxClientNum.Size = new Size(100, 23);
            textBoxClientNum.TabIndex = 0;
            // 
            // textBoxRoomSendMsg
            // 
            textBoxRoomSendMsg.Location = new Point(18, 342);
            textBoxRoomSendMsg.Name = "textBoxRoomSendMsg";
            textBoxRoomSendMsg.Size = new Size(438, 23);
            textBoxRoomSendMsg.TabIndex = 7;
            textBoxRoomSendMsg.Text = "chat 메세지";
            // 
            // btnRoomChat
            // 
            btnRoomChat.Location = new Point(466, 341);
            btnRoomChat.Name = "btnRoomChat";
            btnRoomChat.Size = new Size(75, 23);
            btnRoomChat.TabIndex = 8;
            btnRoomChat.Text = "Chat";
            btnRoomChat.UseVisualStyleBackColor = true;
            btnRoomChat.Click += btnRoomChat_Click;
            // 
            // MainForm
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(620, 1061);
            Controls.Add(groupBox3);
            Controls.Add(listBoxLog);
            Controls.Add(Room);
            Controls.Add(groupBox2);
            Controls.Add(labelStatus);
            Controls.Add(Btn_Echo);
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
            groupBox3.ResumeLayout(false);
            groupBox3.PerformLayout();
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
        private Button Btn_Echo;
        private Button Btn_Login;
        private TextBox textBoxUserID;
        private TextBox textBoxUserPW;
        private Label label3;
        private Label label4;
        private CheckBox checkBoxLocalHostIP;
        private Label labelStatus;
        private GroupBox groupBox2;
        private GroupBox Room;
        private Label label5;
        private TextBox textBoxRoomNumber;
        private Button btn_RoomLeave;
        private Button btn_RoomEnter;
        private ListBox listBoxLog;
        private GroupBox groupBox3;
        private Label label6;
        private TextBox textBoxClientNum;
        private Button BtnMultiLogin;
        private Button BtnMultiDisConnect;
        private Button BtnMultiConnect;
        private Label label7;
        private ListBox listBoxRoomUserList;
        private ListBox listBoxRoomChatMsg;
        private Button btnRoomChat;
        private TextBox textBoxRoomSendMsg;
    }
}
