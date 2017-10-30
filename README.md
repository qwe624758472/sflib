# sflib
`c++` 的一些工具库

## lib_sf_debug

日志库

使用示例：

```cpp
#include "sf_debug.h"

using namespace skyfire;
sf_debug<> sflog;

int main()
{
	//下面两句顺序不能颠倒
	//打开日志开关
	sflog.set_out_to_file(true);
	//设置日志文件
	sflog.set_debug_file("runlog.txt");
	sflog.sf_logout("hello", "这是多条演示", 666, 5.6, 5.7f, 9L);
	sflog.sf_logout(5);
	sflog.sf_track("打印在调试器中", 666);
}
```

以上代码将在`runlog.txt`文件中打印：

```
[2017-10-31 00:14:56][1][main.cpp][14][main][hello][这是多条演示][666][5.6][5.7][9]
[2017-10-31 00:14:56][1][main.cpp][15][main][5]
```

在终端中则会打印：

```
[2017-10-31 00:14:56][1][main.cpp][16][main][打印在调试器中][666]
```



## libsf_empty_class

空类

主要为其他类提供服务（如`lib_sf_debug`）



## libsf_finally

在离开作用域执行一些操作（通常用于清理）

示例：

```cpp
#include <iostream>
#include "sf_finally.h"

using namespace std;
using namespace skyfire;

int main()
{
    try{
        sf_finally t([](){
            cout<<"123456"<<endl;
        });
        cout<<"hello world"<<endl;
        {
            sf_finally t([](){
                cout<<"666"<<endl;
            });
            throw int(5);
        }
    }
    catch(int t)
    {
        cout<<"catch "<<t<<endl;
    }
}
```

此代码输出：

```
hello world
666
123456
catch 5
```

可以看出，即使出现异常，该工具依然可以工作的很好（通过`exit`这种“非正常”的方式离开作用于该工具就无能为力了）



## libsf_range

范围

提供类似`Python`中`range`类似的功能。

示例：

```cpp
#include "sf_range.h"
#include <iostream>

using namespace skyfire;
using namespace std;

int main()
{
	for (auto p : range(5))
	{
		cout << p << endl;
	}
	cout << "----------" << endl;
	for(auto p:range(9,0,-2))
	{
		cout << p << endl;
	}
	cout << "----------" << endl;
	for(auto p:range(2,6))
	{
		cout << p << endl;
	}
}
```

代码输出：

```
0
1
2
3
4
----------
9
7
5
3
1
----------
2
3
4
5
```



## libsf_single_instance

单例类

提供类的单例限制

示例：

```cpp
#include "sf_single_instance.h"
#include <iostream>

class TestClass:public skyfire::sf_single_instance<TestClass>
{
private:
	TestClass()
	{
	}

public:
	void func() const	{
		std::cout << "hello world" << std::endl;
	} 
	friend class skyfire::sf_single_instance<TestClass>;
};


int main()
{
	auto p = TestClass::get_instance();
	p->func();
}
```

代码输出：

```
hello world
```



## libsf_thread_pool

一个简单的线程池类

示例：

```cpp
#include "sf_thread_pool.h"
#include <iostream>
#include <chrono>
#include "../libsf_empty_class/sfempty_class.h"

std::mutex g_mu;

std::atomic<int> add_sum {0};
std::atomic<int> mul_sum {0};

void add(int a, int b)
{
	add_sum += (a + b);
}

void mul(int a, int b)
{
	mul_sum += a*b;
}

const std::string getCurrentSystemTime()
{
	auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm* ptm = localtime(&tt);
	char date[60] = {0};
	sprintf(date, "%d-%02d-%02d      %02d:%02d:%02d",
		(int)ptm->tm_year + 1900,(int)ptm->tm_mon + 1,(int)ptm->tm_mday,
		(int)ptm->tm_hour,(int)ptm->tm_min,(int)ptm->tm_sec);
	return std::string(date);
}

int main()
{
	skyfire::sf_thread_pool<int, int> pool(8);
	std::cout << getCurrentSystemTime()<<std::endl;
	for (auto i = 0; i < 100000; ++i)
	{
		pool.add_task(add, i, i + 1);
		pool.add_task(mul, i, i + 1);
	}
	pool.wait_all_task_finished();
	std::cout << add_sum << std::ends << mul_sum << std::endl;
	std::cout << getCurrentSystemTime()<<std::endl;

	add_sum = mul_sum = 0;
	std::cout << getCurrentSystemTime()<<std::endl;
	for (auto i = 0; i < 100000; ++i)
	{
		add(i, i + 1);
		mul(i, i + 1);
	}
	std::cout << add_sum << std::ends << mul_sum << std::endl;
	std::cout << getCurrentSystemTime()<<std::endl;

}
```

