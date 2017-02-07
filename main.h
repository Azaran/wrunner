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

#ifdef __WIN32 // WINDOWS --------------------------------------
const char *Wrathion_path = "wrathion.exe";
#pragma warning( disable : 4800 ) // stupid warning about bool
#define BUFSIZE 4096
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;
HANDLE g_hChildStd_ERR_Rd = NULL;
HANDLE g_hChildStd_ERR_Wr = NULL;

/**
 * @brief   Creates child process with given name
 *
 * @param proc_name
 *
 * @return  PROCESS_INFORMATION
 */
PROCESS_INFORMATION CreateChildProcess(std::string proc_name);

/**
 * @brief   Reads standard outputs of the given process
 *
 * @param piProcInfo
 *
 * @return  content of STDOUT 
 */
std::string ReadFromPipe(PROCESS_INFORMATION piProcInfo);

#else // ------- GNU/Linux -------------------------------------
    const char *Wrathion_path = "fitcrack41";
#endif

extern bool boinc;

#endif /* WRUNNER_H */
