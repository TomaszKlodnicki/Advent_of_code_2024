#include <iostream>
#include <vector>
#include <stdio.h>
#include <map>
#include <algorithm>
#include <unordered_set>

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

struct Move{
    Position pos;
    Direction dir;
    uint32_t value;
};

struct PosDir{
    Position pos;
    Direction dir;

    bool operator==(const PosDir& p) const{
        return pos == p.pos && dir == p.dir;
    }
};

template<>
struct std::hash<PosDir> {
    std::size_t operator()(const PosDir& p) const noexcept
    {
        return p.pos.x | (p.pos.y << 16) | (static_cast<int>(p.dir) << 24);
    }
};

struct Map{
    std::vector<std::vector<char>> map;
    std::vector<std::vector<std::map<Direction, uint32_t>>> visited;
    int16_t width, height;
    Position robot, end;

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
            data.robot = (Position){static_cast<int32_t>(row.size()), static_cast<int32_t>(data.map.size())};
            row.push_back('S');
            break;

        case 'E':
            data.end = (Position){static_cast<int32_t>(row.size()), static_cast<int32_t>(data.map.size())};
        
        default:
            row.push_back(readChar);
            break;
        }   
    }

    data.height = data.map.size();
    data.width = data.map[0].size();
    
    fclose(fp);

    data.visited = std::vector<std::vector<std::map<Direction, uint32_t>>>(data.height, std::vector<std::map<Direction, uint32_t>>(data.width, {{Direction::UP, UINT32_MAX}, {Direction::DOWN, UINT32_MAX}, {Direction::LEFT, UINT32_MAX}, {Direction::RIGHT, UINT32_MAX}}));
    data.visited[data.robot.y][data.robot.x][Direction::RIGHT] = 0;

    return data;
}

bool inBorder(Position p, const Map& pMap){
    return p.x >= 0 && p.y >= 0 && p.x < pMap.width && p.y < pMap.height;
}

void computePosition(Map& map, std::vector<Move>& toCheck){

    for(int i = -1; i < 2; i++){
        Direction dir = toCheck[0].dir + i;
        Position newPos = toCheck[0].pos;
        if(dir == toCheck[0].dir)
            newPos = newPos + dirPos(dir);

        if(inBorder(newPos, map) && map[newPos] != '#'){
            uint32_t newValue = toCheck[0].value + (toCheck[0].dir == dir ? 1 : 1000);
            if(newValue > map.visited[newPos.y][newPos.x][dir])
                continue;
            map.visited[newPos.y][newPos.x][dir] = newValue;
            toCheck.push_back({newPos, dir, newValue});
        }
    }
}

void selectSpot(Map& map, std::vector<PosDir>& toCheck){

    static std::unordered_set<PosDir> visited; 

    for(int i = -1; i < 2; i++){
        Direction dir = toCheck[0].dir + i;
        Position newPos = toCheck[0].pos;
        if(dir == toCheck[0].dir)
            newPos = newPos + dirPos(!dir);

        if(inBorder(newPos, map) && map[newPos] != '#' && visited.find({newPos, dir}) == visited.end()){
            visited.insert({newPos, dir});
            int32_t diff = map.visited[newPos.y][newPos.x][dir] - map.visited[toCheck[0].pos.y][toCheck[0].pos.x][toCheck[0].dir];
            if(diff != -1 && diff != -1000 & diff != -1001 && diff != 0)
                continue;
            toCheck.push_back({newPos, dir});
            map[newPos] = 'O';
        }
    }
}

int puzzle1(Map& map){

    std::vector<Move> toCheck = {(Move){map.robot, Direction::RIGHT, 0}};

    while(!toCheck.empty()){
        computePosition(map, toCheck);
        toCheck.erase(toCheck.begin());
    }

    return std::min_element(map.visited[map.end.y][map.end.x].begin(), map.visited[map.end.y][map.end.x].end(), [](auto& a, auto& b){return a.second < b.second;})->second;
}

int puzzle2(Map& map){

    int retValue = 0;

    std::vector<PosDir> toCheckEnd;

    uint32_t min = std::min_element(map.visited[map.end.y][map.end.x].begin(), map.visited[map.end.y][map.end.x].end(), [](auto& a, auto& b){return a.second < b.second;})->second;

    for(auto [dir, value] : map.visited[map.end.y][map.end.x])
        if(value == min)
            toCheckEnd.push_back({map.end, dir});

    while(!toCheckEnd.empty()){
        selectSpot(map, toCheckEnd);
        toCheckEnd.erase(toCheckEnd.begin());
    }

    map.map[map.end.y][map.end.x] = 'O';
    map.map[map.robot.y][map.robot.x] = 'O';

    for(int i = 0; i < map.height; i++){
        for(int j = 0; j < map.width; j++){
            if(map.map[i][j] == 'O')
                retValue++;
        }
    }

    return retValue;
}

int main(int argc, char* argv[]) {

    printf("Day 16\n");

    Map map = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %d\n", puzzle1(map));
    printf("Puzzle 2: %d\n", puzzle2(map));

    return 0;
}