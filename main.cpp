#include <iostream>
//
// Created by hzl on 2019/4/18.
//

#include <sstream>
#include <variant>
#include <vector>
#include <map>
#include "algorithm"
#include "unistd.h"

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

int main()
{
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
