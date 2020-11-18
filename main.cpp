#include <iostream>
//
// Created by hzl on 2019/4/18.
//

#include <sstream>
#include <variant>
#include <vector>
#include <map>
#include "math.h"
#include <netinet/in.h>
#include <unordered_map>
#include "algorithm"
#include "unistd.h"
#include "memory"
#include "list"
#include "common_utf8handle.h"

template<class... Ts> struct overloaded_function : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded_function(Ts...) -> overloaded_function<Ts...>;

enum EMysqlDupKeyUpdateType
{
    EMDKUT_Null = 0,    // 不更新
    EMDKUT_Set,         // 更新为新值
    EMDKUT_Sum,         // 求和
};

struct TMysqlColumn
{
    std::string             m_strColumnName;
    std::variant<int32_t, std::string>  m_varValue;
    EMysqlDupKeyUpdateType  m_eType;
};

std::string build_insert_or_update_escape_sql(const std::string &strTableName, const std::vector<TMysqlColumn> &stColumnVec)
{
    std::ostringstream strColumnNames;
    std::ostringstream strColumnValues;
    std::ostringstream strColumnUpdates;

    overloaded_function fcBuildValue{
            [&](const int32_t& value) {strColumnValues << value;},
            [&](const std::string& value) {strColumnValues << "'" << value << "'";}
    };

    overloaded_function fcUpdateValue{
            [&](const int32_t& value) {strColumnUpdates << value;},
            [&](const std::string& value) {strColumnUpdates << "'" << value << "'";}
    };

    std::string strBeginComma;
    for (const auto& column : stColumnVec)
    {
        strColumnNames << strBeginComma;
        strColumnValues << strBeginComma;

        strColumnNames << "`" << column.m_strColumnName << "`";
        std::visit(fcBuildValue, column.m_varValue);

        switch(column.m_eType)
        {
            case EMDKUT_Set:
                if(!strColumnUpdates.str().empty())
                    strColumnUpdates << strBeginComma;
                strColumnUpdates << "`" << column.m_strColumnName << "` = ";
                std::visit(fcUpdateValue, column.m_varValue);
                break;

            case EMDKUT_Sum:
                if(std::get_if<int32_t>(&column.m_varValue) == nullptr)
                {
                    return "";
                }

                if(!strColumnUpdates.str().empty())
                    strColumnUpdates << strBeginComma;
                strColumnUpdates << "`" << column.m_strColumnName << "` = `" << column.m_strColumnName << "` + ";
                std::visit(fcUpdateValue, column.m_varValue);
                break;

            default:
                break;
        }

        strBeginComma = ", ";
    }

    std::ostringstream os;
    os << "insert into " << strTableName << " (" << strColumnNames.str() << ") values (" << strColumnValues.str() << ") ON DUPLICATE KEY UPDATE " << strColumnUpdates.str() << ";";
    return os.str();
}

template<typename T>
static std::string int2bin(T iNumber)
{
    if (iNumber == 0) return std::string("0");

    int iPosition = 0;
    const size_t uiSize = sizeof(T) * 8;
    std::string strRet(uiSize, '0');
    for (size_t i = 0; i < uiSize; ++i)
    {
        if (iNumber & ((T) 0x1 << i))
        {
            iPosition = uiSize - 1 - i;
            strRet[iPosition] = '1';
        }
    }
    return strRet.substr(iPosition);
}

class BmwCar
{
public:
    void print() { std::cout << "jadksjflkja" << std::endl; }
};

static BmwCar* CreateCar(std::string strCardName)
{
    if ("Bmw" == strCardName) {
        return new BmwCar();
    }

    return nullptr;
}

enum EType
{
    ET_Invalid = 0,
    ET_One = 1,
    // ET_Two = 2,
    // ET_Three = 3,
    ET_Four = 4,
    ET_Five = 5,

    ET_Max,
};

class CFather
{
public:
    CFather() {}

    int a;
};

class CChild : public CFather
{
public:
    CChild() {}

    int b;
};

void FuncFather(const CFather& ff)
{
     const CChild& cc = static_cast<const CChild&>(ff);
     // cc.b = 2;
}

struct CS
{
    int a = 0;
    int b = 0;
    std::vector<int> c;
};

void FuncArray(int a[])
{
    a[1] = 1;
}

