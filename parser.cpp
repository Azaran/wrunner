/*
 * Input/output parser
 * 
 * Copyright (C) 2016 Radek Hranicky
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to deal 
 * in the Software without restriction, including without limitation the r  ghts 
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
 * copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 */

#include "parser.h"

#define NEWCONFIG


using namespace std;
using std::string;

TTaskParams taskParams;

void params_init() {
    taskParams.mode = 'u';
    taskParams.from = 0;
    taskParams.to = 0;
    taskParams.password = "";
    taskParams.simulation = 0;
    taskParams.charset = "lower_latin.txt";
    taskParams.length = "1:10";
    taskParams.generator = "BRUTE";
    taskParams.enable_ocl = false;
    taskParams.enable_cuda = false;
    taskParams.name = "";
}

void get_task_params(std::string inputFileName) {
    
    string line, code, param;
    std::ifstream inputStream(inputFileName);
 
    cerr << "parser.cpp" << __LINE__ << endl;
    cerr << "In file start #####################" << endl;
    cerr << inputStream.rdbuf();
    cerr << "In file end #######################" << endl;
    inputStream.clear();                 // clear fail and eof bits
    inputStream.seekg(0, ios::beg); // back to the start!
    /**
     * @brief   Safely parse input file and save content into the structure.
     */
#ifndef NEWCONFIG
    while(!safe_get_line(inputStream, line).eof()) {
        parse_message_simple(line, &code, &param);
        if(code == "mode") {
            taskParams.mode = param.c_str()[0];
        }
        else if(code == "charset") {
            taskParams.charset = param;
        }
        else if(code == "passLength") {
            taskParams.length = param;
        }
        else if(code == "from") {
            taskParams.from = boost::lexical_cast<unsigned long long int>(param);
        }
        else if(code == "count") {
            taskParams.to = taskParams.from + boost::lexical_cast<unsigned long long int>(param);
        }
        else if(code == "password") {
            taskParams.password = param;
        }
    }

#else
    while(!safe_get_line(inputStream, line).eof()) {
        parse_message_new_config(line, &code, &param);

        if(code == "name") {
            taskParams.name = param.c_str()[0];
	    cout << "name: " << taskParams.name << endl;
        }
        else if(code == "mode") {
            taskParams.mode = param.c_str()[0];
	    cout << "mode: " << taskParams.mode << endl;
        }
        else if(code == "from") {
            taskParams.from = boost::lexical_cast<unsigned long long int>(param);
	    cout << "from: " << taskParams.from << endl;
        }
        else if(code == "count") {
            taskParams.to = taskParams.from + boost::lexical_cast<unsigned long long int>(param);
	    cout << "count: " << taskParams.to << endl;
        }
        else if(code == "password") {
            taskParams.password = param;
	    cout << "password: " << taskParams.password << endl;
        }
        else if(code == "generator") {
            taskParams.generator = param;
	    cout << "generator: " << taskParams.generator << endl;
        }
        else if(code == "enable_opencl") {
	    taskParams.enable_ocl = boost::lexical_cast<bool>(param);
	    cout << "enable_opencl: " << taskParams.enable_ocl << endl;
        }
        else if(code == "enable_cuda") {
            taskParams.enable_cuda = boost::lexical_cast<bool>(param);
	    cout << "enable_cuda: " << taskParams.enable_cuda << endl;
        }
        else if(code == "passlen_min") {
            taskParams.passlen_min = boost::lexical_cast<int>(param);
	    cout << "passlen_min: " << taskParams.passlen_min << endl;
        }
        else if(code == "passlen_max") {
            taskParams.passlen_max = boost::lexical_cast<int>(param);
	    cout << "passlen_max: " << taskParams.passlen_max << endl;
	    taskParams.length = to_string(taskParams.passlen_min) + ":" + to_string(taskParams.passlen_max);
        }
        else if(code == "charset_xml") {
	    ofstream charsetFile;
	    charsetFile.open("charset.xml", ios::trunc);
	    charsetFile << decode64(param);
	    charsetFile.close(); 
	    taskParams.charset = "charset.xml";
	    cout << "charset: " << taskParams.charset << endl;
        }
    }
#endif
}

std::istream& safe_get_line(std::istream& is, std::string& t) {
    t.clear();
    /**
     * @brief	The characters in the stream are read one-by-one using a std::streambuf.
     *		That is faster than reading them one-by-one using the std::istream.
     * 		Code that uses streambuf this way must be guarded by a sentry object.
     * 		The sentry object performs various tasks, such as thread 
     * 		synchronization and updating the stream state.
     */

    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();

    for(;;) {
        int c = sb->sbumpc();
        switch (c) {
        case '\n':
            return is;
        case '\r':
            if(sb->sgetc() == '\n')
                sb->sbumpc();
            return is;
        case EOF:
	    /**
	     * @brief	Also handle the case when the last line has no line ending.
	     */
            if(t.empty())
                is.setstate(std::ios::eofbit);
            return is;
        default:
            t += (char)c;
        }
    }
}
