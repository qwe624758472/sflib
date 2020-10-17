#include <sf_finally>
#include <sf_test>
#include <sf_stdc++>

using namespace skyfire;
using namespace std;


bool test_finally_order(){
    int a = 10;
    {
        finally([&](){
            a*=10;
        });
        {
            finally([&](){
                a+=10;
            });
        }
    }
    return a == 200;
}

bool test_finally_order2()
{
    int a = 10;
    {
        finally([&]() {
            a *= 10;
        });
        finally([&]() {
            a += 10;
        });
    }
    return a == 200;
}

int main(){
    sf_test_add(test_finally_order);
    sf_test_add(test_finally_order2);

    test_run();
}