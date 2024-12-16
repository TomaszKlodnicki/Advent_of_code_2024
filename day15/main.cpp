#include <iostream>
#include <vector>
#include <stdio.h>

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

    bool operator==(Position& p) const{
        return x == p.x && y == p.y;
    }

    enum class Direction {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };
};

enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

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
    int16_t width, height;
    Position robot;
    std::vector<Direction> moves;
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
    bool endOfMap = false;

    while((readChar = fgetc(fp)) != EOF){

        if(endOfMap && readChar == '\n')
            break;

        switch (readChar)
        {
        case '\n':
            data.map.push_back(row);
            row.clear();
            endOfMap = true;
            break;

        case '@':
            data.robot = (Position){static_cast<int32_t>(row.size()), static_cast<int32_t>(data.map.size())};
            row.push_back('.');
            endOfMap = false;
            break;
        
        default:
            row.push_back(readChar);
            endOfMap = false;
            break;
        }   
    }

    data.height = data.map.size();
    data.width = data.map[0].size();

    while((readChar = fgetc(fp)) != EOF){
        switch(readChar){
            case '\n':
                break;
            case '^':
                data.moves.push_back(Direction::UP);
                break;
            case 'v':
                data.moves.push_back(Direction::DOWN);
                break;
            case '<':   
                data.moves.push_back(Direction::LEFT);
                break;
            case '>':   
                data.moves.push_back(Direction::RIGHT);
                break;
        }
    }
    
    fclose(fp);


    return data;
}

bool moveBox(Position boxPos, Direction dir, Map& map){
    Position newPos = boxPos + dirPos(dir);

    switch (map.map[newPos.y][newPos.x])
    {
    case '#':
        return false;
    case 'O':
        if(!moveBox(newPos, dir, map))
            return false;

    case '.':
        map.map[newPos.y][newPos.x] = 'O';
        map.map[boxPos.y][boxPos.x] = '.';
        return true;
    }

    return false;
}


void moveRobot(Map& map, Direction dir){

    Position newPos = map.robot + dirPos(dir);

    switch (map.map[newPos.y][newPos.x])
    {
    case '#':
        return;

    case '.':
        map.robot = newPos;
        break;

    case 'O':
        if(moveBox(newPos, dir, map))
            map.robot = newPos;
        break;
    }
}

int puzzle1(Map& map){

    int retValue = 0;

    for(auto& dir : map.moves){
        moveRobot(map, dir);
    }

    for(int i = 0; i < map.height; i++){
        for(int j = 0; j < map.width; j++){
            if(map.map[i][j] == 'O'){
                retValue += 100*i + j;
            }
        }
    }

    return retValue;
}

int puzzle2(Map& map){

    int retValue = 0;

    return retValue;
}

int main(int argc, char* argv[]) {

    printf("Day 15\n");

    Map map = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %d\n", puzzle1(map));
    printf("Puzzle 2: %d\n", puzzle2(map));

    return 0;
}