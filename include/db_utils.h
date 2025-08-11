#ifndef DB_UTILS_H
#define DB_UTILS_H

#include <string>
#include <vector>

bool connectToDatabase();
void disconnectDatabase();

bool addUser(const std::wstring& username, const std::wstring& password);
int authenticateUser(SQLHDBC hDbc, const std::string& username, const std::string& plainTextPassword);
bool logFileAction(const std::string& filename, const std::string& action, int userId);
std::vector<std::string> getAllLogs();

#endif
