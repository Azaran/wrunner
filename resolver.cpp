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

FILE * open_input(std::string inputFilename) {
    char inputPath[PATH_SIZE];
    char buf[BUFFER_SIZE];
    FILE *infile;
    int retval;
    boinc_resolve_filename(inputFilename.c_str(), inputPath, sizeof(inputPath));
    infile = boinc_fopen(inputPath, "r");
    if (!infile) {
        fprintf(stderr,
            "%s Couldn't find input file, resolved name %s.\n",
            boinc_msg_prefix(buf, sizeof(buf)), inputPath
        );
        exit(-1);
    }
    //file_size(inputPath, fsize); /* not needed anymore */
    return infile;
}

std::string resolve_input_file(std::string inputFilename) {
    char inputPath[PATH_SIZE];
    boinc_resolve_filename(inputFilename.c_str(), inputPath, sizeof(inputPath));
    std::string infile(inputPath);
    return infile;
}

std::string get_encrypted_file(std::string inputFilename) {
    char encryptedFile[PATH_SIZE];
    boinc_resolve_filename(inputFilename.c_str(), encryptedFile, sizeof(encryptedFile));
    std::string efile(encryptedFile);
    return efile;
}

void open_output(std::string outputFilename, MFILE &out) {
    char openPath[PATH_SIZE];
    char buf[BUFFER_SIZE];
    int retval;
    boinc_resolve_filename(outputFilename.c_str(), openPath, sizeof(openPath));
    retval = out.open(openPath, "wb");
    if (retval) {
        fprintf(stderr, "%s APP: upper_case output open failed:\n",
            boinc_msg_prefix(buf, sizeof(buf))
        );
        fprintf(stderr, "%s resolved name %s, retval %d\n",
            boinc_msg_prefix(buf, sizeof(buf)), openPath, retval
        );
        perror("open");
        exit(1);
    }
}
