#pragma once

#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>

class MySQLManager
{
public:
    int Run() {
        try {
            // 1. 요리사(드라이버) 데려오기
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();

            std::string host = "tcp://127.0.0.1:3306";
            std::string user = "root";
            std::string pass = "1234";

            // 2. 주소, 아이디, 비밀번호로 연결 (비밀번호는 본인 것으로 수정!)
            sql::Connection* con = driver->connect(host, user, pass);

            // 3. 성공 메시지 출력
            printf("MySQL 서버 연결에 성공\n");
          
            delete con; // 사용한 연결은 꼭 닫아주기
        }
        catch (sql::SQLException& e) {
            // 연결 실패 시 이유 알려주기
            std::cout << "MySQL 서버 연결 실패... 이유: " << e.getErrorCode() << std::endl; 
            std::cout << "MySQL 서버 연결 실패... 이유: " << e.what() << std::endl;
        }
        return 0;
    }
};

