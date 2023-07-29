
/*
* 需要了解C++的面向对象用法
* 了解Cmake，git
* 设计思路
* vector 存储所有用户
* 对象：用户
* 用户数据：
*   ⽤户名（最多15位，不可重复）、
    性别、出⽣⽇期、邮箱（最多30位）、
    UID（从1开始，按照输⼊顺序⾃动⽣成）、
    硬币（初始值100，不能为负）；
  用户方法：
    创建
    输出账户所有信息
    删除后输出
    修改用户硬币后输出
* Manager：
*   输出所有用户
*   按用户名删除
*   按用户名修改用户硬币
*   排序所有用户，硬币->年龄
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
using namespace std;

int calculateAge( string user_bir )
{
    time_t seconds = time(nullptr);
    tm* cur_time = localtime( &seconds );
    int cur_year = cur_time->tm_year + 1900;
    int cur_month = cur_time->tm_mon + 1;
    int cur_day = cur_time->tm_mday;

    char ch;
    int user_year, user_day, user_mon;

    istringstream iss(user_bir);
    iss >> user_year >> ch >> user_mon >> ch >> user_day;

    int age = cur_year - user_year - 1;
    if (cur_month >= user_mon and cur_day >= user_day)
        age+=1;
    return age;
}

class User
{
public:
    string name;
    char sex;
    string email;
    string birthday;
    int uid;
    int have_coins = 100;
    struct number_birthday
    {
        int year;
        int month;
        int day;
        
    };
    User(string name, char sex,string birthday, string email,  int uid, int have_coins = 100)
    {
        this->name = name;
        this->sex = sex;
        this->uid = uid;
        this->have_coins = have_coins;
        this->birthday = birthday;
        this->email = email;

    }
    // 还差年龄
    void print_me() 
    {
        
        cout << "$用户名: " << this->name << "\n";

        
        if (this->sex == 'M')	cout << "性别: " << "男" << "\n";
        else	cout << "性别: " << "女" << "\n";

        cout << "出生日期: " << this->birthday << "\n";
        cout << "邮箱: " << this->email << "\n";
        cout << "UID: " << this->uid << "\n";
        cout << "硬币: " << this->have_coins << "\n";
        cout << "年龄: " << calculateAge(this->birthday) << "\n\n";
    }
    void change_coin(int cout)
    {
        this->have_coins = cout;
    }

    // 此处重载方式只有target值是name才有效
    bool operator == (const string& name) const
    {
        return this->name == name;
    }

    // 重复调用，会不会出现问题？
    number_birthday Convert_BithdayString_to_Number()
    {
        size_t head = 0;
        size_t tail = this->birthday.find("-",head);
        
    

        int year = stoi( this->birthday.substr( head, tail - head ) );

        head = tail + 1;
        tail = this->birthday.find("-",head);
        

        int month = stoi( this->birthday.substr(head, tail - head) );
        head = tail + 1;

        int day = stoi( this->birthday.substr(head));
        number_birthday b = { year , month ,day };
        return b;
    }
    // 硬币 多，年龄大的排前者
    bool operator < ( User& other) 
    {
        if (this->have_coins != other.have_coins)
            return (-this->have_coins) < (-other.have_coins);
        User::number_birthday b_this = this->Convert_BithdayString_to_Number();
        User::number_birthday b_other = other.Convert_BithdayString_to_Number();
        if (b_this.year != b_other.year) 
            return b_this.year < b_other.year;
        if (b_this.month != b_other.year)
            return b_this.month < b_other.month;
        return b_this.day < b_other.month;
    }

};
vector< User > user_list;
class Manager
{
public:
    static int already_found ;
    
    void print_all()
    {
        for ( auto& user : user_list)
        {
            user.print_me();
        }
    }
    void SignIn(string name, char sex, string birthday, string email  )
    {
        
        user_list.push_back(  User( name,  sex,  birthday,  email,  Manager::already_found ) );
        Manager::already_found += 1;
    }
    auto Serach_based_name(string name)
    {
        return find(user_list.begin(), user_list.end(), name);
    }
    // 返回bool通知调用者是否删除成功，抽象层之间通信
    bool Delet_based_name(string name)
    {
        auto it = Manager::Serach_based_name(name);
        if (it != user_list.end())
        {

            user_list.erase(it);
            return true;
        }
        else return false;
    }
    // 返回指向被修改的元素的迭代器供调用者返回，抽象层之间通信
    auto ChangeUserCoin(string name, int newcoins)
    {
        auto it = Manager::Serach_based_name( name );
        if (it != user_list.end())
        {
            (*it).change_coin(newcoins);
            
        }
        return it;
    }

    //注意：排序完了还有打印，无法分离出打印。
    void sort_based_CoinAge()
    {        
        sort(user_list.begin(), user_list.end());        
    }
    bool name_is_ok(string name)
    {
        return name.length() <= 15 and Manager::Serach_based_name(name) == user_list.end();

    }
    bool email_is_ok(string eamil)
    {
        return eamil.length()<=30;
    }
    void Ask_AddUser()
    {
            
        string name;
        cout << "your name?\n";
        cin >> name;
        while ( ! name_is_ok(name))
        {
            cout << "wrong! repeat!\n";
            cin >> name;
        }

        cout << "your sex M/F?\n";
        char sex;
        cin >> sex;

        string bir, email;

        cout << "your bir? yyyy-mm-dd\n";
        cin >> bir;

        cout << "your email?\n";
        cin >> email;
        while ( ! email_is_ok(email) )
        {
            cout << "have problem,repeat!\n";
            cin >> email;
        }
        SignIn(name, sex, bir, email);
    }

        
};
int Manager::already_found = 1 ;
void SaveDataToFile()
{
    
    ofstream out("users.txt");
    if (out.is_open())
    {
                
        for (auto& user : user_list)
        {
            
            char ch = '!';
            out << user.name << ch
                << user.sex << ch
                << user.birthday << ch
                << user.email << ch
                << user.uid << ch
                << user.have_coins << "\n";
        }
        out.close();
        //cout << "save successfully";
    }
    else
    {
        cout << "file fails to open";
    }
}
void LoadDataToFile()
{
    

    ifstream in("users.txt");
    string file_line;
    if (in.is_open())
    {
        while (getline(in, file_line) )
        {
            if (file_line == "") break ; // 手动删除users.txt内容后，会剩一个换行。
           
            char ch = '!';
            int head = 0;
            int tail = file_line.find(ch);

            string name = file_line.substr(head, tail - head);
            head = tail + 1;
            tail = file_line.find(ch, head);

            char sex = file_line.substr(head, tail - head)[0];
            head = tail + 1;
            tail = file_line.find(ch, head);

            string bitrhday = file_line.substr(head, tail - head);
            head = tail + 1;
            tail = file_line.find(ch, head);

            string eamil = file_line.substr(head, tail - head);
            head = tail + 1;
            tail = file_line.find(ch, head);

            int uid = stoi(file_line.substr(head, tail - head));
            Manager::already_found = max(Manager::already_found, uid);
            //cout << w << "\n";
            head = tail + 1;
            //tail = file_line.find(ch, head);

            int coins = stoi(file_line.substr(head));
            user_list.push_back(User(name, sex, bitrhday, eamil, uid, coins));
        }
        in.close();
        Manager::already_found ++;    
    }
    else { cout << "Load fails !!"; }
}
void Print_Menu()
{
    cout << "1. 添加用户账户\n2. 输出所有用户账户信息\n3. 删除用户账户\n4. 修改用户硬币数\n5. 排序用户账户\n6. 保存并退出\n";
}
bool IsUidBig(User& a, User& b)
    {
        return a.uid < b.uid;
    }
void  SortBaesdUid()
    {
        sort(user_list.begin(), user_list.end(), IsUidBig);
    }    
/*
* 待补充功能：
*   搜索：
*       年龄搜索
*       邮箱搜索
*   检查
*       邮箱检查
*       日期检查
*   排序：
*       姓名首字母排序

*/
int main()
{   
    Manager manager;
    LoadDataToFile();	
    /*cout << manager.already_found;
    manager.SignIn("Alice", 'F', "1990-05-15", "alice@example.com");
    cout << manager.already_found;
    manager.SignIn("Bob", 'M', "1985-10-20", "bob@example.com");
    cout << manager.already_found;
    manager.SignIn("Charlie", 'M', "1995-03-10", "charlie@example.com");
    cout << manager.already_found <<"\n";*/
    
    int option = 0;    
   
 
    
    while (true)
    {
        Print_Menu();
        cin >> option;
        if (option == 1)
        {
            manager.Ask_AddUser();
            SaveDataToFile();
        }
        if ( option == 2 ) manager.print_all();
        if (option == 3)
        {
            cout << "user 's  name ?：";
            string tname;
            cin >> tname;
            if (manager.Delet_based_name(tname))
                manager.print_all();
            else cout << "user does not exist\n";    
            SaveDataToFile();
        }
        if (option == 4)
        {
            cout << "user 's  name and how much coins ?：";
            string tname;
            int tcoin;
            cin >> tname >> tcoin;
            auto it = manager.ChangeUserCoin(tname, tcoin);
            if (it != user_list.end())
            {
                cout << "修改成功！" << endl;
            }
                 
            else  cout << "user does not exist\n";
            SaveDataToFile();
        }
        if (option == 5)
        {
            manager.sort_based_CoinAge();
            manager.print_all();
        }
        if (option == 6)
        {
            break;
        }
       // system("cls");
    }
    SortBaesdUid();
    SaveDataToFile();
  
    return 0;
}










// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
