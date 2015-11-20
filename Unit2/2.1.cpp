#include <iostream>
#include <thread>
#include <unistd.h>
#include <exception>
using namespace std;

struct func{
	int& i;
	func(int& i_):i(i_){}
	void operator()(){
		for(unsigned j = 0; j< 100000; ++j)
			cout<<i+j<<":";
		cout<<endl;
	}
};


class thread_guard{
	std::thread& t;
public:
	explicit thread_guard(std::thread& t_):t(t_){}
	~thread_guard(){
		if(t.joinable())
			t.join();
	}
	thread_guard(thread_guard const&) = delete;
	thread_guard& operator=(thread_guard const&) = delete;
};

int main(){
	//2.1-------detach, ensure that & and * in a new thread, follow code has an error
	/*
	{
		int some_local_state = 0;
		func my_func(some_local_state);
		std::thread my_thread(my_func);
		my_thread.detach();
	}
	*/

	//2.2 --------the main thread must ensure the child thread would be joined
	/*
	{
		int some_local_state = 0;
		func my_func(some_local_state);
		std::thread t(my_func);
		try{
			sleep(5);
		}catch(exception &e){
			t.join();
			throw;
		}
		t.join();
	}
	*/

	//2.3----------thread_guard, throw join function to thread_guard var
	/*
	{
		int some_local_state = 0;
		func my_func(some_local_state);
		std::thread t(my_func);
		thread_guard g(t);
		sleep(5);
	}
	*/
	int st = std::thread::hardware_concurrency();
	cout<<st<<endl;	

}
