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

#ifndef WRUNNER_H
#define	WRUNNER_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <signal.h>
#include <dirent.h> 
#include <unistd.h>
#include <fstream>
#include <string>

#ifdef __WIN32
#include <windows.h>
#else
#include <unistd.h>    /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h>  /* for wait */
#include <sys/stat.h>
#include <zip.h>
#endif

#include "boinc_api.h"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#include "resolver.h"
#include "parser.h"
#include "control.h"
#include "socket.h"
#include "standalone.h"

#define PATH_TO_OCLCONFIG "../../opencl.config"
#define PATH_TO_CRACKER "fitcrack0"
#define FILE_SEARCH_KEY "fitcrack"
#define KERNELS_ARCHIVE "kernels6.zip"

/**
 * @brief   Find cracking tool's execution file in the directory 
 * @param path
 * @return    crackerPath
 */
std::string find_exec_file(std::string path);

/**
 * @brief   Prints parameters of with which is the child run
 * @param args
 */
void print_secondary_process_params(char ** args);

/**
 * @brief   Creates new directory with privileges 0755 (safely)
 * @param dir
 */
static void safe_create_dir(const char *dir);

/**
 * @brief   Extracts ZIP archive with OCL kernels
 * @param archive
 * @return    0 - when everything when OK
 *	      1 - when there was some reading or opening / closing issue
 */
int zip_extract(char *archive);

/**
 * @brief   Check if file with given name exists.
 * @param name
 * @return  true - if exists
 *	    false - if doesn't
 */
inline bool file_exists (const std::string& name);

/**
 * @brief   Main function of Runner
 * @param argc
 * @param argv
 * @return  0
 */
int main(int argc, char **argv);
}

#ifdef __WIN32 // WINDOWS --------------------------------------
//const char *Wrathion_path = "wrathion.exe";
#pragma warning( disable : 4800 ) // stupid warning about bool
#define BUFSIZE 4096
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;
HANDLE g_hChildStd_ERR_Rd = NULL;
HANDLE g_hChildStd_ERR_Wr = NULL;

/**
 * @brief   Creates a child process with given process name which uses the previously created pipes
 *	    for STDERR and STDOUT
 * @param proc_name
 * @return  piProcInfo
 */
PROCESS_INFORMATION CreateChildProcess(std::string proc_name);

/**
 * @brief   Read output from the child process's pipe for STDOUT and write to the parent process's pipe for STDOUT.
 Stop when there is no more data. 
 * @param piProcInfo
 * @return   out
 */
std::string ReadFromPipe(PROCESS_INFORMATION piProcInfo);

#else // ------- GNU/Linux -------------------------------------
// TODO: I guess this isnt right is it?
//const char *Wrathion_path = "fitcrack41";
#endif

extern bool boinc;

#endif /* WRUNNER_H */
