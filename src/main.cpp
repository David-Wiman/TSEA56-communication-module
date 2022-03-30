#include <iostream>

#include "connection.h"

using namespace std;

int main() {
    cout << "börjar" << endl;
    Connection connection{1234};
    cout << "klar" << endl;
    cout << "redo att läsa" << endl;
    connection.read();
    cout << "färdigläst" << endl;
    return 0;
}
