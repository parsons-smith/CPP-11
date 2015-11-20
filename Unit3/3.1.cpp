#include <list>
#include <mutex>
#include <thread>
#include <algorithm>

std::list<int> some_list;
std::mutex some_mutex;

void add_to_list(int new_value){
	std::lock_guard<std::mutex> guard(some_mutex);
	some_list.push_back(new_value);
}

bool list_contains(int value_to_find){
	std::lock_guard<std:: mutex> guard(some_mutex);
	return std::find(some_list.begin(), some_list.end(), value_to_find) != some_list.end();
}

int main(){
	std::vector<std::thread> threads;
	for(int i = 0; i < 10 ; i++){
		threads.push_back(std::thread(add_to_list, i+1));
	}
	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
	return 0;
}