#pragma once

#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/exception.h>
class MySQLManager
{

private:
    sql::Connection* con;

public:
    int Run() {
        try {
            // --- [1단계: 연결하기] ---
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();

            // 주소, 아이디, 비번 (L 안 붙이고 std::string 사용!)
            std::string host = "tcp://127.0.0.1:3306";
            std::string user = "root";
            std::string pass = "1234";

            std::cout << "MySQL DB에 연결을 시도합니다..." << std::endl;
            con = driver->connect(host, user, pass);

            // 연결된 후 사용할 DB(Schema) 선택
            con->setSchema("my_game_db");

            std::cout << "MySQL 연결 성공!" << std::endl;


            // --- [2단계: 데이터 넣기] ---
            sql::Statement* stmt = con->createStatement();

            std::string query = "INSERT INTO UserAccount(username, password) VALUES('new_user', '1234')";

            std::cout << "데이터 저장을 시도합니다..." << std::endl;
            stmt->execute(query);

            std::cout << "데이터 저장 성공!" << std::endl;

            delete stmt; // 명령서는 쓰고 바로 버려도 돼요.

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

        return 0;
    }

    // 서버 시작할 때 한 번 호출
    bool Connect() {
        auto driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "root", "password");
        return con->isValid();
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

    // 서버 종료할 때 딱 한 번 호출
    void Disconnect() {
        if (con) {
            delete con;
            con = nullptr;
        }
    }

};

