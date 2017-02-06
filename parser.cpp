/*
 * Input/output parser
 * 
 * Copyright (C) 2016 Radek Hranicky
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to deal 
 * in the Software without restriction, including without limitation the rights 
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

using std::string;

TTaskParams task_params;

void params_init() {
    task_params.mode = 'u';
    task_params.from = 0;
    task_params.to = 0;
    task_params.password = "";
    task_params.simulation = 0;
    task_params.charset = "lower_latin.txt";
    task_params.length = "1:10";
}


void getTaskParams(std::string input_file_name) {
    
    string line, code, param;
    std::ifstream input_stream(input_file_name);
    
    while(!safeGetline(input_stream, line).eof()) {
        parse_message_simple(line, &code, &param);
        if(code == "mode") {
            task_params.mode = param.c_str()[0];
        }
        else if(code == "charset") {
            task_params.charset = param;
        }
        else if(code == "passLength") {
            task_params.length = param;
        }
        else if(code == "from") {
            task_params.from = boost::lexical_cast<unsigned long long int>(param);
        }
        else if(code == "count") {
            task_params.to = task_params.from + boost::lexical_cast<unsigned long long int>(param);
        }
        else if(code == "password") {
            task_params.password = param;
        }
    }
}

std::istream& safeGetline(std::istream& is, std::string& t)
{
    t.clear();
    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

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
            // Also handle the case when the last line has no line ending
            if(t.empty())
                is.setstate(std::ios::eofbit);
            return is;
        default:
            t += (char)c;
        }
    }
}
