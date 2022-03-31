#include <iostream>

#include "connection.h"

using namespace std;

int main() {
    cout << "börjar" << endl;
    Connection connection{1234};
    cout << "klar" << endl;
    while (true) {
        cout << "redo att läsa" << endl;
        string msg = connection.read();
        cout << "färdigläst: " << msg << endl;
        connection.write("ett svar");
    }
    return 0;
}
