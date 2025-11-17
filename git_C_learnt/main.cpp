#include <iostream>
#include <string>

/*
g++ main.cpp -o main.exe
./main.exe
*/
using namespace std; // 1未引用头文件；2未说明作用域

// << 和 >> 不仅是方向，实际上会被重载为输入输出（不能`age << cin`）
// 命名空间：避免名字（变量，函数）冲突

// 函数重载，下列是五个不同的函数，根据传入的参数的数据类型才选择对应的函数
int add(int a, int b) {}   // 或者 float a, float b
int add(int a, float b) {} // 或者 float a, int b
int add(float a, float b, float c) {}


// 函数模版 / 类模板，可以根据参数类型进行匹配
template <typename T1, typename T2> // = (class T)
T1 add(T2 a, T2 b)
{
}

template <typename TT1> // = (class T)
class Point
{
private:
    TT1 m_x;
    TT1 m_y;

public:
    Point(TT1 x, TT1 y) : m_x(x), m_y(y) {}
    TT1 getX() { return m_x; }
    TT1 getY() { return m_y; }  
};

// 类，不仅能有属性，还能有方法（函数）
class Person // 类名习惯用大写字母开头
{
private:
    string m_small_name;
    int m_age;

public:
    string m_name; // 用string减少字符串的操作难度
    int m_height;  // 成员变量命名习惯加上 m_ 前缀

    // 静态成员（属于类，通常在外部初始化）
    static int m_count;
    static void showCount(){ // 静态成员函数
        cout << "person count:" << m_count << endl;
    } 
    
    void show();

    /*构造函数（用于对象初始化）：
    名字必须和类名一致，没有返回值
    也可以重载和先声明后定义
    创建对象的时候会自动调用，每个对象肯定有一个构造函数（默认是无参无操作的构造函数）
    */
    Person(string name, int height) : m_name(name), m_height(height) { // 冒号表示参数直接进行初始化，不需要再写出初始化过程
        m_count++;
    } 
    Person(int height) : m_name("NO_Name"), m_height(height) { // 重载

        m_count++;
    }         
    /*析构函数（用于动态内存分配，资源管理和文件对象的清理操作）：
    名字和类名一致，没有返回值，有‘~’前缀
    在对象被销毁、对象超出作用域（回收栈？）的时候自动调用，每个对象肯定有一个析构函数（默认是无参无操作的析构函数）
    没有参数——>不能重载
    */
    ~Person()
    {
        cout << "Call the destructor function" << endl;
    }

    // 友元函数（是全局函数，不属于类的成员）
    friend void showName(Person &person);

    void setName(string &name); // 复杂的数据类型一般用引用参数，避免不必要的拷贝
    
};

int Person::m_count = 0;    // 静态类成员初始化！！！！要加类型（分配空间）

void Person::show()
{
    cout << "name:" << m_name << endl;
    cout << "height:" << m_height << endl;
}

void showName(Person &person) // 友元函数不需要指明作用域，属于全局
{
    cout << "name:" << person.m_name << endl;
}

// 运算符重载
class square
{
public:
    int a, b;
    square(int a, int b) : a(a), b(b) {}
    square operator+(square other) // 其他对象
    {
        return square(a + other.a, b + other.b);
    }
};

// c++的结构体支持不显示说明结构体关键字
// 类是对象的模板，对象是类的实例；；；使用（初始化和调用）均与结构体一致
// 类的成员的访问和使用权限有public（公开），private（私有），protected（受保护的）
// 私有成员一般配有“获取方法”和“设置方法”

namespace cws
{
    int is_bl = 0;
} // namespace cws

int is_bl = 1;

int main()
{
    int age, height;

    // 输入输出
    // cout << "Enter your age and height: ";
    // cin >> age >> height;    // = (cin >> age; cin >> height;)
    // cout << "Your age is " << age << endl;
    // std::cout << "Your height is " << height << endl;    // ::作用域限定符号

    // 命名空间
    // cout << "show the namespace" << endl;
    // cout << "all:" << is_bl << endl;
    // cout << "namespace cws:" << cws::is_bl << endl;

    // 引用变量（本质上是指针常量，不可覆盖，更容易理解和使用，cpp少用指针了），数据类型是int &
    // age = 18;
    // int &ref_age = age;
    // cout << "ref_age:" << ref_age << endl;  // 不需要再&
    // cout << "now input to change ref_age" << endl;
    // cin >> ref_age;     // 两人连坐
    // cout << "age:" << age << endl;

    // // 构造函数
    // Person person("Tom", 22);
    // person.show();

    // Person person2(24);
    // person2.show();

    // // 静态成员变量
    // cout << "person count:" << Person::m_count << endl; // 也可以用对象访问，但是用类访问更易懂
    // Person::showCount();

    // // 类模板的使用
    // Point<int> point_int(1, 2);
    // Point<float> point_float(2.2, 3.3);

    // 运算符重载
    square s1(1, 2);
    square s2(3, 4);
    square s3 = s1 + s2;    // 重载为 返回一个square对象
    cout << "s3 = " << s3.a * s3.b << endl; // 计算两个正方形的长和宽之和的乘积（对象之间不能直接运算！需要重载）

    return 0;
}

/*友元函数：
（外部访问类的私有成员）
class A
{
private:
    int m_age;

public:
    friend void printf_age(A a);
}

void printf_age(A a)
{
    cout << "age:" << a.m_age << endl;
}
*/

// #include <iostream>
// #include <string>
// using namespace std;

// // 提前声明
// class Teacher;

// class Student {
// private:
//     string m_name;
//     int m_id;
// public:
//     Student(const string& name, int id) : m_name(name), m_id(id) {}

//     // TODO 声明Teacher类为友元类
//      friend class Teacher;

//     // 声明普通函数displayStu为友元函数
//     friend void displayStu(Student& s);

//     // TODO 声明普通函数displayStu为友元函数
    
// };

// class Teacher {
// public:
//     // 定义Teacher类的成员函数
//     void teach(Student& s) {
//         cout << "教导的学生：" << s.m_name << endl;
//         cout << "学号：" << s.m_id << endl;
//     }
// };

// // 定义普通函数
// void displayStu(Student& s) {
//     cout << "学生姓名：" << s.m_name << endl;
//     cout << "学号：" << s.m_id << endl;
// }

// int main() {
//     Student s("张三", 10085);
//     Teacher t;
    
//     // TODO 调用Teacher类的成员函数
//  // 调用Teacher类的成员函数
//     t.teach(s); 

//     // 调用普通函数
//     displayStu(s);
    
//     // TODO 调用普通函数
    

//     return 0;
// }