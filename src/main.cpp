#include <iostream>
#include <json.hpp>

#include "connection.h"
#include "manualdriveinstruction.h"

using namespace std;
using json = nlohmann::json;

bool exists(const json& j, const string& key) {
    return j.find(key) != j.end();
}

int main() {
    cout << "Försöker etablera kontakt" << endl;
    Connection connection{1234};
    cout << "Kontakt etablerad" << endl;
    while (true) {
        cout << "Redo att läsa" << endl;
        string msg = connection.read();
        cout << "Mottaget: " << msg << endl;
        cout << "Gör om till JSON-objekt" << endl;
        json j = json::parse(msg);
        cout << "JSON-objekt: " << j << endl;
        bool is_ManualDriveInstruction = exists(j, "ManualDriveInstruction");
        cout << "Finns key ManualDriveInstruction?: " << is_ManualDriveInstruction << endl;     
        
        if (is_ManualDriveInstruction) {
            cout << "Försöker göra en instans av ManualDriveInstruction" << endl;
            ManualDriveInstruction inst1(j);
            cout << "Instansen skapades" << endl;
            cout << "Throttle: " << inst1.get_throttle() << endl;
            cout << "Steering: " << inst1.get_steering() << endl;
        }
        
        connection.write("ett svar");
    }
    return 0;
}
