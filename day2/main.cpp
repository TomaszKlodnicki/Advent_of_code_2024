#include <iostream>
#include <vector>
#include <algorithm>
#include <optional>

#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

#define CHOSE_FILENAME USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME

#define MAX_INT 0x7FFFFFFF
#define MAX_DIFF 3
#define MIN_DIFF 1

bool checkIfVecOk(std::vector<int>& input, int* failIndex){
    int lastDiff = 0;
    for(int i = 1; i < input.size(); i++){
        int diff = input[i] - input[i-1];
        int absVal = abs(diff);
        if(absVal > MAX_DIFF || absVal < MIN_DIFF || lastDiff * diff < 0){
            *failIndex = i;
            return false;
        }
        lastDiff = diff;
    }
    return true;
}

int puzzle1(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s\n", filename);
        return -1;
    }

    int readInt = MAX_INT;
    std::vector<int> values;
    int retValue = 0;
    int failIndex = 0;
    
    while (fscanf(fp, "%d", &readInt) != EOF) {
        char nextChar = fgetc(fp);

        values.push_back(readInt);

        if(nextChar == EOF || nextChar == '\0' || nextChar == '\n'){
            if(checkIfVecOk(values, &failIndex))
                retValue++;

            values.clear();

            if(nextChar == EOF)
                break;
        }
    }
    fclose(fp);

    return retValue;
}

int puzzle2(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s\n", filename);
        return -1;
    }

    int readInt = MAX_INT;
    std::vector<int> values;
    int retValue = 0;
    int failIndex = 0;
    
    while (fscanf(fp, "%d", &readInt) != EOF) {
        char nextChar = fgetc(fp);

        values.push_back(readInt);

        if(nextChar == EOF || nextChar == '\0' || nextChar == '\n'){
            if(checkIfVecOk(values, &failIndex))
                retValue++;
            else{
                for(; failIndex >= 0; failIndex--){
                    std::vector<int> valuesCopy = values;
                    int temp;
                    valuesCopy.erase(valuesCopy.begin() + failIndex);
                    if(checkIfVecOk(valuesCopy, &temp)){
                        retValue++;
                        break;
                    }
                }
            }

            values.clear();

            if(nextChar == EOF)
                break;
        }
    }
    fclose(fp);

    return retValue;
}

int main(int argc, char* argv[]) {

    printf("Day 2\n");

    printf("Puzzle 1: %d\n", puzzle1(CHOSE_FILENAME));
    printf("Puzzle 2: %d\n", puzzle2(CHOSE_FILENAME));

    return 0;
}