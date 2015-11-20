#include <deque>
#include <mutex>
#include <future>
#include <thread>
#include <utility>
#include <iostream>

//模拟单线程池
std::mutex m;
std::deque<std::packaged_task<void()> > tasks;
int x = 0;

void process_thread(){
	while(true){
		//get and process
		std::packaged_task<void()> task;
		{
			std::lock_guard<std::mutex> lk(m);
			if(tasks.empty())
				continue;
			task = std::move(tasks.front());
			tasks.pop_front();
		}
		task();		//执行任务
	}
}

template<typename Func>
std::future<void> push_task(Func f){
	std::packaged_task<void()> task(f);
	std::future<void> res = task.get_future();
	std::lock_guard<std::mutex> lk(m);
	tasks.push_back(std::move(task));
	return res;
}

int getint(){
	std::cout<<x++<<std::endl;
	return x;
}

int main(){
	std::thread bg_process_thread(process_thread);
	for(int i = 0; i< 20; i++){
		push_task(getint);
	}
	bg_process_thread.join();
	return 0;
}