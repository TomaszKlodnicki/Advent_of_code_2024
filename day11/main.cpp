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

int puzzle1(std::list<uint64_t> list){

    return blink(25, list);
}

uint64_t puzzle2(std::list<uint64_t> list){

    uint64_t retValue = 0;

    blink(25, list);

    std::unordered_map<uint64_t, uint64_t> computedValues50;
    std::unordered_map<uint64_t, uint64_t> computedValues25;

    for(auto itOne = list.begin(); itOne != list.end(); itOne++){
        if(computedValues50.find(*itOne) != computedValues50.end()){
            retValue += computedValues50[*itOne];
            continue;
        }
        std::list<uint64_t> newList = {*itOne};
        blink(25, newList);
        uint64_t ret50 = 0;
        for(auto it = newList.begin(); it != newList.end(); it++){
            if(computedValues25.find(*it) != computedValues25.end()){
                ret50 += computedValues25[*it];
                continue;
            }
            std::list<uint64_t> newNewList = {*it};
            uint64_t ret = blink(25, newNewList);
            computedValues25[*it] = ret;
            ret50 += ret;
        }
        computedValues50[*itOne] = ret50;
        retValue += ret50;
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