/*
 * BOINC logical file name resolver
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

#include <iostream>

#include "boinc_api.h"
#include "resolver.h"

FILE * openInput(std::string input_filename) {
    char input_path[PATH_SIZE];
    char buf[BUFFER_SIZE];
    FILE *infile;
    int retval;
    boinc_resolve_filename(input_filename.c_str(), input_path, sizeof(input_path));
    infile = boinc_fopen(input_path, "r");
    if (!infile) {
        fprintf(stderr,
            "%s Couldn't find input file, resolved name %s.\n",
            boinc_msg_prefix(buf, sizeof(buf)), input_path
        );
        exit(-1);
    }
    //file_size(input_path, fsize); /* not needed anymore */
    return infile;
}

std::string resolveInputFile(std::string input_filename) {
    char input_path[PATH_SIZE];
    boinc_resolve_filename(input_filename.c_str(), input_path, sizeof(input_path));
    std::string infile(input_path);
    return infile;
}

std::string getEncryptedFile(std::string input_filename) {
    char encrypted_file[PATH_SIZE];
    boinc_resolve_filename(input_filename.c_str(), encrypted_file, sizeof(encrypted_file));
    std::string efile(encrypted_file);
    return efile;
}

void openOutput(std::string output_filename, MFILE &out) {
    char output_path[PATH_SIZE];
    char buf[BUFFER_SIZE];
    int retval;
    boinc_resolve_filename(output_filename.c_str(), output_path, sizeof(output_path));
    retval = out.open(output_path, "wb");
    if (retval) {
        fprintf(stderr, "%s APP: upper_case output open failed:\n",
            boinc_msg_prefix(buf, sizeof(buf))
        );
        fprintf(stderr, "%s resolved name %s, retval %d\n",
            boinc_msg_prefix(buf, sizeof(buf)), output_path, retval
        );
        perror("open");
        exit(1);
    }
}
