#include "standalone.h"

using namespace std;
using std::string;

void standalone(int argc, char **argv, string * xmlFile) {
    char m = 'u';
    unsigned long long int f;
    unsigned long long int t;
    string p;
    string x;
    int s = 0;
    string c;
    string l;
    int ch;
    
    opterr = 0;
    
    while ((ch = getopt (argc, argv, "m:f:t:p:x:s:c:l:")) != -1) {
        switch (ch) {
            case 'm':
                m = optarg[0];
            break;
            case 'f':
                f = boost::lexical_cast<unsigned long long int>(optarg);
            break;
            case 't':
                t = boost::lexical_cast<unsigned long long int>(optarg);
            break;
            case 'p':
                p = optarg;
            break;
            case 'x':
                x = optarg;
            break;
            case 's':
                s = atoi(optarg);
            break;
            case 'c':
                c = optarg;
            break;
            case 'l':
                l = optarg;
            break;
        }
    }
    
    if(m == 'u')
        return;
    
    taskParams.mode = m;
    taskParams.charset = c;
    *xmlFile = x;
    
    if(m == 'b') {
        taskParams.length = l;
    }
    else if(m == 'n') {
        taskParams.from = f;
        taskParams.to = t;
        taskParams.length = l;
        taskParams.simulation = s;
    }
    else if(m == 'v') {
        taskParams.password = p;
    }
    
}
