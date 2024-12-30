#include <iostream>
#include <vector>
#include <stdio.h>
#include <unordered_map>

#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

#define CHOSE_FILENAME USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME

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
    std::vector<std::vector<char>> map;
    std::vector<std::vector<uint32_t>> values;
    int16_t width, height;
    Position start, end;

    char& operator[](Position p){
        return map[p.y][p.x];
    }
};

Map loadMap(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s\n", filename);
        exit(1);
    }

    Map data;
    std::vector<char> row;
    char readChar;

    while((readChar = fgetc(fp)) != EOF){

        switch (readChar)
        {
        case '\n':
            data.map.push_back(row);
            row.clear();
            break;

        case 'S':
            data.start = (Position){static_cast<int32_t>(row.size()), static_cast<int32_t>(data.map.size())};
            row.push_back('S');
            break;

        case 'E':
            data.end = (Position){static_cast<int32_t>(row.size()), static_cast<int32_t>(data.map.size())};
        
        default:
            row.push_back(readChar);
            break;
        }   
    }

    data.map.push_back(row);

    data.height = data.map.size();
    data.width = data.map[0].size();
    
    fclose(fp);

    data.values = std::vector<std::vector<uint32_t>>(data.height, std::vector<uint32_t>(data.width, UINT32_MAX));
    data.values[data.start.y][data.start.x] = 0;

    return data;
}

bool inBorder(const Map& map, Position p){
    return p.x >= 0 && p.y >= 0 && p.x < map.width && p.y < map.height;
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

    uint32_t retValue = 0;

    map.values[map.start.y][map.start.x] = 0;
    move(map, map.start);

    uint32_t normalEndTime = map.values[map.end.y][map.end.x];

    std::unordered_map<uint32_t, uint32_t> speeded;

    for(int x = 1; x < map.width - 1; x++){
        for(int y = 1; y < map.height -1; y++){
            if(map.map[y][x] != '#')
                continue;
            Map newMap = map;
            newMap.map[y][x] = '.';
            newMap.values = std::vector<std::vector<uint32_t>>(map.height, std::vector<uint32_t>(map.width, UINT32_MAX));
            newMap.values[map.start.y][map.start.x] = 0;
            move(newMap, map.start);
            if(newMap.values[map.end.y][map.end.x] < normalEndTime)
                speeded[normalEndTime - newMap.values[map.end.y][map.end.x]]++;
        }
    }

    for(auto& [key, value] : speeded){
        // printf("%u -> %u\n", key, value);
        if(key >= 100)
            retValue += value;
    }

    return retValue;
}

uint32_t puzzle2(Map map){

    return 0;
}

int main(int argc, char* argv[]) {

    printf("Day 18\n");

    Map map = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %u\n", puzzle1(map));
    printf("Puzzle 1: %u\n", puzzle2(map));

    return 0;
}