int main()
{
    int a[6] = {0};
    FuncArray(a);
    for(int i = 0; i < 5; ++i)
        std::cout << a[i] << std::endl;

    std::vector<CS> tt;
    std::cout << "-----" << tt.size() << std::endl;


    /*
    std::string strCont = "阿斯f啊,，";
    std::vector<uint32_t> stTalkContent;
    CUTF8Handle::Decode(strCont, stTalkContent);
    std::cout << stTalkContent.size() << std::endl;

    double dd = 0;
    std::cout << std::ceil(dd) << std::endl;

    std::multimap<int, int> stMap;
    std::cout << stMap.size() << std::endl;
    stMap.emplace(1, 1);
    std::cout << stMap.size() << std::endl;
    stMap.emplace(1, 2);
    std::cout << stMap.size() << std::endl;
    stMap.emplace(1, 2);
    std::cout << stMap.size() << std::endl;

    for(auto& iter : stMap)
    {
        std::cout << iter.first << "---" << iter.second << std::endl;
    }
*/


    /*
    std::string str = "";
    std::vector<int> aa = {2, 1, 5};
    for(auto a : aa)
    {
        str += 'A' + a - 1;
    }

    std::cout << 'A' + 0 << std::endl;
    std::cout << str << std::endl;

    std::string strCh = "啊发。";
    std::cout << strCh.size() << std::endl;
     */


    /*
    enum TEnum
    {
        EE_Invalid = 0,
        EE_One = 1,
        EE_Two = 5,
        EE_Max,
        EE_Cur = EE_Max - 1,
    };

    std::cout << EE_Max << "---" << EE_Cur << std::endl;

    for(int i=0; i<10; ++i)
    {
        std::vector<int> st;
        st.push_back(1);
        std::cout << st.size() << std::endl;
    }
     */

    /*
    CS cc;
    cc.a = 213;
    std::map<int, CS> m_stMap;
    m_stMap[1] = std::move(cc);
    auto p = &cc;

    printf("%d %d---\n", m_stMap[1].a, p->a);

    CFather ff;
    int c, d;

    printf("%d %d\n", ff.a, d);

    std::string str = "Hello";
    std::vector<std::string> v;
    //调用常规的拷贝构造函数，新建字符数组，拷贝数据
    v.push_back(str);
    std::cout << "After copy, str is \"" << str << "\"\n";
    //调用移动构造函数，掏空str，掏空后，最好不要使用str
    v.push_back(std::move(str));
    std::cout << "After move, str is \"" << str << "\"\n";
    std::cout << "The contents of the vector are \"" << v[0]
              << "\", \"" << v[1] << "\"\n";
              */


    /*
    int a = 10;
    int b = 0;

    a = a++;
    // b = a++ + a++;

    printf("%d %d\n", a, b);
     */


    /*
    std::unordered_map<int, int> mm;
    auto iter = mm.begin();
    iter->first;

    auto pCar = CreateCar("Bmw");
    pCar->print();

    std::map<int32_t, int32_t> stMap;
    stMap[1];
    ++stMap[2];
    ++stMap[2];
    for(auto i : stMap)
    {
        // printf("%d--%d\n", i.first, i.second);
    }

    std::cout << ET_Max << std::endl;
*/

    /*
    std::string str = "1_1";
    int i = ::atoi(str.c_str());
    std::cout << i << std::endl;

    std::cout << std::numeric_limits<int32_t>::min() << std::endl;
*/

    /*
    std::map<int32_t, std::map<int, int>> stMap;
    std::cout << stMap.size() << std::endl;
    if(stMap[1][2] >= 0)
        std::cout << "ffffffff" << std::endl;
    else
        std::cout << "dddddddd" << std::endl;

    std::cout << stMap.size() << std::endl;
    std::cout << stMap[1].size() << std::endl;
    std::cout << stMap[1][2] << std::endl;
*/

    /*
    std::list<int> stList;
    stList.push_back(1);
    stList.push_back(2);
    stList.push_back(3);

    auto iterBegin = stList.begin();
    auto iterEnd = stList.end();
    int i = 1;
    for(; iterBegin != iterEnd; ++iterBegin)
    {
        if(*iterBegin == 2)
        {
            stList.insert(iterBegin, 9);

            std::cout << *iterBegin << std::endl;
        }

        if(*iterBegin == 9)
        {
            std::cout << *iterBegin << std::endl;
        }

        if(*iterBegin == 3)
        {
            stList.insert(iterBegin, 9);

            std::cout << *iterBegin << std::endl;
        }

        std::cout << "=>" << i++ << std::endl;
    }

    std::cout << "-----------" << std::endl;

    for(auto iter : stList)
        std::cout << iter << std::endl;
*/


    /*
    std::vector<int> stVec;
    stVec.push_back(0);
    stVec.push_back(1);
    stVec[1] = 2;
    for(int i = 0; i < stVec.size(); ++i)
        std::cout << i << "---" << stVec[i] << std::endl;

    stVec.resize(5);
    for(int i = 0; i < stVec.size(); ++i)
        std::cout << i << "---" << stVec[i] << std::endl;

    stVec[3] = 2;
    stVec.resize(8);
    for(int i = 0; i < stVec.size(); ++i)
        std::cout << i << "---" << stVec[i] << std::endl;
*/

    /*
    std::shared_ptr<TMysqlColumn> ff(nullptr);

    std::cout<< (ff.get() == nullptr) << std::endl;
*/

    /*
    std::vector<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    for(auto iter = list.begin(); iter != list.end();)
    {
        if(3 == *iter)
        {
            // list.erase(iter++);  崩溃  !!! 顺序容器（vector, queue）erase之后元素会自动前移，iter++会出错导致程序崩溃
            iter = list.erase(iter);
        }
        else
            ++iter;
    }

    for(auto i : list)
        std::cout << i << std::endl;
*/

    /*
    for(int i=0; i<=3; i++)
    {
        for(int j=0; j<=13; j++)
        {
            for(int m=0; m<=2; m++)
            {
                if(i+j+m == 5 && i*12+j*13+m*14 <= 65)
                    std::cout << i << "--" << j << "--" << m << std::endl;
            }
        }
    }
     */

    /*
    bool bTest = false;
    for(int i=0; i<5; ++i)
    {
        bool b = (rand()%2 ? false : true);
        bTest += b;
        std::cout << b << "-----" << bTest << std::endl;
    }
     */


    /*
    int flag = 0;
    pid_t pId = fork();
    if (pId == -1)
    {
        perror("fork error");
        exit(EXIT_FAILURE);
    }
    else if (pId == 0)
    {
        int myPid = getpid();
        int parentPid = getppid();
        printf("Child:SelfID=%d ParentID=%d \n", myPid, parentPid);
        flag = 123;
        printf("Child:flag=%d %p \n", flag, &flag);
        int count = 0;
        do{
            count ++;
            sleep(1);
            printf("Child count=%d \n", count);
            if (count >= 5)
            {
                break;
            }
        }while (1);

        return EXIT_SUCCESS;
    }
    else
    {
        printf("Parent:SelfID=%d MyChildPID=%d \n", getpid(), pId);
        flag = 456;
        printf("Parent:flag=%d %p \n", flag, &flag); // 连地址都一样,说明是真的完全拷贝,但值已经是不同的了..
        int count = 0;
        do{
            count ++;
            sleep(1);
            printf("Parent count=%d \n", count);
            if (count >= 2)
            {
                break;
            }
        }while (1);
    }

    sleep(5);
    return EXIT_SUCCESS;
*/

    /*
    int iTag = 0;
    iTag = iTag | (1 << 2);
    std::cout << iTag << std::endl;
    std::string ss;
    ss = int2bin<int32_t>(iTag);
    std::cout << ss << std::endl;

    std::cout << ((uint) 0x1) << std::endl;
    std::cout << ((int) 0x1 << 2) << std::endl;
*/

    /*
    std::string str("abc,def,hjk");
    std::cout << std::string(str.begin(), str.end()) << std::endl;

    std::cout << std::string(str.rbegin(), str.rend()) << std::endl;

    std::string::iterator iter = std::find(str.begin(), str.end(), ',');
    std::cout << std::string(str.begin(), iter) << std::endl;

    std::cout << "==============" << std::endl;

    std::string::reverse_iterator riter = std::find(str.rbegin(), str.rend(), ',');
    std::cout << *riter << std::endl;
    std::cout << *(riter.base()) << std::endl;
    std::cout << std::string(str.rbegin(), riter) << std::endl;

    std::cout << std::string(riter.base(), str.end()) << std::endl;
*/

    /*
    std::map<int, int> mc, m;

    int min;
    int max;

    for(int i=1; i<5; i++)
    {
        mc.insert(std::make_pair(5, 5555));
        m.insert(std::make_pair(5, 5555));
    }

    std::cin >> min >> max;

    std::cout << min << "---" << max << std::endl;

    auto iter = m.lower_bound(min);
    std::cout << iter->first << " " << iter->second << std::endl;

    ++iter;
    std::cout << iter->first << " " << iter->second << std::endl;

    iter = m.upper_bound(max);
    std::cout << iter->first << " " << iter->second << std::endl;

    std::cout << "**********************************" << std::endl;

    for(auto iter = m.lower_bound(min); iter != m.upper_bound(max); ++iter)
    {
        std::cout << iter->first << " " << iter->second << std::endl;
    }

    std::cout << "**********************************" << std::endl;

    for (const auto& c : m) {
        std::cout << c.first << " " << c.second << std::endl;
    }
*/

    /*
    std::map<int, int> aa = {
            {1, 111111111},
            {2, 222222222},
    };

    for (const auto& c : aa) {
        std::cout << c.first << " " << c.second << std::endl;
    }

    auto num = aa[3];
    std::cout << num << std::endl;

    for (const auto& c : aa) {
        std::cout << c.first << " " << c.second << std::endl;
    }
*/

    /*
    int sum;
    std::cin >> sum;

    std::string sum_column_name = (sum > 0 ? "sum_output_value" : "sum_consume_value");
    std::string str = build_insert_or_update_escape_sql("stat_item",
                                                 {
                                                         {"stat_date", "2019-07-30 10:21:21"},
                                                         {"platform", "jaksf"},
                                                         {"server_id", 1},
                                                         {"cmd", 1111},
                                                         {"item_type", 421},
                                                         {"item_id", 287481},
                                                         {sum_column_name, sum, EMDKUT_Sum}
                                                 });

    if(!str.empty())
        std::cout << str << std::endl;

*/
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
