// new.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
#include<unordered_map>
#include <stack>
#include<typeinfo>


//template<typename T>
class StackCell
{
	//std::stack<StackCell<T>> MyStack;
public:
	virtual ~StackCell() = default;
};

template<typename T>
class MyStack:public StackCell
{
public:
	std::stack<T> MyStackInfo;
	~MyStack()final {};
	MyStack() {};
};
 

class StackInfo
{
private:
	std::unordered_map<std::size_t /*std::type_info*/, StackCell*> MyMap;
public:
	template<typename T>
	void push(const T& PushTarget)
	{
		std::cout << "Push:"<< typeid(T).hash_code() << std::endl;

		//StackCell* p = nullptr;
		MyStack<T>* q = nullptr;
		if (!MyMap[typeid(T).hash_code()])
		{
			q = new MyStack<T>();
			MyMap[typeid(T).hash_code()] = q;
		}
		else
			q = dynamic_cast<MyStack<T>*>( MyMap[typeid(T).hash_code()] );
		q->MyStackInfo.push(PushTarget);
	}

	template<typename T>
	void pop(T& Search) 
	{
		std::cout << "Pop:" << typeid(T).hash_code() << std::endl;
		if (MyMap[typeid(T).hash_code()])
		{
			MyStack<T>* Get = dynamic_cast<MyStack<T>*>(MyMap[typeid(T).hash_code()]);
			if (Get->MyStackInfo.size() == 0) {
				std::cout << "Stack 0 " << std::endl;
				return;
			}
			Search = Get->MyStackInfo.top();
			Get->MyStackInfo.pop();
		}
		else
		{
			std::cout << "Not Find " << std::endl;
		}
	}
};

class Test
{
public:
	int p1 = 1;
	bool p2 = false;
	std::string p3 = "p33333333";

	void testWithStack(StackInfo& stack)
	{
		int v1 = 0;
		int v5 = 2;
		std::string v2;
		Test v3;
		Test * v4;// = NULL;

		stack.pop(v1);
		stack.pop(v5);
		stack.pop(v2);
		stack.pop(v3);
		stack.pop<Test*>(v4);

		std::cout << "testWithStack ";
		std::cout << v1 << " ";
		std::cout << v2 << " ";
		std::cout << v3.p3 << " ";
		std::cout << v4->p3 << std::endl;

		stack.push(1);
	}
};

void test2()
{
	Test t1;

	StackInfo stack;  // 实现此类，就俩模板函数，push和pop
	stack.push(111);
	stack.push<std::string>("asdfasdfasdf");

	Test t2;
	t2.p3 = "t2teststring";
	stack.push(t2);
	stack.push(&t1);

	t1.testWithStack(stack);

	int ret = 0;
	stack.pop(ret);
	std::cout << "testWithStack ret " << ret << std::endl;
}

int main()
{
	test2();
	//StackInfo test;
	//int a = 1;
	//int b = 333333;

	//test.push(a);
	//test.push(b);
	//std::cout << test.pop(a) << std::endl;
}
