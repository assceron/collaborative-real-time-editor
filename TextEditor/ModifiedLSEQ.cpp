//
// 23/05/2019.
//

#include "ModifiedLSEQ.h"
#include <random>
#include <algorithm>
#include <functional>
#include <cmath>
#include <iostream>

#define MAX_LV_SZ_EXPONENT 30u

#define PDS_LAB_3_DEBUG_ENABLED false

ModifiedLSEQ::ModifiedLSEQ(){
    _forward = std::vector<bool>();
    std::random_device rd; // obtain a random number from hardware
    _gen = std::mt19937(rd()); // seed the generator
}

unsigned int ModifiedLSEQ::levelSize(unsigned int depth) {
    unsigned int size = (unsigned int)std::pow(2, std::min(MAX_LV_SZ_EXPONENT, 5+(depth/2)));
#if PDS_LAB_3_DEBUG_ENABLED
    std::cout << "levelSize(" << depth << ")" << " -> " << size << std::endl;
#endif
    return size;
}

unsigned int ModifiedLSEQ::randomNumberInRange(unsigned int from, unsigned int to){
    std::uniform_int_distribution<> distr(from, to); // define the range
    return distr(_gen);
}

// pos2 se non esiste deve essere una potenza di 2 che aumenta ogni 2 livelli (i livelli pari contengono i site id)
std::vector<unsigned int> ModifiedLSEQ::alloc(std::vector<unsigned int> pos1, std::vector<unsigned int> pos2, unsigned int siteId){

#if PDS_LAB_3_DEBUG_ENABLED
    std::cout << "pos1: ";
    for(auto iter = pos1.begin(); iter<pos1.end(); iter++){
        std::cout << "[" << (int)*iter << "]";
    }
    std::cout << " pos2: ";
    for(auto iter = pos2.begin(); iter<pos2.end(); iter++){
        std::cout << "[" << (int)*iter << "]";
    }
    std::cout << std::endl;
#endif

    std::vector<unsigned int> newPos;
    unsigned int depth = 0;
    unsigned int interval = calculateInterval(siteId, pos1, pos2, depth);
    while(interval <= 1){ // 0==same position, 1==adjacent o come si scrive.
        depth+=2;
        interval = calculateInterval(siteId, pos1, pos2, depth); // pos2 must be after pos1! (altrimenti esplode tutto)
    }
    unsigned int step = std::min(levelSize(depth)-1, (unsigned int)interval-1);
#if PDS_LAB_3_DEBUG_ENABLED
    std::cout << "\talloc(" << "?, ?, " << siteId << ")" << ".step(" << levelSize(depth)-2 << ", " << interval - 2 << ") = " << step << std::endl;
#endif

    for(unsigned int i = _forward.size(); i <= std::floor(depth/2); i++){
        //auto gen = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine(siteId));
        _forward.push_back(true);//[std::floor(depth/2)]=true; //always boundary+ //gen();
    }

    if(_forward[std::floor(depth/2)]){ // boundary+
        //auto gen = std::bind(std::uniform_int_distribution<>(1,step),std::default_random_engine(siteId));
        unsigned int addVal = randomNumberInRange(1, step);
#if PDS_LAB_3_DEBUG_ENABLED
        std::cout << "\talloc(" << "?, ?, " << siteId << ")" << ".addVal = " << addVal << std::endl;
#endif
        newPos = prefix(siteId, 0/*levelSize(depth)*/, pos1, depth);
        newPos[newPos.size()-1]=(newPos[newPos.size()-1]+addVal)% levelSize(depth);
    }else{ // boundary-
        //auto gen = std::bind(std::uniform_int_distribution<>(1,step),std::default_random_engine(siteId));
        unsigned int subVal = randomNumberInRange(1, step);
        newPos = prefix(siteId, levelSize(depth), pos2, depth);
        newPos[newPos.size()-1]=(newPos[newPos.size()-1]-subVal)% levelSize(depth);
    }
    newPos.push_back(siteId);
#if PDS_LAB_3_DEBUG_ENABLED
    std::cout << "alloc(" << "?, ?, " << siteId << ")" << " -> ";
    for(auto iter = newPos.begin(); iter<newPos.end(); iter++){
        std::cout << "[" << (int)*iter << "]";
    }
    std::cout << std::endl;
#endif
    return newPos;
}

unsigned int ModifiedLSEQ::calculateInterval(unsigned int siteId, std::vector<unsigned int> pos1, std::vector<unsigned int> pos2, unsigned int depth){
    std::vector<unsigned int> pref1 = prefix(siteId, 0/*levelSize(depth)*/, pos1, depth);
    std::vector<unsigned int> pref2 = prefix(siteId, levelSize(depth), pos2, depth);
    unsigned int interval = std::max((unsigned int)0, (unsigned int)(pref2[depth] - pref1[depth]/*(pref1[depth]+1)%levelSize(depth)*/));
#if PDS_LAB_3_DEBUG_ENABLED
    std::cout << "calculateInterval(" << "?, ?, " << depth << ") -> " << interval << std::endl;
#endif
    return interval;
}

std::vector<unsigned int> ModifiedLSEQ::prefix(
        unsigned int siteId,
        unsigned int zeroValue,
        std::vector<unsigned int> pos, unsigned int depth) {
    std::vector<unsigned int> newPos = std::vector<unsigned int>();
    for(unsigned int d = 0; d<=depth; d+=2){ // first row: pos; second row: siteId.
        if(d>=2){
            if(d-1<pos.size()){
                newPos.push_back(pos[d-1]);
            }else{
                newPos.push_back(siteId);
            }
        }
        if(d<pos.size()){
            newPos.push_back(pos[d]);
        }else{
            newPos.push_back((d+2<=depth)?0:zeroValue);
        }
    }
#if PDS_LAB_3_DEBUG_ENABLED
    std::cout << "prefix(" << zeroValue << ", ?, " << depth << ")" << " -> ";
    for(auto iter = newPos.begin(); iter<newPos.end(); iter++){
        std::cout << "[" << (int)*iter << "]";
    }
    std::cout << std::endl;
#endif
    return newPos;
}

ModifiedLSEQ::~ModifiedLSEQ() {
    //std::cout << "~ModifiedLSEQ()" << std::endl;
    _forward.clear();
}