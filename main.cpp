#include <memory>
#include <mutex>
#include <thread>
#include <future>
#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WString>
#include "process.h"

using namespace std;

class App : public Wt::WApplication
{
public:
    App(const Wt::WEnvironment& env);

private:
	void run_process();
	void handle_input();

    Wt::WText *screen;
    Wt::WLineEdit *prompt;

	mutex input_mutex; 
	string input_buffer;
};

App::App(const Wt::WEnvironment& env)
    : Wt::WApplication(env)
	 // p("wump"), 
{
    setTitle("HUNT THE WUMPUUUS!!!!!!!");
    screen = new Wt::WText    ( root() );
	screen->setTextFormat(Wt::PlainText);
    root()->addWidget( new Wt::WBreak );
    prompt  = new Wt::WLineEdit( root() );
	prompt->changed().connect(this, &App::handle_input);
	run_process();
}

void App::handle_input() {
	//p.write( input->text() );
	string in = prompt->text().toUTF8();
	prompt->setText("");

	std::lock_guard<std::mutex> lock(input_mutex);
	input_buffer += in;
}

void App::run_process() {
	//enable server side updates
	enableUpdates(true);

	//asynchronous
	auto fun = [&]() {
		Process p("/usr/bin/wump");
		string input, output;
		while(p.running())
		{
			if(hasQuit())
				return;
			//fetch input from buffer			
			{
				std::lock_guard<std::mutex> lock(input_mutex);
				if(!input_buffer.empty()) {
					input = input_buffer;
					input_buffer.clear();
					goto was_input;
				}
				goto no_input;
			}
			was_input:		
				cout << "input: " << input << endl;
				p.write(input + "\n");
				input.clear();
			no_input:

			std::this_thread::yield();

			string text = p.read();
			if(text != "")
				cout << "read: " << text << " fin!" << endl; 
			if(!output.empty() && !text.empty()) {
				output += text;
				continue;
			}
			output += text;
			if(!output.empty()) {
				cout << "writing!!!\n";
				{
					Wt::WApplication::UpdateLock lock(this);
					if(!lock)
						return;
					auto contents = screen->text();
					contents += output;
					screen->setText(contents);
					triggerUpdate();
				}
				output.clear();
			}
		}
		return;
	};
	thread t1(fun);
	t1.detach();
	//async(std::launch::async, fun);
	//cout << "Bar\n";
}

Wt::WApplication *createApplication(const Wt::WEnvironment &env) {
	auto app = new App(env);
	//app->enableUpdates(true);
	return app;
}

int main(int argc, char **argv)
{
	Wt::WRun(argc, argv, createApplication);
}
