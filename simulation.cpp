#include "simulation.h"

using namespace std;
using std::string;

string simulation(int index, unsigned long long int counter) {
    string result = "";
    switch(index) {
        case 1:
        switch(counter) {
            case 15: result = "pause"; break;
            case 25: result = "resume"; break;
        }
        break;
        case 2:
        switch(counter) {
            case 20: result = "stop"; break;
        }
        break;
    }
    return result;
}