#include <atomic>
#include <memory>
#include <thread>
#include <iostream>

//mutile-productor, single-consumer

template<typename T>
class lock_free_queue{
private:
	struct  node;
	struct counted_node_ptr{
		int external_count;
		node* ptr;
	};

	struct node_counter{
		unsigned internal_count:30;
		unsigned external_counters:2;
	};

	struct node{
		std::atomic<T*> data;
		std::atomic<node_counter> count;
		counted_node_ptr next;
		node(){
			node_counter new_count;
			new_count.internal_count = 0;
			new_count.external_counters = 2;
			count.store(new_count);
			next.ptr = nullptr;
			next.external_count = 0;
		}
	};

public:
	lock_free_queue():head(new node), tail(head.load()){}
	lock_free_queue(const lock_free_queue& other) = delete;
	lock_free_queue& operator=(const lock_free_queue& other) = delete;
	~lock_free_queue(){
		while(node* const old_head = head.load()){
			head.store(old_head->next);
			delete old_head;
		}
	}

	std::shared_ptr<T> pop(){
		node* old_head = pop_head();
		if(!old_head){
			return std::shared_ptr<T>();
		}
		std::shared_ptr<T> const res(old_head->data);
		delete old_head;
		return res;
	}

	void push(T new_value){
		std::unique_ptr<T> new_data(new T(new_value));
		counted_node_ptr new_next;
		new_next.ptr = new node;
		new_next.external_count = 1;
		counted_node_ptr old_tail = tail.load();

		for(;;){
			increase_ex
		}
	}
};

lock_free_queue<int> lfq;

void push_to_queue(){
	for(int i = 0; i < 100; i++){
		lfq.push(i);
	}
}

void pop_from_queue(){
	for(int i = 0; i < 100; i++){
		std::shared_ptr<int> s = lfq.pop();
		if(s.get() != nullptr)
			std::cout<<*s.get()<<std::endl;
		else
			std::cout<<"NULL"<<std::endl;
	}
}

int main(){
	std::thread w = std::thread(push_to_queue);
	std::thread x = std::thread(pop_from_queue);
	w.join();
	x.join();
	return 0;
}