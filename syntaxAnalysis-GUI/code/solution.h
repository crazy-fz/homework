#ifndef SOLUTION_H
#define SOLUTION_H
#include "trie.h"
#include <QString>
#include <set>
#include <string>

class solution
{
public:
    std::string result[2]={"关键字","运算符号"};
    std::set<char> sp;
    std::set<char> isnum;
    Trie *t;

    /**
     * @param inFileName   输入文件位置
     * @param outFileName  输出文件保存位置
     *
     * @retval 0    success
     * @retval 1    failed
     */
    int work(const QString &inFileName,const QString &outFileName);

    solution();
    virtual ~solution();
private:
    /**
     * @brief 加载qrc里的字典
     * @retval 0    success
     * @retval 1    failed
     */
    int loadQrc();
};

#endif // SOLUTION_H
