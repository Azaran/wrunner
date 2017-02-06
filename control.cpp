#include "control.h"

#define COUNTERMAX 100000000
#define HEARTBEATMAX 160000000000 //16
#define BENCHMARK_TIMEOUT 60

using namespace std;
using namespace boost;
using std::string;

pid_t pid = 0;
boost::asio::io_service gio_service;

int counter = -1;
int lastPause;
int lastStop;
int lastHeartbeat = -1;
unsigned long long int avgSpeed = 0;
unsigned int avgSpeedCount = 0;
float lastBoincFractionDone = 0.0;
unsigned long long int pwcount = 0;
double totaltime = 0.0;
string errorText = "no code";
bool passwordFound = false;
string password = "";


string generate_output_content() {
    //cerr << "control.cpp:" << __LINE__ << endl;
    cerr << "control.cpp:" << __LINE__ << " totaltime:" << totaltime << ", avgSpeed:"<< avgSpeed << ", pwcount" << pwcount << endl;
    string result = "";
    
    if(task_params.mode == 'b') {
        if(avgSpeed) {
            result = "b\n0\n";
            std::stringstream ss1;
            ss1 << avgSpeed;
            result += ss1.str() + "\n";
            std::stringstream ss2;
            ss2 << pwcount;
            result += ss2.str() + "\n";
            std::stringstream ss3;
            ss3 << totaltime;
            result += ss3.str() + "\n";
        }
        else {
            result = "b\n2\n" + errorText + "\n";
        }
    }
    else if(task_params.mode == 'n') {
        if(errorText.length()) {
            result = "n\n2\n" + errorText + "\n";
        }
        else if(password.length()) {
            result = "n\n0\n" + password + "\n";
            std::stringstream ss;
            ss << totaltime;
            result += ss.str() + "\n";
        }
        else {
            result = "n\n1\n";
            std::stringstream ss;
            ss << totaltime;
            result += ss.str() + "\n";
        }
    }
    else if(task_params.mode == 'v') {
        std::stringstream ss;
        ss << totaltime;
        if(errorText.length()) {
            result = "v\n2\n" + errorText + "\n";
            result += ss.str() + "\n";
        }
        else if(password.length()) {
            result = "v\n0\n" + password + "\n";
            result += ss.str() + "\n";
        }
        else {
            result = "v\n1\n";
            result += ss.str() + "\n";
        }
    }
    
    return result;
}

void create_output_file(bool boinc) {
    //cerr << "control.cpp:" << __LINE__ << endl;
    string content = generate_output_content();
    if(boinc) {
        char buf[256];
        MFILE out;
        openOutput("out", out);
        out.printf("%s", content.c_str());
        int retval = out.flush();
        if (retval) {
            fprintf(stderr, "%s APP: upper_case flush failed %d\n",
                boinc_msg_prefix(buf, sizeof(buf)), retval
            );
            exit(1);
        }
    }
    
    //cerr << "control.cpp:" << __LINE__ << endl;
    cerr << content << endl;
    
}

int heartbeat_diff() {
    return counter >= lastHeartbeat ? counter - lastHeartbeat: COUNTERMAX - lastHeartbeat + counter;
}

