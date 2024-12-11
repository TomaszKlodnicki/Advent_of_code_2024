#include <iostream>
#include <vector>
#include <stdio.h>
#include <list>
#include <optional>
#include <unordered_map>

#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

#define CHOSE_FILENAME USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME

struct RC {
    uint64_t rock;
    unsigned int blinkCount;

    bool operator==(const RC& other) const {
        return rock == other.rock && blinkCount == other.blinkCount;
    }
};

template<>
struct std::hash<RC> {
    std::size_t operator()(const RC& rc) const noexcept
    {
        return std::hash<uint64_t>()(rc.rock) ^ std::hash<unsigned int>()(rc.blinkCount);
    }
};

std::list<uint64_t> loadMap(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s\n", filename);
        exit(1);
    }

    std::list<uint64_t> data;
    uint64_t readData;
    while(fscanf(fp, "%llu", &readData) != EOF){
        data.push_back(readData);
    }
    
    fclose(fp);

    return data;
}

std::optional<std::pair<uint64_t, uint64_t>> evenDigits(uint64_t number){
    char buff[100];
    snprintf(buff, 100, "%llu", number);

    size_t len = strlen(buff);

    if(len % 2 != 0)
        return std::nullopt;

    std::pair<uint64_t, uint64_t> retValue;

    sscanf(buff + len/2, "%llu", &retValue.second);
    *(buff + len/2) = 0;
    sscanf(buff, "%llu", &retValue.first);

    return retValue;
}

int blink(unsigned int blinkCount, std::list<uint64_t>& list){

    for(unsigned int i = 0; i < blinkCount; i++){
        for(auto it = list.begin(); it != list.end(); it++){
            std::optional<std::pair<uint32_t, uint32_t>> eD;
            if(*it == 0){
                *it = 1;
            } else if((eD = evenDigits(*it)) != std::nullopt){
                list.insert(it, eD.value().first);
                *it = eD.value().second;
            } else {
                *it *= 2024;
            }
        }

    }

    return list.size();

}

uint64_t computeRock(uint64_t rock, unsigned int blinkCount){

    if(blinkCount == 0){
        return 1;
    }

    static std::unordered_map<RC, uint64_t> computedValues;
    if(computedValues.find({rock, blinkCount}) != computedValues.end()){
        return computedValues[{rock, blinkCount}];
    }

    uint64_t retValue = 0;
    
    if(rock == 0){
        retValue = computeRock(1, blinkCount - 1);
    }else{
        std::optional<std::pair<uint32_t, uint32_t>> eD = evenDigits(rock);
        if(eD != std::nullopt){
            retValue = computeRock(eD.value().first, blinkCount - 1) + computeRock(eD.value().second, blinkCount - 1);
        } else {
            retValue = computeRock(rock * 2024, blinkCount - 1);
        }
    }
    computedValues[{rock, blinkCount}] = retValue;
    return retValue;
}

int puzzle1(std::list<uint64_t> list){

    return blink(25, list);
}

uint64_t puzzle2(std::list<uint64_t> list){

    uint64_t retValue = 0;

    for (auto it = list.begin(); it != list.end(); it++){
        retValue += computeRock(*it, 75);
    }

    return retValue;
}

int main(int argc, char* argv[]) {

    printf("Day 11\n");

    std::list<uint64_t> list = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %d\n", puzzle1(list));
    printf("Puzzle 2: %llu\n", puzzle2(list));

    return 0;
}