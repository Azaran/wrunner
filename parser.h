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

#ifndef PARSER_H
#define	PARSER_H

#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <boost/lexical_cast.hpp>

#include "control.h"

/**
 * @brief   Holds information about task.
 */
typedef struct taskParams{
    char mode;	/// Cracking mode b-benchmark, n-normal, v-verify, u-unknown.
    unsigned long long int from; /// Number of first password permutation.
    unsigned long long int to;	/// Number of last password permutation.
    std::string password;   /// Password if we have found one.
    int simulation;	/// TOASK: What is this for?
    std::string charset;    /// Charset of the task.
    std::string length;	    /// Range of the length of the password. Format 'x:y'.
} TTaskParams;

extern TTaskParams taskParams;

/**
 * @brief   Initializes values of taskParams to default.
 */
void params_init();

/**
 * @brief   Saves parameter values from the input file to the taskParams
 *	    structure.
 * @param inputFileName
 */
void get_task_params(std::string inputFileName);

/**
 * @brief   
 * @param programOutput
 * @return  
 */
std::string look_for_password(std::string programOutput);

/**
 * @brief   
 * @param is
 * @param t
 * @return  
 */
std::istream& safe_get_line(std::istream& is, std::string& t);

#endif /* PARSER_H */
