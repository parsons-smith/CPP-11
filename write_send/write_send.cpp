#include <mutex>
#include <queue>
#include <iostream>
#include <thread>
#include <string>
#include <condition_variable>

std::mutex mut;
std::queue<std::string> data_queue;
std::condition_variable data_cond;

void write_thread(){
	int i = 0;
	while(i <50 ){
		std::string const data = std::string("xxxxxx");
		std::lock_guard<std::mutex> lk(mut);
		data_queue.push(data);
		data_cond.notify_one();
	}
	
}


void send_thread(){
	while(true){
		std::unique_lock<std::mutex> lk(mut);
		data_cond.wait(lk, []{return !data_queue.empty();});
		std::string data = data_queue.front();
		data_queue.pop();
		lk.unlock();
		std::cout<<data<<std::endl;
	}
}

int main(){
	std::thread w = std::thread(write_thread);
	std::thread s = std::thread(send_thread);
	w.join();
	s.join();
	return 0;
}