void control_main(class Session * session) {
    //cerr << "control.cpp:" << __LINE__ << " control_main()" << endl;
    BOINC_STATUS status;
    
    // kontrola PID
    if (0 != kill(pid, 0)) {
        //cerr << "control.cpp:" << __LINE__ << " kontrola PID" << endl;
        //create_output_file("process not alive", false, "");
        errorText = "process not alive";
        gio_service.stop();
    }
    
    // kontrola heartbeatu
    if(heartbeat_diff() > HEARTBEATMAX) {
        //cerr << "control.cpp:" << __LINE__ << " kontrola heartbeatu" << endl;
        kill(pid, SIGTERM);
        //create_output_file("process not responding", false, "");
        errorText = "process not responding";
        gio_service.stop();
    }
    
    // kontrola absolutny timeout pri benchmarku
    if(task_params.mode == 'b' && counter == BENCHMARK_TIMEOUT) {
        cerr << "control.cpp:" << __LINE__ << " absolutny timeout" << endl;
        //cerr << "lastHeartbeat=" << lastHeartbeat << endl;
        avgSpeed = 0; // informacia ze benchmark nebol uspesny
        //create_output_file("timeout", false, "");
        errorText = "timeout";
        gio_service.stop();
    }
    
    // inicializacne nastaveni
    if(counter == -1) {
        cerr << "> heartbeat;5" << endl;
        Session::send_line(session, "heartbeat;5\n");
        
        // nastaveni z BOINC manazeru
        boinc_get_status(&status);
        lastPause = status.suspended;
        lastStop = status.quit_request + status.abort_request;
    }
    else {
        //cerr << "control.cpp:" << __LINE__ << " kontrola PID" << endl;
        // pravidelna udalost kazdych 5s
        if(counter % 5 == 4) {
            //cerr << "control.cpp:" << __LINE__ << endl;
            //cerr << "> status" << endl;
            Session::send_line(session, "status\n");
        } 
    }
    
    // nastaveni z BOINC manazeru
    boinc_get_status(&status);
    
    
    
    if( (lastPause > 0 && status.suspended == 0 ) || (lastStop > 0 && status.quit_request + status.abort_request == 0)) {
        Session::send_line(session, "resume\n");
    }
    else if (lastStop == 0 && status.quit_request + status.abort_request > 0) {
        Session::send_line(session, "stop\n");
    }
    else if (lastPause == 0 && status.suspended > 0) {
        Session::send_line(session, "pause\n");
    }
    lastPause = status.suspended;
    lastStop = status.quit_request + status.abort_request;
    
    
    string simulationMessage = simulation(task_params.simulation, counter);
    if(simulationMessage != "")
        Session::send_line(session, simulationMessage);
    
    
    counter++;
    
    if(counter == COUNTERMAX)
        counter = 0;
}

void parse_message(string message, string * part1, string * part2, string * part3) {
    size_t delimeter;
    int index, n, begin;
    
    index = 0;
    //cerr << "% index=" << index << endl;
    delimeter = message.find(';', index);
    //cerr << "% delimeter=" << delimeter << endl;
    if(delimeter != string::npos) {
        *part1 = message.substr(index, delimeter - index);
        //cerr << "% part1=" << *part1 << endl;
        
        index = delimeter + 1;
        delimeter = message.find(';', index);
        if(delimeter != string::npos) {
            *part2 = message.substr(index, delimeter - index);
            //cerr << "% part2=" << *part2 << endl;
            
            index = delimeter + 1;
            delimeter = message.find(';', index);
            if(delimeter != string::npos) {
                *part3 = message.substr(index, delimeter - index);
                //cerr << "% part3=" << *part3 << endl;
            }
            else {
                *part3 = message.substr(index, string::npos);
            }
        }
        else {
            *part2 = message.substr(index, string::npos);
            *part3 = "";
        }
    }
    else {
        *part1 = message.substr(index, string::npos);
        *part2 = "";
        *part3 = "";
    }
}

void parse_message2(string message, string * part1, string * part2, string * part3) {
    size_t delimeter1, delimeter2, delimeter3;
    int index, n, begin;
    
    n = std::count(message.begin(), message.end(), ';');
    //cerr << "% n=" << n << endl;
    
    iterator_range<string::iterator> r = find_nth(message, ";", n-4);
    begin = distance(message.begin(), r.begin());
    begin = 0;
    //cerr << "% begin=" << begin << endl;
    
    index = begin + 1;
    //cerr << "% index=" << index << endl;
    delimeter1 = message.find(';', index);
    //cerr << "% delimeter1=" << delimeter1 << endl;
    if(delimeter1 != string::npos) {
        *part1 = message.substr(index, delimeter1 - index);
        //cerr << "% part1=" << *part1 << endl;
    }
    else {
        *part1 = message;
    }
    //cerr << "%" << endl;
    
    index = delimeter1 + 1;
    //cerr << "% index=" << index << endl;
    delimeter2 = message.find(';', index);
    //cerr << "% delimeter2=" << delimeter2 << endl;
    if(delimeter2 != string::npos) {
        *part2 = message.substr(index, delimeter2 - index);
        //cerr << "% part2=" << *part2 << endl;
    }
    else {
        *part2 = message;
    }
    //cerr << "%" << endl;
    
    index = delimeter2 + 1;
    //cerr << "% index=" << index << endl;
    delimeter3 = message.find(';', index);
    //cerr << "% delimeter3=" << delimeter3 << endl;
    if(delimeter3 != string::npos) {
        *part3 = message.substr(index, delimeter3 - index);
        //cerr << "% part3=" << *part3 << endl;
    }
    else {
        *part3 = message;
    }
    
}

