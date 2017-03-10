/*
 * Wrathion runner for BOINC client for dictionary-based version
 *
 * Copyright (C) 2016 Radek Hranicky, 2017 Vojtech Vecera
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

#include "main.h"

using namespace std;
using std::string;
using boost::asio::ip::tcp;

bool boinc = false;

string find_exec_file(string path) {
    cerr << "main.cpp " << __LINE__ << " find_exec_file(begin) -" << endl;
    string crackerPath = PATH_TO_CRACKER;
    string file;
    DIR *dir = opendir(path.c_str()); 
    if(dir)
    {
	struct dirent *ent; 
	while((ent = readdir(dir)) != NULL) 
	{
	    file = ent->d_name;
	    if(file != FILE_SEARCH_KEY && file.find(FILE_SEARCH_KEY) != string::npos) {
		cerr << "main.cpp " << __LINE__ << " find_exec_file() -" << crackerPath << endl;
		if(boost::lexical_cast<int>(file.substr(8, file.length() - 8)) - \
			boost::lexical_cast<int>(crackerPath.substr(8, crackerPath.length() - 8)) > 0) {
		    crackerPath = file;
		}
		cerr << "main.cpp " << __LINE__ << " find_exec_file() -" << crackerPath << endl;
	    }
	} 
    }
    cerr << "main.cpp " << __LINE__ << " find_exec_file() -" << crackerPath << endl;
    return crackerPath;
}

void print_secondary_process_params(std::vector<char*> args) {
    cerr << "./";
    for (std::vector<char*>::iterator i = args.begin(); i != args.end(); i++)
	cerr << *i << " ";
    cerr << endl;
}

static void safe_create_dir(const char *dir) {
    if (mkdir(dir, 0755) < 0) {
	if (errno != EEXIST) {
	    perror(dir);
	    exit(1);
	}
    }
}

int zip_extract(char *archive) {
    struct zip *za;
    struct zip_file *zf;
    struct zip_stat sb;
    char buf[100];
    int err;
    int i, len;
    int fd;
    long long sum;

    if ((za = zip_open(archive, 0, &err)) == NULL) {
	return 1;
    }

    char * pch;
    char directory[256];
    int index;

    for (i = 0; i < zip_get_num_entries(za, 0); i++) {
	if (zip_stat_index(za, i, 0, &sb) == 0) {

	    /**
	     * @brief Creates  directory tree from ZIP 
	     */
	    pch = strchr((char *)sb.name, '/');
	    while(pch != NULL)
	    {
		index = pch - sb.name + 1;
		memcpy( directory, sb.name, index);
		directory[index] = '\0';
		safe_create_dir(directory);
		pch=strchr(pch + 1, '/');
	    }

	    len = strlen(sb.name);
	    if (sb.name[len - 1] == '/') {
		safe_create_dir(sb.name);
	    } else {
		zf = zip_fopen_index(za, i, 0);
		if (!zf) {
		    return 1;
		}

		fd = open(sb.name, O_RDWR | O_TRUNC | O_CREAT, 0644);
		if (fd < 0) {
		    return 1;
		}

		sum = 0;
		while (sum != sb.size) {
		    len = zip_fread(zf, buf, 100);
		    if (len < 0) {
			return 1;
		    }
		    write(fd, buf, len);
		    sum += len;
		}
		close(fd);
		zip_fclose(zf);
	    }
	}
    }   

    if (zip_close(za) == -1) {
	return 1;
    }

    return 0;
}

inline bool file_exists (const string& name) {
    struct stat buffer;   
    return (stat (name.c_str(), &buffer) == 0); 
}

