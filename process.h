#ifndef MISSBLIT_PROCESS_H
#define MISSBLIT_PROCESS_H

#include <string>

/* Class for managing a sub-process interactively */
class Process {
public:
	/* Constructor launches a new child process */
	Process(std::string command);

	/* Destructor kills the child as well */
	~Process();

	/* Returns true if the process is still running
       Ret code is set to return code if process is done running */
	bool running();
	bool running(int& ret_code);

	/* Writes data to child's stdin
       launches another thread in-case not all data can be written at once */
	void write(std::string input);
	
	/* closes child's stdin, when there is no more input */
	void close_input();

	/* Reads all pending output */
	std::string read();
	//string read_stdout();
	//string read_stderr();
private:
	int pid;
	int in[2]; //stdin, read from in[0], write to in[1] 
	int out[2]; //stdout
	//int err[2]; //stderr
};

#endif //MISSBLIT_PROCESS_H
