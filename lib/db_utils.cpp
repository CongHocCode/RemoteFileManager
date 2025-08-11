#include "db_utils.h"
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <iostream>

SQLHENV hEnv = NULL;
SQLHDBC hDbc = NULL;

bool connectToDatabase() {
    //Biến để kiểm tra kết nối có thành công không
    SQLRETURN ret;

    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

    // Sửa lại thông tin kết nối phù hợp với máy bạn
    SQLWCHAR connStr[] = L"Driver={ODBC Driver 17 for SQL Server};Server=localhost;Database=FileManager;Trusted_Connection=Yes;";
    SQLWCHAR outStr[1024];
    SQLSMALLINT outStrLen;

    ret = SQLDriverConnect(hDbc, NULL, connStr, SQL_NTS, outStr, 1024, &outStrLen, SQL_DRIVER_COMPLETE);
    if (SQL_SUCCEEDED(ret)) {
        std::cout << "[+] Kết nối SQL Server thành công!\n";
        return true;
    }
    else {
        std::cout << "[-] Kết nối thất bại!\n";
        return false;
    }
}

void disconnectDatabase()
{
    if (hDbc)
    {
        //Disconnect trước rồi free handle
        SQLDisconnect(hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        hDbc = NULL;
    }
    if (hEnv)
    {
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        hEnv = NULL;
    }
}

bool addUser(const std::wstring& username, const std::wstring& password)
{
    //password trong hàm và PasswordHash là cột trong bảng
    // Có gì hash password trước rồi lưu vào PasswordHash cho tiện !
    //Xin cấp phát statement handle
    SQLHSTMT hStmt;
    //Biến để kiểm tra việc xin cấp phát statement handle thành công không
    SQLRETURN ret;
    ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        std::cerr << "Loi: Khong the cap phat statement handle." << std::endl;
        return false;
    }
    //Chuẩn bị lệnh SQL với tham số (?)
    SQLWCHAR sql[] = L"INSERT INTO Users(Username, PasswordHash) VALUES(? , ? ); ";
    ret = SQLPrepare(hStmt, sql, SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        std::cerr << "Loi: Khong the chuan bi cau lenh SQL." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return false;
    }

    //Gắn dữ liệu vào các tham số (?)
    
    //Gắn 'Username' vào dấu '?' đầu tiên
    ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
        50, 0, (SQLPOINTER)username.c_str(), 0, NULL);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        std::cerr << "Loi: Khong the gan tham so username." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return false;
    }

    //Gắn 'passwordHash' vào dấu '?' thứ hai
    ret = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
        255, 0, (SQLPOINTER)password.c_str(), 0, NULL);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) 
    {
        std::cerr << "Loi: Khong the gan tham so passwordHash." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return false;
    }
    

    //Thực thi câu lệnh sql
    ret = SQLExecute(hStmt);
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
    {
        std::wcout << "Them user '" << username << "' thanh cong." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return true;
    }
    else
    {
        std::cerr << "Loi: Thuc thi INSERT user that bai (co the do username da ton tai)." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return false;
    }
}

int authenticateUser(SQLHDBC hDbc, const std::string& username, const std::string& plainTextPassword)
{
    SQLHSTMT hStmt = SQL_NULL_HSTMT;
    SQLRETURN ret;
    int userId = -1; // Mặc định là thất bại
    SQLCHAR storedHash[256]; //Buffer để nhận hash từ DB
    SQLLEN hashLen;

    //Cấp phát handle
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    //Chuẩn bị lệnh SQL
    SQLWCHAR sql[] = L"SELECT Id, PasswordHash FROM Users WHERE Username = ?;";
    SQLPrepare(hStmt, sql, SQL_NTS);

    //Gắn tham số usernaem vào dấu '?'
    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
        50, 0, (SQLPOINTER)username.c_str(), 0, NULL);

    //Thực thi lệnh
    ret = SQLExecute(hStmt);

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
    {
        //Gắn các cột kết quả vào biến C++
        SQLBindCol(hStmt, 1, SQL_C_SLONG, &userId, sizeof(userId), NULL);
        SQLBindCol(hStmt, 2, SQL_C_CHAR, storedHash, sizeof(storedHash), &hashLen);

        //Lấy các dòng kết quả đầu tiên bằng Fetch
        ret = SQLFetch(hStmt);
        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
        {
            //So sánh mật khầu
            std::string hashFromDb = (const char*)storedHash;
            //hash mật khẩu rồi so sánh với cái hash đã stored trước đó
        }
    }
}
