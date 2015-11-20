#include <future>
#include <iostream>
#include <string>

struct X{
	void foo(int a, std::string const& b){
		std::cout<<a<<":"<<b<<std::endl;
	}
	std::string bar(std::string const& c){
		std::cout<<c<<std::endl;
		return c;
	}
};

struct Y{
	double operator()(double d){
		std::cout<<d<<std::endl;
	}
};

int main(){
	X x;
	Y y;
	auto f1 = std::async(&X::foo, &x, 42, "hello");	//p->foo(42,"hello") 其中p = &x
	auto f2 = std::async(&X::bar, x, "goodbye");	//tmpx.bar("goodbye") 其中tmpx 是x的副本
	auto f3 = std::async(Y(), 3.141);			//tmpy(3.141) 其中tmpy是从Y()构造移动过来的
	auto f4 = std::async(std::ref(y), 2.718);		//调用y(2.718)
	//X baz(X&);
	//std::async(baz, std::ref(x));
	f1.get();
	std::cout<<f2.get()<<"+answer"<<std::endl;
	f3.get();
	f4.get();
	return 0;
}