int main(int argc, char **argv) {
    int retval;
    char buf[256];
    MFILE out;
    string password = "";
    string xmlFile;

    /**
     * @brief   Initializes and sets taskParams based on xmlFile
     */
    params_init();
    standalone(argc, argv, &xmlFile);
    if(taskParams.mode == 'u') {
	boinc = true;
    }

    /**
     * @brief   On mode 'u' initiliaze boinc and extract ZIP with OCL kernels
     */
    if(boinc) {
	retval = boinc_init();
	//cerr << "main.cpp " << __LINE__ << " boinc_init()" << endl;
	zip_extract(KERNELS_ARCHIVE);
    }

    /**
     * @brief   On boinc_init error print error on STDERR
     */
    if (retval) {
	fprintf(stderr, "%s boinc_init returned %d\n",
		boinc_msg_prefix(buf, sizeof(buf)), retval
	       );
	exit(retval);
    }

    /**
     * @brief   Read information from files sent by server about task
     */
    cerr << "main.cpp " << __LINE__ << endl;
    if(boinc) {
	cerr << "main.cpp " << __LINE__ << endl;
	get_task_params(resolve_input_file("in"));
	cerr << "main.cpp " << __LINE__ << endl;
	xmlFile = resolve_input_file("data");
    }

    cerr << "Runner is running with parameters:" << endl;
    cerr << "mode - " << taskParams.mode << endl;
    cerr << "from - " << taskParams.from << endl;
    cerr << "to - " << taskParams.to << endl;
    cerr << "password - " << taskParams.password << endl;
    cerr << "simulation - " << taskParams.simulation << endl;
    cerr << "charset - " << taskParams.charset << endl;
    cerr << "generator - " << taskParams.generator << endl;
    cerr << "length - " << taskParams.length << endl << endl;

    /**
     * @brief	'u' mode is just for the "help" message and is used as default
     *		value for the case when there wouldn't be specific value in
     *		config file
     */
    if(taskParams.mode == 'u') {
	printf("./wrunner\n");
	printf("1. (benchmark) -m b -c lower_lattin.txt -x test.xml\n");
	printf("2. (normal) -m n -c lower_lattin.txt -f 1000 -t 2000 -x test.xml -s 1\n");
	printf("3. (verify) -m v -c lower_lattin.txt -p heslo-base64 -x test.xml\n");
	return 0;
    }

    cerr << "main.cpp " << __LINE__ << endl;
    Server s(gioService);
    cerr << "main.cpp " << __LINE__ << endl;
    cerr << "Port " << s.listeningPort << endl;

    /**
     * @brief   Run Wrathion
     */
    string crackerPath = find_exec_file(".");
    chmod(crackerPath.c_str(), S_IRWXU); //S_IRGRP|S_IXGRP|S_IROTH


#ifdef __WIN32
    /// TODO: Needs corrections or rework to work under windows
    cerr << "Windows sucks" << endl;
    /*
       string w_cracker = "fitcracker.exe -m BOINC";
       w_cracker += " -i ";
       w_cracker += xmlFile;
       w_cracker += " --mport ";
       w_cracker += s.listeningPort.c_str();
       w_cracker += " -v ";
       if(taskParams.mode == 'n') {
       w_cracker += " --index ";
       w_cracker += taskParams.from;
       w_cracker += ":";
       w_cracker += taskParams.to;
       }
       else if(taskParams.mode == 'v') {
       w_cracker += " -g SINGLEPASS ";
       w_cracker += " --pw64 ";
       w_cracker += taskParams.password;
       }

       SECURITY_ATTRIBUTES sa; 
       printf("\n->Start of parent execution.\n");
    // Set the bInheritHandle flag so pipe handles are inherited. 
    sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
    sa.bInheritHandle = TRUE; 
    sa.lpSecurityDescriptor = NULL; 
    // Create a pipe for the child process's STDERR. 
    if ( ! CreatePipe(&g_hChildStd_ERR_Rd, &g_hChildStd_ERR_Wr, &sa, 0) ) {
    exit(1); 
    }
    // Ensure the read handle to the pipe for STDERR is not inherited.
    if ( ! SetHandleInformation(g_hChildStd_ERR_Rd, HANDLE_FLAG_INHERIT, 0) ){
    exit(1);
    }
    // Create a pipe for the child process's STDOUT. 
    if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &sa, 0) ) {
    exit(1);
    }
    // Ensure the read handle to the pipe for STDOUT is not inherited
    if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) ){
    exit(1); 
    }
    // Create the child process. 
    PROCESS_INFORMATION piProcInfo = CreateChildProcess(w_cracker);

    // Read from pipe that is the standard output for child process. 
    printf( "\n->Contents of child process STDOUT:\n\n", argv[1]);
    password = ReadFromPipe(piProcInfo); 

    printf("\n->End of parent execution.\n");
    */
    //end of __WIN32

#else

    int pipefd[2];
    pipe(pipefd);
    pid=fork();
    if (pid==0) { /// child (cracking tool) process

	/**
	 * @brief  Sets OCL configs 
	 */
	string openclConfig1 = "";
	string openclConfig2 = "";
	if(file_exists(PATH_TO_OCLCONFIG)) {
	    openclConfig1 = "--opencl-set";
	    ifstream ifs(PATH_TO_OCLCONFIG);
	    openclConfig2.assign( (istreambuf_iterator<char>(ifs) ), (istreambuf_iterator<char>() ) );
	    openclConfig2.erase(find_if(openclConfig2.rbegin(), openclConfig2.rend(), not1(ptr_fun<int, int>(isspace))).base(), openclConfig2.end());
	    fprintf(stderr, ">>>>>>>>>>>>>>%s\n", openclConfig2.c_str());
	    openclConfig2 = "0:" + openclConfig2;
	}

	/**
	 * @brief  Sets listening port of the server to the tool
	 */
	stringstream port;
	port << s.listeningPort;

	/**
	 * @brief  Sets what tool should we run and with what parameters
	 */
	cerr << "main.cpp " << __LINE__ << endl;
	std::vector<char*> args;
	prepare_args(args, crackerPath, port.str(), openclConfig1, openclConfig2, xmlFile);

	close(pipefd[0]);    // close reading end in the child
	dup2(pipefd[1], 1);  // send stdout to the pipe
	dup2(pipefd[1], 2);  // send stderr to the pipe
	close(pipefd[1]);    // this descriptor is no longer needed


	/**
	 * @brief  Prints parameters and name of the binary file which we want to execute
	 */
	cerr << "main.cpp " << __LINE__ << endl;
	int result = 0;
	cerr << "Fitcrack running as: " << endl;
	print_secondary_process_params(args);
	execv(args[0], &args[1]);

	/**
	 * @brief   We test result and if cracking tool ended with
	 *	    error we redirect is STDERR to client's 
	 */
	if( result != 0 )
	{
	    char buffer[ 256 ];
	    char * errorMessage = strerror_r( errno, buffer, 256 ); // get string message from errno

	    //cerr << __LINE__ << errorMessage << endl;
	}

	//cerr << "main.cpp " << __LINE__ << endl;

	/**
	 * @brief   Only if mode wasn't 'b' and program ran by execv failed
	 *	    while running
	 */
	exit(127); 
    }
    else { /// pid!=0; parent process

	string programOutput = "";
	char out_buffer;
	close(pipefd[1]); 

	cerr << "PID: " <<  pid << endl;
	//cerr << "main.cpp:" << __LINE__ << endl;
	s.start_accept();

	//int test = gio_service&ioService.run();
	//cerr << "main.cpp:" << __LINE__ << endl;
	//fprintf(stderr, "main.cpp:%d\n", __LINE__);

	cerr << "main.cpp:" << __LINE__ << endl;

	while (read(pipefd[0], &out_buffer, 1) != 0)
	{
	    programOutput += out_buffer;
	}
	cerr << "main.cpp:" << __LINE__ << endl;
	cerr << programOutput << endl;
    }
    // end of else    
#endif

    /**
     * @brief  Creates output file and finishes the boinc task  
     */
    create_output_file(boinc);
    //cerr << "main.cpp:" << __LINE__ << endl;

    if(boinc) {
	boinc_fraction_done(1);
	//cerr << "main.cpp:" << __LINE__ << endl;
	boinc_finish(0);
    }
    //cerr << "main.cpp:" << __LINE__ << endl;
}

