#include <iostream>
#include <vector>
#include <stdio.h>
#include <unordered_map>
#include <algorithm>

#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

#define CHOSE_FILENAME USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME


struct data{
    std::unordered_map<int, std::vector<int>> orderList;
    std::vector<std::vector<int>> pagesList;
};

data loadMap(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s\n", filename);
        exit(1);
    }
;
    data oList;
    char* buff = NULL;
    ssize_t len;
    size_t allocatedData = 0;
    while((len = getline(&buff, &allocatedData, fp)) != -1){
        if(*buff == '\n')
            break;
        int prev, next;
        sscanf(buff, "%d|%d", &prev, &next);
        oList.orderList[next].push_back(prev);
    }

    std::vector<int> row;
    int readInt;
    char readC;

    while(fscanf(fp, "%d", &readInt) != EOF){
        row.push_back(readInt);
        readC = fgetc(fp);
        if(readC == '\n' || readC == EOF){
            oList.pagesList.push_back(row);
            row.clear();
            if(readC == EOF)
                break;
        }
    }

    fclose(fp);

    return oList;
}


int checkIfOrderedValue(std::vector<int>& pages, std::unordered_map<int, std::vector<int>>& orderList, int index){        

    if(orderList.find(pages[index]) == orderList.end())
        return 0;

    int retValue = 0;

    for(int j = 0; j < orderList[pages[index]].size(); j++){
        for(int k = index+1; k < pages.size(); k++){
            if(pages[k] == orderList[pages[index]][j]){
                retValue++;
            }
        }
    }

    return retValue;
}

bool checkIfOrdered(std::vector<int>& pages, std::unordered_map<int, std::vector<int>>& orderList){

    for(int i = 0; i < pages.size(); i++){
        if(checkIfOrderedValue(pages, orderList, i) > 0)
            return false;
    }

    return true;
}

void bubbleSort(std::vector<int>& pages, std::unordered_map<int, std::vector<int>>& orderList){
    bool sorted = false;
    while(!sorted){
        sorted = true;
        for(int i = 0; i < pages.size()-1; i++){
            if(checkIfOrderedValue(pages, orderList, i) > checkIfOrderedValue(pages, orderList, i+1)){
                int temp = pages[i];
                pages[i] = pages[i+1];
                pages[i+1] = temp;
                sorted = false;
            }
        }
    }
}

int checkUnordered(std::vector<int>& pages, std::unordered_map<int, std::vector<int>>& orderList){
    if(checkIfOrdered(pages, orderList))
        return 0;

    bubbleSort(pages, orderList);

    return pages[int(pages.size()/2)];
}

int puzzle1(data& oList){

    int retValue = 0;

    for(int i = 0; i < oList.pagesList.size(); i++){
        if(checkIfOrdered(oList.pagesList[i], oList.orderList))
            retValue += oList.pagesList[i][int(oList.pagesList[i].size()/2)];
    }

    return retValue;
}

int puzzle2(data& oList){
    int retValue = 0;

    for(int i = 0; i < oList.pagesList.size(); i++){
        retValue += checkUnordered(oList.pagesList[i], oList.orderList);
    }

    return retValue;
}

int main(int argc, char* argv[]) {

    printf("Day 5\n");

    data oList = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %d\n", puzzle1(oList));
    printf("Puzzle 2: %d\n", puzzle2(oList));

    return 0;
}