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

    switch (map[newPos])
    {
    case '#':
        return false;
    case 'O':
        if(!moveBox(newPos, dir, map))
            return false;

    case '.':
        map[newPos] = 'O';
        map[boxPos] = '.';
        return true;
    }

    return false;
}

bool moveBigBoxLR(Position boxPos, Direction dir, Map& map){
    Position newPos = boxPos + (dirPos(dir) * 2);

    if(map[newPos] == '#')
        return false;
    
    if(dir == Direction::LEFT){
        if(map[newPos] == ']')
            if(!moveBigBoxLR(newPos, dir, map))
                return false;
        
        map[newPos] = '[';
        map.map[newPos.y][newPos.x + 1] = ']';
        map[boxPos] = '.';
        return true;
    }
    else{
        if(map[newPos] == '[')
            if(!moveBigBoxLR(newPos, dir, map))
                return false;
        
        map[newPos] = ']';
        map.map[newPos.y][newPos.x - 1] = '[';
        map[boxPos] = '.';
        return true;
    }

    return false;
}

bool moveBigBoxUD(Position boxPos, Direction dir, Map& map, bool move = true){

    Position boxPos2 = boxPos + (map[boxPos] == ']' ? Position{-1, 0} : Position{1, 0});

    if(map[boxPos2] != ']'){
        Position temp = boxPos2;
        boxPos2 = boxPos;
        boxPos = temp;
    }

    Position newPos = boxPos + dirPos(dir);
    Position newPos2 = boxPos2 + dirPos(dir);

    if(map[newPos] == '#' || map[newPos2] == '#')
        return false;
    
    if(map[newPos] == ']' && map[newPos2] == '['){
        if(!moveBigBoxUD(newPos, dir, map, false) || !moveBigBoxUD(newPos2, dir, map, false))
            return false;

        moveBigBoxUD(newPos, dir, map, move);
        moveBigBoxUD(newPos2, dir, map, move);
    } else if(map[newPos] == '[' || map[newPos] == ']'){

        if(!moveBigBoxUD(newPos, dir, map, move))
            return false;

    } else if(map[newPos2] == '[' || map[newPos2] == ']'){

        if(!moveBigBoxUD(newPos2, dir, map, move))
            return false;

    }

    if(move){
        map[boxPos] = '.';
        map[boxPos2] = '.';
        map[newPos] = '[';
        map[newPos2] = ']';
    }

    return true;
}

bool moveBigBox(Position boxPos, Direction dir, Map& map){

    if(dir == Direction::UP || dir == Direction::DOWN)
        return moveBigBoxUD(boxPos, dir, map);
    else
        return moveBigBoxLR(boxPos, dir, map);

}


void moveRobot(Map& map, Direction dir){

    Position newPos = map.robot + dirPos(dir);

    switch (map[newPos])
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

    case ']':
        if(moveBigBox(newPos, dir, map))
            map.robot = newPos;
        break;
    case '[':
        if(moveBigBox(newPos, dir, map))
            map.robot = newPos;
        break;
    }
}

void resizeMap(Map& map){
    for(auto& row : map.map){
        for(unsigned int i = 0; i < row.size(); i+=2){
            switch(row[i]){
                case '#':
                    row.insert(row.begin() + i, '#');
                    break;
                case 'O':
                    row[i] = ']';
                    row.insert(row.begin() + i, '[');
                    break;
                case '.':
                    row.insert(row.begin() + i, '.');
                    break;
            }
        }
    }
    map.width *= 2;
    map.robot.x *= 2;
}

void printMap(Map& map){
    for(int i = 0; i < map.height; i++){
        for(int j = 0; j < map.width; j++){
            if(i == map.robot.y && j == map.robot.x)
                printf("@");
            else
                printf("%c", map.map[i][j]);
        }
        printf("\n");
    }
}

int puzzle1(Map map){

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

    resizeMap(map);

    for(auto& dir : map.moves)
        moveRobot(map, dir);

    for(int i = 0; i < map.height; i++){
        for(int j = 0; j < map.width; j++){
            if(map.map[i][j] == '['){
                retValue += 100*i + j;
            }
        }
    }

    return retValue;
}

int main(int argc, char* argv[]) {

    printf("Day 15\n");

    Map map = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %d\n", puzzle1(map));
    printf("Puzzle 2: %d\n", puzzle2(map));

    return 0;
}