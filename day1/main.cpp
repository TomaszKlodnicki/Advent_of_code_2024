#include <iostream>
#include <vector>
#include <algorithm>
#include <optional>
#include <unordered_map>

#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

struct dataSet{
    std::vector<int> collOne, collTwo;
};

std::optional<dataSet> readData(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s\n", filename);
        return std::nullopt;
    }

    dataSet data;
    int readOne, readTwo;
    while(fscanf(fp, "%d %d\n", &readOne, &readTwo) != EOF){
        data.collOne.push_back(readOne);
        data.collTwo.push_back(readTwo);
    }   
    fclose(fp);

    printf("Read %lu lines\n", data.collOne.size());

    return data;
}

int puzzle1(dataSet data){

    sort(data.collOne.begin(), data.collOne.end());
    sort(data.collTwo.begin(), data.collTwo.end());

    int outputValue = 0;

    for(int i = 0; i < data.collOne.size(); i++){
        outputValue += abs(data.collOne[i] - data.collTwo[i]);
    }

    return outputValue;
}

int puzzle2(const dataSet& data){

    int outputValue = 0;

    std::unordered_map<int, int> count;

    for(const int& i : data.collTwo){
       count[i]++;
    }

    for(const int& i : data.collOne){
        outputValue += count[i] * i;
    }

    return outputValue;
}

int main(int argc, char* argv[]) {

    std::optional<dataSet> data = readData(USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME);
    if(!data.has_value()){
        return -1;
    }

    printf("Day 1\n");

    printf("Puzzle 1: %d\n", puzzle1(data.value()));
    printf("Puzzle 2: %d\n", puzzle2(data.value()));

    return 0;
}