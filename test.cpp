#include <iostream>
#include <map>

int main()
{
    class A
    {
        public:
    typedef void(A::*f)(int, int);
    A() { s["bibek"] = &A::run;}
    void test()
    {
    std::map<std::string, f>::const_iterator it;
    it = s.find("bibek");
    (this->*it->second)(2,2);

    }

    void run(int a, int b) { std::cout << " running";}
        private:

    std::map<std::string, f> s;
    };
    A a;
    a.test();
        return 0;
}
