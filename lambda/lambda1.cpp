#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>

std::function<int(int)> make_offset(int offset){
	return [=](int j){return offset + j; };	//使用局部变量时，在[]中加上=(捕捉偏移量的副本)
}

int main(){
	std::function<int(int)> offset_42 = make_offset(42);
	std::function<int(int)> offset_123 = make_offset(123);
	std::cout<<offset_42(12)<<","<<offset_123(12)<<std::endl;


	int offset = 42;
	std::function<int(int)> offset_a = [&](int j){return offset + j; };	//使用&，捕捉当前空间变量的引用
	offset = 123;
	std::function<int(int)> offset_b = [&](int j){return offset + j; };
	std::cout<<offset_a(12)<<","<<offset_b(12)<<std::endl;
	offset = 99;
	std::cout<<offset_a(12)<<","<<offset_b(12)<<std::endl;


	int i = 1234, j= 5678, k= 9;
	std::function<int()> f = [=, &j, &k]{return i+j+k; };		//i是副本捕捉，j和k是引用捕捉
	i = 1;
	j = 2;
	k = 3;
	std::cout<<f()<<std::endl;

	i = 1234, j= 5678, k= 9;
	std::function<int()> f_ = [&, j, k]{return i+j+k; };		//&设置默认采用引用捕捉，&后面跟以复制捕捉的变量列表
	i = 1;
	j = 2;
	k = 3;
	std::cout<<f_()<<std::endl;


	i = 1234, j= 5678, k= 9;
	std::function<int()> f__ = [&i, j, &k]{return i+j+k; };		//列出需要捕捉的变量，通过前缀&表明是引用捕捉
	i = 1;
	j = 2;
	k = 3;
	std::cout<<f__()<<std::endl;


	struct X{							//lambda访问类成员，必须讲this指针添加到捕捉列表中先行捕捉
		int some_data;
		void foo(std::vector<int>& vec){
			std::for_each(vec.begin(), vec.end(), [this](int& i){ i += some_data; });
		}
	};
	return 0;
}