void parse_message_simple(string message, string * part1, string * part2) {
    size_t delimeter1, delimeter2;
    
    delimeter1 = message.find(';', 0);
    if(delimeter1 != string::npos) {
        *part1 = message.substr(0, delimeter1);
    }
    else {
        *part1 = message;
    }
    
    delimeter2 = message.find(';', delimeter1 + 1);
    if(delimeter2 != string::npos) {
        *part2 = message.substr(delimeter1 + 1, delimeter2 - (delimeter1 + 1));
    }
    else {
        *part2 = message.substr(delimeter1 + 1, message.length() - (delimeter1 + 1));
    }
}

/*
void parse_message(string message, string * part1, string * part2, string * part3) {
    size_t delimeter1, delimeter2;
    
    delimeter1 = message.find(';', 0);
    if(delimeter1 != string::npos) {
        *part1 = message.substr(0, delimeter1);
    }
    else {
        *part1 = message;
    }
    
    delimeter2 = message.find(';', delimeter1 + 1);
    if(delimeter2 != string::npos) {
        *part2 = message.substr(delimeter1 + 1, delimeter2 - (delimeter1 + 1));
        *part3 = message.substr(delimeter2 + 1, message.length() - (delimeter2 + 1));
    }
    else {
        *part2 = message.substr(delimeter1 + 1, message.length() - (delimeter1 + 1));
        *part3 = "";
    }
}
*/
void control_read(string message) {
    //cerr << "control.cpp:" << __LINE__ << " " << message << endl;
    //cerr << "<message: " << message << "(" << message.length() << ")" << endl;
    //cout << "< " << message << "(" << message.length() << ")" << endl;
    
    string part1, part2, part3;
    parse_message(message, &part1, &part2, &part3);
    
    //cerr << "<parts: " << part1 << " " << part2 << " " << part3 << endl;
    
    if(part1 == "heartbeat") {
        ///cerr << "control.cpp" << __LINE__ << endl;
        lastHeartbeat = counter;
    }
    else if(part1 == "status") {
        //cerr << "control.cpp" << __LINE__ << endl;
        float done = stof(part2);
        //cerr << "control.cpp" << __LINE__ << endl;
        
        if(done > lastBoincFractionDone) {
            //cerr << "control.cpp" << __LINE__ << endl;
            //cerr << "boinc_fraction_done = " << done << endl;
            boinc_fraction_done(done);
            lastBoincFractionDone = done;
        }
    }
    else if(part1 == "code") {
        //cerr << "control.cpp" << __LINE__ << endl;
        unsigned int code = boost::lexical_cast<int>(part2);
        errorText = "";
        switch(code) {
            case 0:
                //fprintf(stderr, "control.cpp:%d\n", __LINE__);
                //create_output_file("", false, "");
                kill(pid, SIGTERM);
                gio_service.stop();
            break;
            case 1:
                //create_output_file("", false, "");
            break;
            case 2:
                //create_output_file("", true, part3);
                passwordFound = true;
                password = part3;
            break;
        }
    }
    else if(part1 == "speed") {
        //cerr << "control.cpp" << __LINE__ << endl;
        avgSpeed = boost::lexical_cast<unsigned long long int>(part2);
        //create_output_file("", false, "");
    }
    else if(part1 == "pwcount") {
        //cerr << "control.cpp" << __LINE__ << endl;
        pwcount = boost::lexical_cast<unsigned long long int>(part2);
        //create_output_file("", false, "");
    }
    else if(part1 == "totaltime") {
        //cerr << "control.cpp" << __LINE__ << endl;
        totaltime = boost::lexical_cast<double>(part2);
        //create_output_file("", false, "");
    }
    
    
}
