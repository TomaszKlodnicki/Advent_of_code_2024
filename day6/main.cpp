#include <iostream>
#include <vector>
#include <stdio.h>
#include <unordered_set>

#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

#define CHOSE_FILENAME USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME

#define RIGHT {1, 0}
#define UP {0, -1}
#define LEFT {-1, 0}
#define DOWN {0, 1}

const int directions[4][2] = {UP, RIGHT, DOWN, LEFT};

struct step{
    int16_t x, y;
    uint8_t dir;

    bool operator==(const step& s) const{
        return x == s.x && y == s.y && dir == s.dir;
    }
};

struct data {
    std::vector<std::vector<char>> map;
    int16_t guardX, guardY;
    uint8_t dir = 0;
    int16_t width, height;
};

template<>
struct std::hash<step>
{
    std::size_t operator()(const step& s) const noexcept
    {
        return  s.dir | (s.x << 8) | (s.y << 24);
    }
};

data loadMap(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s\n", filename);
        exit(1);
    }
;
    data  data;
    std::vector<char> row;
    char readChar;
    do{
        readChar = fgetc(fp);

        if(readChar == '\n' || readChar == EOF){
            data.map.push_back(row);
            row.clear();
        } else{
            if(readChar == '^'){
                data.guardX = row.size();
                data.guardY = data.map.size();
                readChar = '.';
            }
            row.push_back(readChar);
        }

    } while (readChar != EOF);
    
    fclose(fp);

    data.width = data.map[0].size();
    data.height = data.map.size();

    return data;
}


bool moveGuard(data& dMap){

    dMap.map[dMap.guardY][dMap.guardX] = 'X';

    int16_t newX = dMap.guardX + directions[dMap.dir][0];
    int16_t newY = dMap.guardY + directions[dMap.dir][1];

    if(newX < 0 || newX >= dMap.width || newY < 0 || newY >= dMap.height)
        return false;

    if(dMap.map[newY][newX] == '#'){
        dMap.dir = (dMap.dir + 1) % 4;
        return true;
    }

    dMap.guardX = newX;
    dMap.guardY = newY;

    return true;
}

bool checkIfGuardEscapes(data& dMap){

    std::unordered_set<step> steps;

    while(moveGuard(dMap)){

        if(steps.find({dMap.guardX, dMap.guardY, dMap.dir}) != steps.end())
            return false;

        steps.insert({dMap.guardX, dMap.guardY, dMap.dir});

    }

    return true;
}


int puzzle1(data dMap){

    int retValue = 0;

    while(moveGuard(dMap));

    for(int16_t i = 0; i < dMap.height; i++){
        for(int16_t j = 0; j < dMap.width; j++){
            if(dMap.map[i][j] == 'X')
                retValue++;
        }
    }

    return retValue;
}

int puzzle2(data dMap){
    int retValue = 0;

    data firstRun = dMap;

    while(moveGuard(firstRun));

    for(int16_t i = 0; i < firstRun.height; i++){
        for(int16_t j = 0; j < firstRun.width; j++){
            if(firstRun.map[i][j] == 'X' && !(i == dMap.guardY && j == dMap.guardX)){
                data checkMap = dMap;
                checkMap.map[i][j] = '#';

                if(!checkIfGuardEscapes(checkMap))
                    retValue++;
            }
        }
    }

    return retValue;
}

int main(int argc, char* argv[]) {

    printf("Day 6\n");

    data dMap = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %d\n", puzzle1(dMap));
    printf("Puzzle 2: %d\n", puzzle2(dMap));

    return 0;
}