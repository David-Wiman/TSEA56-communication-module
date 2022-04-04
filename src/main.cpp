#include <iostream>
#include <boost/asio.hpp>
#include <json.hpp>

#include "connection.h"
#include "manualdriveinstruction.h"
#include "drivedata.h"
#include "log.h"

using namespace std;
using json = nlohmann::json;

bool exists(const json& j, const string& key) {
    return j.find(key) != j.end();
}

int main() {
    Logger log1{"log.txt"}; //create a log
    
    cout << "Försöker etablera kontakt" << endl;
    Connection connection{1234};
    cout << "Kontakt etablerad" << endl;
    while (true) {
        cout << "Redo att läsa" << endl;
        
        string msg{};
        try {
            msg = connection.read();
        } catch (boost::exception&) {
            cout << "Lost connection" << endl;
            break;
        }
        
        cout << "Mottaget: " << msg << endl;
        cout << "Gör om till JSON-objekt" << endl;
        
        json j{};
        try {
            j = json::parse(msg);
        } catch (std::invalid_argument&) {
            cout << "Invalid argument" << endl;
        }
        
        cout << "JSON-objekt: " << j << endl;
        bool is_ManualDriveInstruction = exists(j, "ManualDriveInstruction");
        cout << "Finns key ManualDriveInstruction?: " << is_ManualDriveInstruction << endl;     
        
        if (is_ManualDriveInstruction) {
            cout << "Försöker göra en instans av ManualDriveInstruction" << endl;
            ManualDriveInstruction inst1(j);
            cout << "Instansen skapades" << endl;
            cout << "Throttle: " << inst1.get_throttle() << endl;
            cout << "Steering: " << inst1.get_steering() << endl;
            
            log1.log("Throttle", inst1.get_throttle());
            log1.log("Steering", inst1.get_steering());
        }
        
        connection.write("ETT SVAR");
    }
    return 0;
}
