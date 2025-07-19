#include "db_utils.h"
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <iostream>

SQLHENV hEnv = NULL;
SQLHDBC hDbc = NULL;

bool connectToDatabase() {
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
