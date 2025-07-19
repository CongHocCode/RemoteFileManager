#include <iostream>
#include "db_utils.h"

int main() {
    if (connectToDatabase()) {
        std::cout << "Sẵn sàng truy vấn CSDL!\n";
    }
    else {
        std::cout << "Không kết nối được CSDL!\n";
    }
    return 0;
}
