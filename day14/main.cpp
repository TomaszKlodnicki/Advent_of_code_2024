#include <iostream>
#include <vector>
#include <stdio.h>
#include <unordered_set>
#include <algorithm>

#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

#define CHOSE_FILENAME USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME

constexpr int map_width(){if(USE_EXAMPLE) return 11; return 101;}
constexpr int map_height(){if(USE_EXAMPLE) return 7; return 103;}

#define MAP_WIDTH map_width()
#define MAP_HEIGHT map_height()

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
};

struct Robot{
    Position curentPos;
    Position velocity;
};

std::vector<Robot> loadMap(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s\n", filename);
        exit(1);
    }

    std::vector<Robot>  data;
    Robot newR;

    while(fscanf(fp, "p=%d,%d v=%d,%d\n", &newR.curentPos.x, &newR.curentPos.y, &newR.velocity.x, &newR.velocity.y) != EOF){
        data.push_back(newR);
    }
    
    fclose(fp);


    return data;
}

void moveRobot(Robot& r){
    r.curentPos = r.curentPos + r.velocity;

    if(r.curentPos.x < 0)
        r.curentPos.x = MAP_WIDTH + r.curentPos.x;
    if(r.curentPos.y < 0)
        r.curentPos.y = MAP_HEIGHT + r.curentPos.y;

    r.curentPos.x = r.curentPos.x % MAP_WIDTH;
    r.curentPos.y = r.curentPos.y % MAP_HEIGHT;
}

int computeSecurityFactor(std::vector<Robot>& rVec){

    static const uint32_t q1 = MAP_WIDTH / 2;
    static const uint32_t q2 = MAP_HEIGHT / 2;

    unsigned int quadrants[4] = {};
    for(auto& r : rVec){
        if(r.curentPos.x < q1 && r.curentPos.y < q2)
            quadrants[0]++;
        else if(r.curentPos.x > q1 && r.curentPos.y < q2)
            quadrants[1]++;
        else if(r.curentPos.x < q1 && r.curentPos.y > q2)
            quadrants[2]++;
        else if(r.curentPos.x > q1 && r.curentPos.y > q2)
            quadrants[3]++;
    }

    return quadrants[0] * quadrants[1] * quadrants[2] * quadrants[3];
}

int puzzle1(std::vector<Robot> rVec){

    for(int i = 0; i < 100; i++){
        for(auto& r : rVec)
            moveRobot(r);
    }

    return computeSecurityFactor(rVec);
}

void displayRobots(const std::vector<Robot>& rVec){

    uint32_t map[MAP_HEIGHT][MAP_WIDTH] = {};

    for(auto& r : rVec)
        map[r.curentPos.y][r.curentPos.x]++;

    for(int h = 0; h < MAP_HEIGHT; h++){
        for(int w = 0; w < MAP_WIDTH; w++){
            if(map[h][w] > 0)
                printf("%d", map[h][w]);
            else
                printf(".");
        }
        printf("\n");
    }
}

bool inBorder(Position pos){
    return pos.x >= 0 && pos.y >= 0 && pos.x < MAP_WIDTH && pos.y < MAP_HEIGHT;
}

int countBlobSize(Position pos, std::vector<std::vector<bool>>& counted, const std::vector<std::vector<uint32_t>>& map){

    if(counted[pos.y][pos.x])
        return 0;

    counted[pos.y][pos.x] = true;

    int retValue = 1;

    const Position dir[4] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    for(int i = 0 ; i < 4; i++){
        Position toCheck = pos + dir[i];
        if(inBorder(toCheck) && map[toCheck.y][toCheck.x] != 0)
            retValue += countBlobSize(toCheck, counted, map);
    }

    return retValue;
}

int findBiggestBlob(const std::vector<Robot>& rVec){

    std::vector<std::vector<uint32_t>> map(MAP_HEIGHT, std::vector<uint32_t>(MAP_WIDTH, 0));
    std::vector<std::vector<bool>> counted(MAP_HEIGHT, std::vector<bool>(MAP_WIDTH, false));

    for(auto& r : rVec)
        map[r.curentPos.y][r.curentPos.x]++;


    uint32_t biggestBlob = 0;
    for(int h = 0; h < MAP_HEIGHT; h++){
        for(int w = 0; w < MAP_WIDTH; w++){
            if(!counted[h][w]){
                int blobSize = countBlobSize({w, h}, counted, map);
                if(blobSize > biggestBlob)
                    biggestBlob = blobSize;
            }
        }
    }

    return biggestBlob;
}

int puzzle2(std::vector<Robot> rVec){

    int retValue = 0;

    std::vector<Robot> biggestBlob;
    int biggestBlobSize = 0;
    
    for(int i = 0; i < 20000; i++){
        for(auto& r : rVec)
            moveRobot(r);

        int blobSize = findBiggestBlob(rVec);
        if(blobSize > biggestBlobSize){
            biggestBlobSize = blobSize;
            biggestBlob = rVec;
            retValue = i + 1;
        }
    }

    displayRobots(biggestBlob);

    return retValue;
}

int main(int argc, char* argv[]) {

    printf("Day 14\n");

    std::vector<Robot> rVec = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %d\n", puzzle1(rVec));
    printf("Puzzle 2: %d\n", puzzle2(rVec));

    return 0;
}