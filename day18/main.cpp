#include <iostream>
#include <vector>
#include <stdio.h>

#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

#define CHOSE_FILENAME USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME

constexpr const int maxMem(){ return USE_EXAMPLE ? 7 : 71;}
constexpr const uint32_t fallIter(){ return USE_EXAMPLE ? 12 : 1024;}

#define MAX_MEM maxMem()
#define FALL_ITER fallIter()

struct Position{
    int32_t x, y;

    Position operator+(const Position& p) const{
        Position newPos;
        newPos.x = x + p.x;
        newPos.y = y + p.y;
        return newPos;
    }

    Position operator+(int32_t p) const{
        Position newPos;
        newPos.x = x + p;
        newPos.y = y + p;
        return newPos;
    }

    Position operator-(const Position& p) const{
        Position newPos;
        newPos.x = x - p.x;
        newPos.y = y - p.y;
        return newPos;
    }

    Position operator*(int32_t i) const{
        Position newPos;
        newPos.x = x * i;
        newPos.y = y * i;
        return newPos;
    }

    bool operator==(const Position& p) const{
        return x == p.x && y == p.y;
    }

    bool operator!=(const Position& p) const{
        return x != p.x || y != p.y;
    }
};

enum class Direction : int {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
};

Direction operator+(const Direction& d, int i){
    Direction dir = static_cast<Direction>((static_cast<int>(d) + (i % 4) + 4) % 4);
    return dir;
}

Direction operator!(const Direction& d){
    return d + 2;
}

constexpr Position dirPos(Direction dir){
    switch(dir){
        case Direction::UP:
            return {0, -1};
        case Direction::DOWN:
            return {0, 1};
        case Direction::LEFT:
            return {-1, 0};
        case Direction::RIGHT:
            return {1, 0};
    }
}

struct Map{
    std::vector<std::vector<char>> map = std::vector<std::vector<char>>(MAX_MEM, std::vector<char>(MAX_MEM, '.'));
     std::vector<std::vector<uint32_t>> values = std::vector<std::vector<uint32_t>>(MAX_MEM, std::vector<uint32_t>(MAX_MEM, UINT32_MAX));
    std::vector<Position> fallingBytes;

    char& operator[](Position p){
        return map[p.y][p.x];
    }
};

const Position startPos = {0, 0};
const Position endPos = {MAX_MEM-1, MAX_MEM-1};

Map loadMap(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s\n", filename);
        exit(1);
    }

    Map data;
    Position readData;

    while(fscanf(fp, "%d, %d\n", &readData.x, &readData.y) != EOF)
        data.fallingBytes.push_back(readData);
        

    fclose(fp);

    return data;
}

void fall(Map& map, uint32_t iterations){
    for(uint32_t i = 0; i < iterations; i++){
        map[map.fallingBytes[i]] = '#';
    }
}

bool inBorder(const Map& map, Position p){
    return p.x >= 0 && p.y >= 0 && p.x < MAX_MEM && p.y < MAX_MEM;
}

void move(Map& map, Position pos){

    uint32_t value = map.values[pos.y][pos.x] + 1;
    for(Direction dir : {Direction::UP, Direction::RIGHT, Direction::DOWN, Direction::LEFT}){
        Position newPos = pos + dirPos(dir);
        if(inBorder(map, newPos) && map[newPos] != '#' && map.values[newPos.y][newPos.x] > value){
            map.values[newPos.y][newPos.x] = value;
            move(map, newPos);
        }
    }

}

uint32_t puzzle1(Map map){

    fall(map, FALL_ITER);
    map.values[startPos.y][startPos.x] = 0;
    move(map, startPos);

    return map.values[endPos.y][endPos.x];
}

uint32_t puzzle2(Map map){

    fall(map, FALL_ITER);
    map.values[startPos.y][startPos.x] = 0;

    for(uint32_t i = FALL_ITER; i < map.fallingBytes.size(); i++){
        map[map.fallingBytes[i]] = '#';
        move(map, startPos);
        if(map.values[endPos.y][endPos.x] == UINT32_MAX)
            return i;

        map.values = std::vector<std::vector<uint32_t>>(MAX_MEM, std::vector<uint32_t>(MAX_MEM, UINT32_MAX));
        map.values[startPos.y][startPos.x] = 0;
    }

    return 0;
}

int main(int argc, char* argv[]) {

    printf("Day 18\n");

    Map map = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %u\n", puzzle1(map));
    uint32_t p2 = puzzle2(map);
    printf("Puzzle 2: %u -> %d,%d\n", p2, map.fallingBytes[p2].x, map.fallingBytes[p2].y);

    return 0;
}