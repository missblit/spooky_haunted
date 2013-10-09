#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "process.h"

using namespace std;
	
Process::Process(string command) {
	/* TODO error handling for pretty much everything */
	/* Create interprocess pipes */
	assert( !pipe(in) );
	assert( !pipe(out) );
	/* Set output read pipe to nonblocking mode */
	int flags = fcntl(out[0], F_GETFL, 0);
	fcntl(out[0], F_SETFL, flags | O_NONBLOCK);
	
	pid = fork();
	if(!pid) {
		/* Child process */
		/* Close unused pipes */
		close( in[1]); //input  write
		close(out[0]); //output read

		/* set up std io for pipes */
		dup2(in[0],  STDIN_FILENO ); //set stdin  to read  channel of in
		dup2(out[1], STDOUT_FILENO); //set stdout to write channel of out
		dup2(out[1], STDERR_FILENO); //set stderr to write channel of out

		/* execute program */
		execl(command.c_str(), command.c_str(), (char*)NULL);
	}
	else {
		/* Parent process */
		/* Close unused pipes */
		close( in[0]); //input read
		close(out[1]); //output write
	}
}

/* TODO: properly destruct process */
Process::~Process() {}

/* TODO: handles signals and stopped processes and such */
bool Process::running(int &ret_code) {
	int stat_val;
	if(!waitpid(pid, &stat_val, WNOHANG))
		return true;
	ret_code = WEXITSTATUS(stat_val);
	return false;
}
bool Process::running() {
	int dummy;
	return running(dummy);
}
	
/* TODO: handle blocking or incomplete writes (will require multithreading */
void Process::write(string input) {
	auto count = ::write(in[1], input.c_str(), input.size());
	assert(count == input.size());
}

string Process::read() {
	/* messy code to read all pending characters into vector */
	const int block_size = 1024;
	unsigned int amount_read = 0;
	vector<char> buff(block_size);
	while(true) {
		buff.resize(amount_read + block_size);
		auto count = ::read(out[0], buff.data() + amount_read, block_size);
		if(count == 0)
			break;
		else if(count == -1) {
			if(errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			else
				assert(false);
		}
		else
			amount_read += count;
			continue;
	}
	return string(buff.begin(), buff.begin() + amount_read);
}

void Process::close_input() {
	close(in[1]);
}

/* start a bash shell
 send input from page to shell
 send output from shell to page
 detect shell closure (and restart)
*/

