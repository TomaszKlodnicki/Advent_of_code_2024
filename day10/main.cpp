#include <iostream>
#include <vector>
#include <stdio.h>
#include <unordered_set>

#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

#define CHOSE_FILENAME USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME

struct Position{
    int16_t first, second;

    bool operator== (const Position& p) const{
        return first == p.first && second == p.second;
    }

    bool operator!= (const Position& p) const{
        return first != p.first || second != p.second;
    }
};

struct Road{
    Position road[10];

    bool operator== (const Road& r) const{
        return memcmp(road, r.road, 10 * sizeof(Position)) == 0;
    }
};

struct data {
    std::vector<std::vector<char>> map;
    int16_t width, height;
    std::vector<Position> zeros;
};

template<>
struct std::hash<Position> {
    std::size_t operator()(const Position& p) const noexcept
    {
        return p.first | (p.second << 16);
    }
};

template<>
struct std::hash<Road> {
    std::size_t operator()(const Road& p) const noexcept
    {
        size_t hashValue = 0;
        for(int i = 0; i < 10; i++)
            hashValue +=  p.road[i].first | (p.road[i].second << 16);
        
        return hashValue;
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
            if(readChar == '0'){
                data.zeros.push_back((Position){(int16_t)row.size(), (int16_t)data.map.size()});
            }
            row.push_back(readChar);
        }

    } while (readChar != EOF);
    
    fclose(fp);

    data.width = data.map[0].size();
    data.height = data.map.size();

    return data;
}

bool inBorders(Position p, const data& hMap){
    return p.first >= 0 && p.second >= 0 && p.first < hMap.width && p.second < hMap.height;
}

bool inBorders(int16_t x, int16_t y, const data& hMap){
    return x >= 0 && y >= 0 && x < hMap.width && y < hMap.height;
}

void moveUp(Position pos, data& hMap, std::unordered_set<Position>& visitedHills){

    if(hMap.map[pos.second][pos.first] == '9'){
        visitedHills.insert(pos);
        return;
    }

    if(inBorders(pos.first, pos.second - 1, hMap) && hMap.map[pos.second][pos.first] + 1 == hMap.map[pos.second - 1][pos.first]){
        Position newPos;
        newPos.first = pos.first;
        newPos.second = pos.second - 1;
        moveUp(newPos, hMap, visitedHills);
    }

    if(inBorders(pos.first, pos.second + 1, hMap) && hMap.map[pos.second][pos.first] + 1 == hMap.map[pos.second + 1][pos.first]){
        Position newPos;
        newPos.first = pos.first;
        newPos.second = pos.second + 1;
        moveUp(newPos, hMap, visitedHills);
    }
        
    if(inBorders(pos.first - 1, pos.second, hMap) && hMap.map[pos.second][pos.first] + 1 == hMap.map[pos.second][pos.first - 1]){
        Position newPos;
        newPos.first = pos.first - 1;
        newPos.second = pos.second;
        moveUp(newPos, hMap, visitedHills);
    }
        
    if(inBorders(pos.first + 1, pos.second, hMap) && hMap.map[pos.second][pos.first] + 1 == hMap.map[pos.second][pos.first + 1]){
        Position newPos;
        newPos.first = pos.first + 1;
        newPos.second = pos.second;
        moveUp(newPos, hMap, visitedHills);
    }

}

void moveUp(Position pos, data& hMap, std::unordered_set<Road>& visitedRoad, Road& road){

    road.road[hMap.map[pos.second][pos.first] - '0'] = pos;

    if(hMap.map[pos.second][pos.first] == '9'){
        visitedRoad.insert(road);
        return;
    }

    if(inBorders(pos.first, pos.second - 1, hMap) && hMap.map[pos.second][pos.first] + 1 == hMap.map[pos.second - 1][pos.first]){
        Position newPos;
        newPos.first = pos.first;
        newPos.second = pos.second - 1;
        Road newRoad = road;
        moveUp(newPos, hMap, visitedRoad, newRoad);
    }

    if(inBorders(pos.first, pos.second + 1, hMap) && hMap.map[pos.second][pos.first] + 1 == hMap.map[pos.second + 1][pos.first]){
        Position newPos;
        newPos.first = pos.first;
        newPos.second = pos.second + 1;
        Road newRoad = road;
        moveUp(newPos, hMap, visitedRoad, newRoad);
    }
        
    if(inBorders(pos.first - 1, pos.second, hMap) && hMap.map[pos.second][pos.first] + 1 == hMap.map[pos.second][pos.first - 1]){
        Position newPos;
        newPos.first = pos.first - 1;
        newPos.second = pos.second;
        Road newRoad = road;
        moveUp(newPos, hMap, visitedRoad, newRoad);
    }
        
    if(inBorders(pos.first + 1, pos.second, hMap) && hMap.map[pos.second][pos.first] + 1 == hMap.map[pos.second][pos.first + 1]){
        Position newPos;
        newPos.first = pos.first + 1;
        newPos.second = pos.second;
        Road newRoad = road;
        moveUp(newPos, hMap, visitedRoad, newRoad);
    }

}

int reachTheHill(Position& zero, data& hMap){

    std::unordered_set<Position> visitedHills;

    moveUp(zero, hMap, visitedHills);

    return visitedHills.size();

}


int reachTheRoad(Position& zero, data& hMap){

    std::unordered_set<Road> visitedRoads;

    Road road;

    moveUp(zero, hMap, visitedRoads, road);

    return visitedRoads.size();

}

int puzzle1(data& hMap){

    int retValue = 0;

    for(auto& zero : hMap.zeros){
        retValue += reachTheHill(zero, hMap);
    }

    return retValue;
}

int puzzle2(data& hMap){
    int retValue = 0;

    for(auto& zero : hMap.zeros){
        retValue += reachTheRoad(zero, hMap);
    }

    return retValue;
}

int main(int argc, char* argv[]) {

    printf("Day 10\n");

    data hMap = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %d\n", puzzle1(hMap));
    printf("Puzzle 2: %d\n", puzzle2(hMap));

    return 0;
}