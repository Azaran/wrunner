/*
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

#ifndef RESOLVER_H
#define	RESOLVER_H

#define PATH_SIZE 512
#define BUFFER_SIZE 256

/**
 * @brief   Opens input file with given filename.
 * @param inputFilename
 * @return  infile 
 */
FILE * open_input(std::string inputFilename);

/**
 * @brief   Resolves filename and makes absolute path to the file.
 * @param inputFilename
 * @return  infile 
 */
std::string resolve_input_file(std::string inputFilename);

/**
 * @brief   Resolves filename and makes absolute path to the encrypted file.
 * @param inputFilename
 * @return  
 */
std::string get_encrypted_file(std::string inputFilename);

/**
 * @brief   Resolves filename and opens output filename.
 * @param outputFilename
 * @param out
 */
void open_output(std::string outputFilename, MFILE &out);

#endif /* RESOLVER_H */
