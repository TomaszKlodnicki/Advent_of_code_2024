#include <iostream>
#include <vector>
#include <stdio.h>

#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

#define CHOSE_FILENAME USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME

struct equation{
    uint64_t result;
    std::vector<uint64_t> values;
};

std::vector<equation> loadMap(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s\n", filename);
        exit(1);
    }

    std::vector<equation>  data;
    char readChar;
    uint64_t readInt;
    
    while(fscanf(fp, "%llu:", &readInt) != EOF){
        equation eq;
        eq.result = readInt;
        do{
            fscanf(fp, "%llu", &readInt);
            eq.values.push_back(readInt);
            readChar = fgetc(fp);
        }while(readChar != '\n' && readChar != EOF);

        data.push_back(eq);
    }
    
    fclose(fp);

    return data;
}

uint64_t concentration(const uint64_t& a, uint64_t b){

    char buff[40];

    snprintf(buff, 40, "%llu%llu", a, b);
    sscanf(buff, "%llu", &b);

    return b;
}

uint64_t calculateEquation(std::vector<uint64_t>& values, uint16_t possibilities){

    uint64_t result = values[0];

    for(int i = 1; i < values.size(); i++){
        if(possibilities & (1 << (i-1))){
            result += values[i];
        } else {
            result *= values[i];
        }
    }

    return result;
}

uint64_t calculateEquationWithConc(std::vector<uint64_t>& values, uint32_t possibilities){

    uint64_t result = values[0];

    for(int i = 1; i < values.size(); i++){
        switch (possibilities % 3)
        {
            case 0:
                result *= values[i];
                break;

            case 1:
                result += values[i];
                break;
            
            case 2:
                result = concentration(result, values[i]);
                break;
        }
        possibilities = (possibilities - (possibilities % 3)) / 3;
    }

    return result;
}

bool checkEquation(equation& eq){

    uint16_t possibilities = pow(2, eq.values.size()-1);
    for(uint16_t i = 0; i < possibilities; i++){
        if(calculateEquation(eq.values, i) == eq.result){
            return true;
        }
    }

    return false;
}

bool checkEquationWithConc(equation& eq){

    uint32_t possibilities = pow(3, eq.values.size()-1);

    for(uint32_t pos = 0; pos < possibilities; pos++){
        if(calculateEquationWithConc(eq.values, pos) == eq.result){
            return true;
        }
    }

    return false;
}

uint64_t puzzle1(std::vector<equation>& data){

    uint64_t retValue = 0;

    for(equation& eq : data){
        if(checkEquation(eq))
            retValue += eq.result;
    }

    return retValue;
}

uint64_t puzzle2(std::vector<equation>& data){
    uint64_t retValue = 0;

    for(equation& eq : data){
        if(checkEquationWithConc(eq))
            retValue += eq.result;
    }

    return retValue;
}

int main(int argc, char* argv[]) {

    printf("Day 7\n");

    std::vector<equation> data = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %llu\n", puzzle1(data));
    printf("Puzzle 2: %llu\n", puzzle2(data));

    return 0;
}