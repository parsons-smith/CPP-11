#include <atomic>
#include <memory>
#include <thread>
#include <iostream>

template<typename T>
class lock_free_stack{
private:
	struct node{
		T data;
		node* next;
		node(T const& data_):data(data_){}
	};
	std::atomic<node*> head;
public:
	lock_free_stack():head(nullptr){
	}
	~lock_free_stack(){}
	void push(T const& data){
		node* const new_node = new node(data);
		new_node->next = head.load();
		while(!head.compare_exchange_weak(new_node->next, new_node));

	}
	void pop(T& result){
		node* old_head = head.load();
		while(old_head && !head.compare_exchange_weak(old_head, old_head->next));
		result = old_head->data;
	}

	T pop(){
		node* old_head = head.load();
		while(old_head && !head.compare_exchange_weak(old_head, old_head->next));
		return old_head->data;
	}
};

lock_free_stack<int> lfs;

void push_to_stack(){
	for(int i = 0; i < 50; i++){
		lfs.push(i);
	}
}

void pop_from_stack(){
	for(int i = 0; i < 50; i++){
		int x;
		lfs.pop(x);
		std::cout<<x<<std::endl;
	}
}

int main(){
	std::thread w = std::thread(push_to_stack);
	std::thread s = std::thread(push_to_stack);
	w.join();
	s.join();
	std::thread x = std::thread(pop_from_stack);
	std::thread y = std::thread(pop_from_stack);
	x.join();
	y.join();
	return 0;
}