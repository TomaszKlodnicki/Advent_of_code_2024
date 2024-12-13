#include <iostream>
#include <vector>
#include <stdio.h>
#include <unordered_set>

#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

#define CHOSE_FILENAME USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME

struct data {
    std::vector<std::vector<char>> map;
    int16_t width, height;
};

struct Position {
    int16_t x, y;

    Position operator+(const Position& p) const{
        Position newPos;
        newPos.x = x + p.x;
        newPos.y = y + p.y;
        return newPos;
    }

    bool operator==(const Position& p) const{
        return x == p.x && y == p.y;
    }
};

template<>
struct std::hash<Position> {
    std::size_t operator()(const Position& p) const noexcept
    {
        return p.x | (p.y << 16);
    }
};

enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct Fence {
    std::unordered_set<Position> UP;
    std::unordered_set<Position> DOWN;
    std::unordered_set<Position> LEFT;
    std::unordered_set<Position> RIGHT;
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
            row.push_back(readChar);
        }

    } while (readChar != EOF);
    
    fclose(fp);

    data.width = data.map[0].size();
    data.height = data.map.size();

    return data;
}

bool inBorder(int16_t x, int16_t y, const data& pMap){
    return x >= 0 && y >= 0 && x < pMap.width && y < pMap.height;
}

void findArea(int16_t h, int16_t w, const data& pMap, bool** takenTab, int& area, int& perimeter){

    if(takenTab[h][w])
        return;

    char plant = pMap.map[h][w];

    area++;
    takenTab[h][w] = true;

    if(inBorder(w, h + 1, pMap) && pMap.map[h + 1][w] == plant){
        findArea(h + 1, w, pMap, takenTab, area, perimeter);
    } else {
        perimeter++;
    }


    if(inBorder(w, h - 1, pMap) && pMap.map[h - 1][w] == plant){
        findArea(h - 1, w, pMap, takenTab, area, perimeter);
    } else {
        perimeter++;
    }


    if(inBorder(w + 1, h, pMap) && pMap.map[h][w + 1] == plant){
        findArea(h, w + 1, pMap, takenTab, area, perimeter);
    } else {
        perimeter++;
    }


    if(inBorder(w - 1, h, pMap) && pMap.map[h][w - 1] == plant){
        findArea(h, w - 1, pMap, takenTab, area, perimeter);
    } else {
        perimeter++;
    }

}

void findFence(int16_t h, int16_t w, const data& pMap, bool** takenTab, int& area, Fence& fences){

    if(takenTab[h][w])
        return;

    char plant = pMap.map[h][w];

    area++;
    takenTab[h][w] = true;

    if(inBorder(w, h + 1, pMap) && pMap.map[h + 1][w] == plant){
        findFence(h + 1, w, pMap, takenTab, area, fences);
    } else {
        fences.DOWN.insert({w, h});
    }

    if(inBorder(w, h - 1, pMap) && pMap.map[h - 1][w] == plant){
        findFence(h - 1, w, pMap, takenTab, area, fences);
    } else {
        fences.UP.insert({w, h});
    }

    if(inBorder(w + 1, h, pMap) && pMap.map[h][w + 1] == plant){
        findFence(h, w + 1, pMap, takenTab, area, fences);
    } else {
        fences.RIGHT.insert({w, h});
    }

    if(inBorder(w - 1, h, pMap) && pMap.map[h][w - 1] == plant){
        findFence(h, w - 1, pMap, takenTab, area, fences);
    } else {
        fences.LEFT.insert({w, h});
    }

}

void popInDirection(std::unordered_set<Position>& directionSet, std::unordered_set<Position>::iterator it, const Position move, bool eraseThis){

    Position toFind = *it + move;
    std::unordered_set<Position>::iterator next = directionSet.find(toFind);

    if(next != directionSet.end())
        popInDirection(directionSet, next, move, true);

    if(eraseThis)
        directionSet.erase(it);
}

int checkIfInRow(std::unordered_set<Position>& directionSet, Direction dir){
    if (directionSet.size() < 1)
        return 0;

    std::unordered_set<Position>::iterator it = directionSet.begin();

    switch (dir){
        case Direction::UP:
        case Direction::DOWN:
            popInDirection(directionSet, it, {1, 0}, false);
            popInDirection(directionSet, it, {-1, 0}, true);
            break;


        case Direction::LEFT:
        case Direction::RIGHT:
            popInDirection(directionSet, it, {0, -1}, false);
            popInDirection(directionSet, it, {0, 1}, true);
            break;
    }

    return 1;
}

int computeFenceCost(int16_t h, int16_t w, const data& pMap, bool** takenTab){

    int area = 0, perimeter = 0;
    Fence fences;
    findFence(h, w, pMap, takenTab, area, fences);

    while(checkIfInRow(fences.UP, Direction::UP))
        perimeter++;

    while(checkIfInRow(fences.DOWN, Direction::DOWN))
        perimeter++;

    while(checkIfInRow(fences.LEFT, Direction::LEFT))
        perimeter++;

    while(checkIfInRow(fences.RIGHT, Direction::RIGHT))
        perimeter++;

    return area * perimeter;

}

int puzzle1(data pMap){

    int retValue = 0;

    bool** taken = (bool**)malloc(pMap.height * sizeof(bool*));
    for(int i = 0; i < pMap.height; i++)
        taken[i] = (bool*)calloc(pMap.width, sizeof(bool));
 

    for(int h = 0; h < pMap.height; h++){
        for(int w = 0; w < pMap.width; w++){
            if(!taken[h][w]){
                int area = 0, perimeter = 0;
                findArea(h, w, pMap, taken, area, perimeter);
                retValue += area * perimeter;
            }
        }
    }

    for(int i = 0; i < pMap.height; i++)
        free(taken[i]);
    free(taken);

    return retValue;
}

int puzzle2(data pMap){

    int retValue = 0;

    bool** taken = (bool**)malloc(pMap.height * sizeof(bool*));
    for(int i = 0; i < pMap.height; i++)
        taken[i] = (bool*)calloc(pMap.width, sizeof(bool));
 

    for(int h = 0; h < pMap.height; h++){
        for(int w = 0; w < pMap.width; w++){
            if(!taken[h][w]){
                retValue += computeFenceCost(h, w, pMap, taken);
            }
        }
    }

    for(int i = 0; i < pMap.height; i++)
        free(taken[i]);
    free(taken);

    return retValue;
}

int main(int argc, char* argv[]) {

    printf("Day 12\n");

    data pMap = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %d\n", puzzle1(pMap));
    printf("Puzzle 2: %d\n", puzzle2(pMap));

    return 0;
}