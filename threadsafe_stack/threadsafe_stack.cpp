#include <memory>
#include <mutex>
#include <stack>
#include <vector>
#include <thread>
#include <algorithm>
#include <iostream>

template <typename T>
class threadsafe_stack{
private:
	std::stack<T> data;
	mutable std::mutex m;
public:
	threadsafe_stack(){}
	threadsafe_stack(const threadsafe_stack& other){
		std::lock_guard<std::mutex> lock(other.m);
		data = other.data;
	}

	threadsafe_stack& operator= (const threadsafe_stack&) = delete;

	void push(T new_value){
		std::lock_guard<std::mutex> lock(m);
		data.push(new_value);
	}

	std::shared_ptr<T> pop(){
		std::lock_guard<std::mutex> lock(m);
		if(data.empty()) return NULL;
		std::shared_ptr<T> const res(std::make_shared<T>(data.top()));
		data.pop();
		return res;
	}

	void pop(T& value){
		std::lock_guard<std::mutex> lock(m);
		if(data.empty()) return NULL;
		value = data.top();
		data.pop();
	}

	bool empty() const{
		std::lock_guard<std::mutex> lock(m);
		return data.empty();
	}
};

threadsafe_stack<int> ts;
std::mutex mu;
void pushtostack(int t_){
	ts.push(t_);
}

void popfromstack(){
	std::shared_ptr<int> p = ts.pop();
	if(p != NULL)
		std::cout<<*p<<std::endl;
	else
		std::cout<<"NULL"<<std::endl;
}


int main(){
	std::vector<std::thread> threads;
	for(int i = 0; i < 10 ; i++){
		threads.push_back(std::thread(pushtostack, i+1));
	}
	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
	std::vector<std::thread> threads_;
	for(int i = 0 ; i < 11 ; i++){
		threads_.push_back(std::thread(popfromstack));
	}
	std::for_each(threads_.begin(), threads_.end(), std::mem_fn(&std::thread::join));
	return 0;
}