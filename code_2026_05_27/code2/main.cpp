#include "mySharedPtr.h"
#include <iostream>
using namespace std;

class MyClass
{
public:
    MyClass()
    {
        cout << "调用构造函数" << endl;
    }

    ~MyClass()
    {
        cout << "调用析构函数" << endl;
    }
};

int main()
{
    {
        mySharedPtr<MyClass> p1(new MyClass());
        {
            mySharedPtr<MyClass> p2 = p1;
            cout << "拷贝给p2" << endl;
            {
                mySharedPtr<MyClass> p3 = p1;
                cout << "拷贝给p3" << endl;
                cout << "p3析构" << endl;
            }
            cout << "p2析构" << endl;
        }
        cout << "p1析构" << endl;
    }
    return 0;
}