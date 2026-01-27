#pragma once

#include "MySQLTaskDefine.h"

#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/exception.h>


class MySQLManager
{

public:

    bool Connect(std::string host, std::string user, std::string pass) {
        try {
            // --- [1단계: 연결하기] ---
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();

            // 주소, 아이디, 비번 (L 안 붙이고 std::string 사용!)
            //std::string host = "tcp://127.0.0.1:3306";
            //std::string user = "root";
            //std::string pass = "1234";

            std::cout << "MySQL DB에 연결을 시도합니다..." << std::endl;
            con = driver->connect(host, user, pass);

            std::cout << "MySQL 연결 성공!" << std::endl;

			return true;
        }
        catch (sql::SQLException& e) {
            // MySQL 관련 에러가 나면 일로 점프해요! (비밀번호 틀림, 테이블 없음 등)
            std::cout << "MySQL 에러 발생!!" << std::endl;
            std::cout << "에러 코드: " << e.getErrorCode() << std::endl;
            std::cout << "에러 내용: " << e.what() << std::endl;
        }
        catch (std::exception& e) {
            // 그 외 일반적인 컴퓨터 에러 처리
            std::cout << "일반 에러 발생: " << e.what() << std::endl;
        }

        // 마지막 정리 (에어백 밖에서 안전하게 처리)
        if (con != nullptr) {
            delete con;
            std::cout << " MySQL 연결을 안전하게 닫았습니다." << std::endl;
        }

        return false;
    }

    // 서버 시작할 때 한 번 호출
    bool Run(std::string host, std::string user, std::string pass, const UINT32 threadCount_) 
{
        auto driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "root", "password");

        if (Connect(host, user, pass) == false)
        {
            std::cout << "MySQL 연결 실패!" << std::endl;
			return false;   
        }

        // 스레드 시작
        mIsTaskRun = true;

        for (UINT32 i = 0; i < threadCount_; i++)
        {
            mTaskThreads.emplace_back([this]() { TaskProcessThread(); });
        }

		return true;
    }

    // 데이터 넣기 함수
    void SaveUserData(std::string name, int level) {
        if (con) {
            sql::Statement* stmt = con->createStatement();
            std::string query = "INSERT INTO UserAccount(username, level) VALUES('" + name + "', " + std::to_string(level) + ")";
            stmt->execute(query);
            delete stmt; // 말 한마디(명령문)는 끝날 때마다 지워줘도 돼요.
        }
    }

    void End()
    {
        mIsTaskRun = false;

        for (auto& thread : mTaskThreads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }

        Disconnect();
    }

    void PushTask(MySQLTask task_)
    {
        std::lock_guard<std::mutex> guard(mReqLock);
        mRequestTask.push_back(task_);
    }

    // 서버 종료할 때 딱 한 번 호출
    void Disconnect() {
        if (con) {
            delete con;
            con = nullptr;
        }
    }

    void TaskProcessThread()
    {
        printf("MySQL 스레드 시작...\n");

        while (mIsTaskRun)
        {
            bool isIdle = true;

            if (auto task = TakeRequestTask(); task.TaskID != MySQLTaskID::INVALID)
            {
                isIdle = false;

                if (task.TaskID == MySQLTaskID::REQUEST_LOGIN)
                {
                    auto pRequest = (MySQLLoginReq*)task.pData;

                    MySQLLoginRes bodyData;
                    bodyData.Result = (UINT16)ERROR_CODE::LOGIN_USER_INVALID_PW;

					// TODO: MySQL 에서 검증하기 

                    /*std::string value;
                    if (mConn.get(pRequest->UserID, value))
                    {
                        bodyData.Result = (UINT16)ERROR_CODE::NONE;

                        if (value.compare(pRequest->UserPW) == 0)
                        {
                            bodyData.Result = (UINT16)ERROR_CODE::NONE;
                        }
                    }*/

                    //

                    MySQLTask resTask;
                    resTask.UserIndex = task.UserIndex;
                    resTask.TaskID = MySQLTaskID::RESPONSE_LOGIN;
                    resTask.DataSize = sizeof(MySQLLoginRes);
                    resTask.pData = new char[resTask.DataSize];
                    CopyMemory(resTask.pData, (char*)&bodyData, resTask.DataSize);

                    PushResponse(resTask);
                }

                task.Release();
            }


            if (isIdle)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        printf("MySQL 스레드 종료\n");
    }

    MySQLTask TakeRequestTask()
    {
        std::lock_guard<std::mutex> guard(mReqLock);

        if (mRequestTask.empty())
        {
            return MySQLTask();
        }

        auto task = mRequestTask.front();
        mRequestTask.pop_front();

        return task;
    }

    void PushResponse(MySQLTask task_)
    {
        std::lock_guard<std::mutex> guard(mResLock);
        mResponseTask.push_back(task_);
    }

    sql::Connection* con;

    bool mIsTaskRun = false;

    std::vector<std::thread> mTaskThreads;

    std::mutex mReqLock;
    std::deque<MySQLTask> mRequestTask;

    std::mutex mResLock;
    std::deque<MySQLTask> mResponseTask;

    // [추가] 조건 변수 선언
    std::condition_variable mTaskCond;

};

