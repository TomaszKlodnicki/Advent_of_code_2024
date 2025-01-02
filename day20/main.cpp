#include <iostream>
#include <vector>
#include <stdio.h>
#include <unordered_map>
#include <unordered_set>
#include <queue>

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

template<>
struct std::hash<Position> {
    std::size_t operator()(const Position& p) const noexcept
    {
        return p.x | (p.y << 16);
    }
};

template<>
struct std::hash<std::pair<Position, Position>> {
    std::size_t operator()(const std::pair<Position, Position>& p) const noexcept
    {
        return p.first.x ^ (p.first.y << 8) ^ (p.second.x << 16) ^ (p.second.y << 4);
    }
};

bool operator==(const std::pair<Position, Position>& p1, const std::pair<Position, Position>& p2){
    return p1.first.x == p2.first.x && p1.first.y == p2.first.y && p1.second.x == p2.second.x && p1.second.y == p2.second.y;
}

enum class Direction : int {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
};

const Direction directions[] = {Direction::UP, Direction::RIGHT, Direction::DOWN, Direction::LEFT};

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

    const char& operator[](Position p) const{
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

    return data;
}

bool inBorder(int16_t width, int16_t height, Position p){
    return p.x >= 0 && p.y >= 0 && p.x < width && p.y < height;
}

void move(const Map& map, std::vector<std::vector<uint32_t>>& values, Position pos, const std::unordered_set<Position>& allowed){

    uint32_t value = values[pos.y][pos.x] + 1;
    for(Direction dir : directions){
        Position newPos = pos + dirPos(dir);
        if(inBorder(map.width, map.height, newPos) && (map[newPos] != '#' || allowed.find(newPos) != allowed.end()) && values[newPos.y][newPos.x] > value){
            values[newPos.y][newPos.x] = value;
            move(map, values, newPos, allowed);
        }
    }
}

uint32_t puzzle1(Map map){

    uint32_t retValue = 0;

    map.values[map.start.y][map.start.x] = 0;
    move(map, map.values, map.start, {});

    uint32_t normalEndTime = map.values[map.end.y][map.end.x];

    std::unordered_map<uint32_t, uint32_t> speeded;

    for(int x = 1; x < map.width - 1; x++){
        for(int y = 1; y < map.height -1; y++){
            if(map.map[y][x] != '#')
                continue;
            
            Position wallPosition = (Position){x, y};
            Position startPosition = wallPosition;

            for(Direction dir : directions){
                Position newPos = (Position){x, y} + dirPos(dir);
                if(inBorder(map.width, map.height, newPos) && map.values[newPos.y][newPos.x] < map.values[startPosition.y][startPosition.x]){
                    startPosition = newPos;
                }
            }

            if(map.values[startPosition.y][startPosition.x] == UINT32_MAX)
                continue;
            
            auto newValues = map.values;
            move(map, newValues, startPosition, {wallPosition});
            if(newValues[map.end.y][map.end.x] < normalEndTime)
                speeded[normalEndTime - newValues[map.end.y][map.end.x]]++;
        }
    }

    for(auto& [key, value] : speeded)
        if(key >= 100)
            retValue += value;

    return retValue;
}

void computeSpeededForPosition(Map& map, const Position& startPos, Position pos, std::unordered_map<uint32_t, uint32_t>& speeded, uint8_t cheatLen = 0){

    static std::unordered_set<std::pair<Position, Position>> visited;
    static std::queue<std::pair<Position, uint8_t>> toVisit;

    cheatLen++;

    for(Direction dir : directions){
        Position newPos = pos + dirPos(dir);

        if(visited.find({startPos, newPos}) != visited.end())
            continue;

        visited.insert({startPos, newPos});

        if(inBorder(map.width, map.height, newPos)){
            if(map.values[newPos.y][newPos.x] + cheatLen < map.values[startPos.y][startPos.x] && map[newPos] != '#'){
                uint32_t speededTime = map.values[startPos.y][startPos.x] - (map.values[newPos.y][newPos.x] + cheatLen);
                speeded[speededTime]++;
            }
            if(cheatLen < 20)
                toVisit.push({newPos, cheatLen});
        }
    }

    while(!toVisit.empty()){
        auto [queuePos, newCheatLen] = toVisit.front();
        toVisit.pop();
        computeSpeededForPosition(map, startPos, queuePos, speeded, newCheatLen);
    }

}

uint32_t puzzle2(Map map){

    uint32_t retValue = 0;

    map.values[map.end.y][map.end.x] = 0;
    move(map, map.values, map.end, {});

    std::unordered_map<uint32_t, uint32_t> speeded;

    for(int16_t h = 0; h < map.height; h++){
        for(int16_t w = 0; w < map.width; w++){
            if(map.map[h][w] == '#')
                continue;

            Position startPos = (Position){w, h};
            
            computeSpeededForPosition(map, startPos, startPos, speeded);
        }
    }


    for(auto& [key, value] : speeded)
        if(key >= 100)
            retValue += value;

    return retValue;
}

int main(int argc, char* argv[]) {

    printf("Day 20\n");

    Map map = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %u\n", puzzle1(map));
    printf("Puzzle 2: %u\n", puzzle2(map));

    return 0;
}