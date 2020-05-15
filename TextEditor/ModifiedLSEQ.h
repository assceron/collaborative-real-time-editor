//
// 23/05/2019.
//

#ifndef LAB3_MODIFIEDLSEQ_H
#define LAB3_MODIFIEDLSEQ_H

#include <vector>
#include <random>

class ModifiedLSEQ {
public:
    ModifiedLSEQ();
    ~ModifiedLSEQ();
    std::vector<unsigned int> alloc(std::vector<unsigned int> pos1, std::vector<unsigned int> pos2, unsigned int siteId);
private:
    //unsigned int _boundary = 10;
    unsigned int levelSize(unsigned int depth);
    std::vector<bool> _forward;
    std::vector<unsigned int> prefix(unsigned int siteId, unsigned int zeroValue, std::vector<unsigned int> pos, unsigned int depth);
    unsigned int calculateInterval(unsigned int siteId, std::vector<unsigned int> pos1, std::vector<unsigned int> pos2, unsigned int depth);
    unsigned int randomNumberInRange(unsigned int from, unsigned int to);
    std::mt19937 _gen;
};

#endif //LAB3_MODIFIEDLSEQ_H
