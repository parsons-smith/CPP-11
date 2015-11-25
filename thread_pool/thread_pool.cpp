#include <memory>
#include <queue>
#include <mutex>
#include <string>
#include <iostream>
#include <thread>
#include <atomic>
#include <future>
#include <condition_variable>

//#define DEBUG

class thread_pool{
	using Task = std::function<void()>;
private:
	std::vector<std::thread> pool;
	std::queue<Task> tasks;
	std::mutex m_task;
	std::condition_variable cv_task;
	std::atomic<bool> commit_flag;
	std::atomic<bool> terminate_flag;
	
public:
	thread_pool(size_t size = 0): commit_flag {false}, terminate_flag(false) {
		size = size < 1 ? std::thread::hardware_concurrency() : size;
		for(size_t i = 0; i< size; ++i){
			pool.emplace_back(&thread_pool::schedual, this);
		}
	}
	
	~thread_pool(){
	}

	void close_commit(){
		this->commit_flag.store(true);
	}

	bool get_commit_flag(){
		return commit_flag.load();
	}

	void open_commit(){
		this->commit_flag.store(false);
	}

	void terminate_all_thread(){
		this->terminate_flag.store(true);
		cv_task.notify_all();
		for(std::thread& thread : pool){
			thread.join();
		}
	}
	
	template<class F, class... Args>
	auto commit(F&& f, Args&&... args) ->std::future<decltype(f(args...))> {
		if(commit_flag.load()){
			throw std::runtime_error("task executor have closed commit.");
		}
		using ResType =  decltype(f(args...));
		auto task = std::make_shared<std::packaged_task<ResType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		{
			std::lock_guard<std::mutex> lock {m_task};
			tasks.emplace([task](){(*task)();});
		}
		cv_task.notify_all();
		std::future<ResType> future = task->get_future();
		return future;
	}
	
private:
	bool get_one_task(Task& task){
		std::unique_lock<std::mutex> lock {m_task};
		cv_task.wait(lock, [this](){ return !tasks.empty()||terminate_flag.load(); });
		if(!terminate_flag.load()){
			task = std::move(tasks.front());
			tasks.pop();
			return true;
		}
		return false;
	}
	
	void schedual(){
	#ifdef DEBUG
		std::cout<<"thread ["<<std::this_thread::get_id()<<"] started"<<std::endl;
	#endif
		while(!terminate_flag.load()){
			Task task;
			if(get_one_task(task)){
				task();
			}else{
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}
	#ifdef DEBUG
		std::cout<<"thread ["<<std::this_thread::get_id()<<"] terminated"<<std::endl;
	#endif
	}
};
 
 
int f(int x)
{
	std::cout << "hello, f !" << std::endl;
	return x;
}
 
struct G{
	int operator()(){
		std::cout << "hello, g !" << std::endl;
		return 42;
	}
};
 
 
int main(){
	try{
		thread_pool executor {4};
		std::future<int> ff = executor.commit(f, 50);
		std::future<int> fg = executor.commit(G{});
		std::future<std::string> fh = executor.commit([]()->std::string { std::cout << "hello, h !" << std::endl; return "hello,fh !";});
		executor.close_commit();
		std::cout<<ff.get()<<std::endl;
		std::cout << fg.get() << " " << fh.get() << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		executor.open_commit();
		executor.commit(f, 30).get();
		executor.terminate_all_thread();
		std::cout << "end..." << std::endl;
		return 0;
	}catch(std::exception& e){
		std::cout << "some unhappy happened... " << e.what() << std::endl;
	}
}