void prepare_args(std::vector<char*>& vArgs, string crackerPath, string port, string openclConfig1, string openclConfig2, string xmlFile) {

	stringstream ss1, ss2;

	ss1 << taskParams.from;
	ss2 << taskParams.to;
	string fromTo = ss1.str() + ":" + ss2.str();

	ss1.clear();
	ss2.clear();

	ss1 << taskParams.passlen_min;
	ss2 << taskParams.passlen_max;
	string plen = ss1.str() + ":" + ss2.str();

	// common part of args for all modes
	// it is this ugly coz no c++11

	vArgs.push_back(const_cast<char*>(crackerPath.c_str()));
	vArgs.push_back("-m");
	vArgs.push_back("BOINC");
	vArgs.push_back("-i");
	vArgs.push_back(const_cast<char*>(xmlFile.c_str()));
	vArgs.push_back("--mport");
	vArgs.push_back(const_cast<char*>(port.c_str()));
	vArgs.push_back("-g");

	if (taskParams.mode != 'v') {
	    vArgs.push_back("-l");
	    vArgs.push_back(const_cast<char*>(plen.c_str()));

	    if(taskParams.mode != 'b'){
		vArgs.push_back(const_cast<char*>((taskParams.generator).c_str()));
		vArgs.push_back("-b");
	    } else { // mode == 'n'
		vArgs.push_back("--index");
		vArgs.push_back(const_cast<char*>(fromTo.c_str()));
	    }

	} else {
	    vArgs.push_back("SINGLEPASS");
	    vArgs.push_back("--pw64");
	    vArgs.push_back(const_cast<char*>((taskParams.password).c_str()));
	}

	if (taskParams.generator == "DICT"){
	    vArgs.push_back("-d");
	    vArgs.push_back("dict");
	} else {
	    vArgs.push_back("-c");
	    vArgs.push_back(const_cast<char*>((taskParams.charset).c_str()));
	}
	
	if (taskParams.enable_ocl) {
	    vArgs.push_back("-o");
	    vArgs.push_back(const_cast<char*>(openclConfig1.c_str()));
	    vArgs.push_back(const_cast<char*>(openclConfig2.c_str()));
	}
	vArgs.push_back(0);
}

