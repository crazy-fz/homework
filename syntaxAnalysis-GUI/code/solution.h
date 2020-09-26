#ifndef SOLUTION_H
#define SOLUTION_H
#include "trie.h"
#include <QString>
#include <set>
#include <string>

class solution
{
public:

    /**
     * @param s   source code
     *
     * @return    analysis
     */
    std::string work(const std::string &s);

    /**
     * @return   compressed code
     */
    std::string compressCode(const std::string &s);

    solution();
    virtual ~solution();

protected:
    const std::string result[2]={"关键字","运算符号"};
    std::set<char> sp; // special words
    std::set<char> isnum; // numberic words
    Trie *t;

private:
    /**
     * @brief load dic files in qrc
     * @retval 0    success
     * @retval 1    failed
     */
    int loadQrc();
};

#endif // SOLUTION_H