代码输出：

```
2017-10-31      00:36:36
1410065408921457440
2017-10-31      00:36:37
2017-10-31      00:36:37
1410065408921457440
2017-10-31      00:36:37
```



## libsf_tri_type

检验变量是否被赋值的工具

示例：

```cpp
#include <iostream>
#include "sf_tri_type.h"


int main()
{
	skyfire::sf_tri_type<bool> b(false);
	std::cout << bool(b) << std::endl;
	std::cout << *b << std::endl;
	b = false;
	std::cout << bool(b) << std::endl;
	std::cout << *b << std::endl;
}

```

代码输出：

```
0
0
1
0
```



## libsf_watch_time

统计作用域中消耗时间的工具：

示例：

```cpp
#include "sf_watch_time.h"

skyfire::sf_watch_time<int> wc;
using pt = skyfire::check_point<int>;

void func1()
{
	pt p(1, wc);
	auto sum = 0;
	for (auto i = 0; i < 1000; ++i)
	{
		pt p(2, wc);
		sum += i;
	}
	for (auto i = 0; i < 10000; ++i)
	{
		pt p(3, wc);
		sum += i;
	}
}


void func2()
{
	pt p(4, wc);
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main()
{
	func1();
	func2();
	wc.watch();
}
```

代码输出：

```
                                     SkyFire Time Watch
=========================================================================================
Points                        |     Thread      |     Time
=========================================================================================
1                             |     1           |         4ms   286us 643ns
2                             |     1           |               24us  761ns
3                             |     1           |               360us 599ns
4                             |     1           |   1s          75us  684ns
-----------------------------------------------------------------------------------------
```



## libsf_serialize

序列化

支持所有的`POD`类型。

示例代码：

```cpp
#include "sf_serialize.h"
#include <iostream>


using namespace skyfire;
using namespace std;

struct myType
{
    int t;
    double u;
    char str[1024];
};

int main()
{
    int a{5};
    double b{9.6};
    float c{6.54f};
    string d{"hello world"};
    vector<string> e{"nihao","hahaha","the end"};
    tuple<string,double,myType> f{"113assd",6.87,myType{6,5.6,"yyyyyy"}};
    auto t=sf_serialize(a,b,c,d,e,f);
    int aa;
    double bb;
    float cc;
    string dd;
    vector<string> ee;
    tuple<string,double,myType> ff;
    size_t pos=0;
    pos=sf_deserialize(t,aa,pos);
    pos=sf_deserialize(t,bb,pos);
    pos=sf_deserialize(t,cc,pos);
    pos=sf_deserialize(t,dd,pos);
    pos=sf_deserialize(t,ee,pos);
    pos=sf_deserialize(t,ff,pos);
    cout<<aa<<endl;
    cout<<bb<<endl;
    cout<<cc<<endl;
    cout<<dd<<endl;
    for(auto p:ee)
    {
        cout<<p<<endl;
    }
    cout<<get<0>(ff)<<endl;
    cout<<get<1>(ff)<<endl;
    cout<<get<2>(ff).t<<endl;
    cout<<get<2>(ff).u<<endl;
    cout<<get<2>(ff).str<<endl;
    
}
```

代码输出：

```
5
9.6
6.54
hello world
nihao
hahaha
the end
113assd
6.87
6
5.6
yyyyyy
```