#ifdef __WIN32
PROCESS_INFORMATION CreateChildProcess(string procName){

    //char szCmdline[]="test --log_level=all --report_level=detailed";

    PROCESS_INFORMATION piProcInfo; 
    STARTUPINFO siStartInfo;
    bool bSuccess = FALSE; 

    /**
     * @brief	Set up members of the PROCESS_INFORMATION structure. 
     */
    ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );

    /**
     * @brief	Set up members of the STARTUPINFO structure. 
     *		This structure specifies the STDERR and STDOUT handles for redirection.
     */
    ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
    siStartInfo.cb = sizeof(STARTUPINFO); 
    siStartInfo.hStdError = g_hChildStd_ERR_Wr;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    LPSTR cmdline = const_cast<char*>(procName.c_str());

    /**
     * @brief	Create the child process. 
     */
    bSuccess = CreateProcess(NULL, 
	    //szCmdline,     // command line 
	    cmdline,
	    NULL,          // process security attributes 
	    NULL,          // primary thread security attributes 
	    TRUE,          // handles are inherited 
	    0,             // creation flags 
	    NULL,          // use parent's environment 
	    NULL,          // use parent's current directory 
	    &siStartInfo,  // STARTUPINFO pointer 
	    &piProcInfo);  // receives PROCESS_INFORMATION
    CloseHandle(g_hChildStd_ERR_Wr);
    CloseHandle(g_hChildStd_OUT_Wr);

    /**
     * @brief	If an error occurs, exit the application. 
     */
    if ( ! bSuccess ) {
	exit(1);
    }
    return piProcInfo;
}

string ReadFromPipe(PROCESS_INFORMATION piProcInfo) {
    DWORD dwRead; 
    CHAR chBuf[BUFSIZE];
    bool bSuccess = FALSE;
    string out = "", err = "";
    for (;;) { 
	bSuccess=ReadFile( g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
	if( ! bSuccess || dwRead == 0 ) break; 

	string s(chBuf, dwRead);
	out += s;
    } 
    dwRead = 0;
    for (;;) { 
	bSuccess=ReadFile( g_hChildStd_ERR_Rd, chBuf, BUFSIZE, &dwRead, NULL);
	if( ! bSuccess || dwRead == 0 ) break; 

	string s(chBuf, dwRead);
	err += s;

    } 
    cerr << "stdout:" << out << endl;
    cerr << "stderr:" << err << endl;
    return out;
}
#endif
