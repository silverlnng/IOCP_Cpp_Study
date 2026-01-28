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

            con->setSchema("my_game_db");

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

    // Packet Manager 의 ProcessThread 에서 계속 확인하고 작동 중
    MySQLTask TakeResponseTask()
    {
        std::lock_guard<std::mutex> guard(mResLock);

        if (mResponseTask.empty())
        {
            return MySQLTask();
        }

        auto task = mResponseTask.front();
        mResponseTask.pop_front();

        return task;
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
                    // 기본값: 비밀번호 틀림으로 설정
                    bodyData.Result = (UINT16)ERROR_CODE::LOGIN_USER_INVALID_PW;

                    printf("[MySQL::TaskProcessThread] 로그인 TaskID : %s\n", pRequest->UserID);

                    // TODO: MySQL 에서 검증하기 

                    // -------------------------------------------------------------
                    // [수정] 로그인 요청 처리 (SELECT 쿼리 검증)
                    // -------------------------------------------------------------


                    try {

                        // 1) Statement 생성
                   // 주의: 멀티스레드 환경에서는 커넥션 풀을 쓰거나 lock이 필요하지만, 
                   // 현재 구조상 mConn 하나를 공유하므로 간단히 구현합니다.
                        sql::Statement* stmt = con->createStatement();

                        // 2) 쿼리 작성 (UserAccount 테이블, username 컬럼 기준)
                        // 실제로는 SQL Injection 방지를 위해 PreparedStatement 사용을 권장합니다.
                        std::string query = "SELECT password FROM UserAccount WHERE id = '";
                        query += std::string(pRequest->UserID) + "'";

                        // 3) 쿼리 실행
                        sql::ResultSet* res = stmt->executeQuery(query);

                        // 4) 결과 확인
                        if (res->next()) {
                            // DB에 유저가 존재함 -> 비밀번호 가져오기
                            std::string dbPw = res->getString("password");
                         

                            // 5) 비밀번호 검증
                            // pRequest->UserPW: 유저가 보낸 비번, dbPw: DB에 저장된 비번
                            if (dbPw == std::string(pRequest->UserPW))
                            {
                                bodyData.Result = (UINT16)ERROR_CODE::NONE; // 성공 [2]


                                MySQLTask resTask;
                                resTask.UserIndex = task.UserIndex;
                                resTask.TaskID = MySQLTaskID::RESPONSE_LOGIN;
                                resTask.DataSize = sizeof(MySQLLoginRes);
                                resTask.pData = new char[resTask.DataSize];
                                CopyMemory(resTask.pData, (char*)&bodyData, resTask.DataSize);

                                PushResponse(resTask);

                                task.Release();

                            }
                            else {
                                printf("[MySQL::TaskProcessThread] Password Mismatch: %s\n",pRequest->UserID);
                            }
                        }
                        else {
                            // DB에 유저가 없음
                            printf("[MySQL::TaskProcessThread] User Not Found: %s\n",pRequest->UserID);
                            // 필요하다면 ERROR_CODE::LOGIN_USER_NOT_FOUND 등을 정의해서 사용
                        }

                        // 리소스 해제
                        delete res;
                        delete stmt;
                    }
                    catch (sql::SQLException& e) {
                        printf("[MySQL Error::TaskProcessThread] %s\n", e.what());
                        bodyData.Result = (UINT16)ERROR_CODE::MYSQL_SERVER_ERROR; // 서버 에러 처리
                    }
                }
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

