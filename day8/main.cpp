#include <iostream>
#include <vector>
#include <stdio.h>
#include <unordered_map>
#include <unordered_set>
#include <optional>

#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

#define CHOSE_FILENAME USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME

struct Position{
    int16_t x, y;

    bool operator== (const Position& p) const{
        return x == p.x && y == p.y;
    }
};

struct AntennaMap{
    std::unordered_map<char, std::vector<Position>> data;
    int16_t width = 0;
    int16_t height = 0;
};

template<>
struct std::hash<Position>
{
    std::size_t operator()(const Position& s) const noexcept
    {
        return s.x | (s.y << 16);
    }
};


AntennaMap loadMap(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s\n", filename);
        exit(1);
    }

    AntennaMap  data;
    char readChar;
    
    while((readChar = fgetc(fp)) != EOF){
        switch (readChar)
        {
        case '\n':
            data.height++;
            data.width = 0;
            break;
        case '.':
            data.width++;
            break;
        
        default:
            data.data[readChar].push_back({data.width, data.height});
            data.width++;
            break;
        }
    }
    
    fclose(fp);

    data.height++; 

    return data;
}

bool inBorders(Position p, const AntennaMap& data){
    return p.x >= 0 && p.y >= 0 && p.x < data.width && p.y < data.height;
}

std::pair<std::optional<Position>, std::optional<Position>> computeAntennaPosition(Position firstOne, Position secondOne, const AntennaMap& data){
    Position diff;
    diff.x = secondOne.x - firstOne.x;
    diff.y = secondOne.y - firstOne.y;

    std::pair<Position, Position> retValue;
    retValue.first.x = firstOne.x - diff.x;
    retValue.first.y =  firstOne.y - diff.y;
    retValue.second.x = secondOne.x + diff.x;
    retValue.second.y =  secondOne.y + diff.y;

    return {
        inBorders(retValue.first, data) ? std::optional<Position>(retValue.first) : std::nullopt,
        inBorders(retValue.second, data) ? std::optional<Position>(retValue.second) : std::nullopt
    };
}

std::vector<Position> computeAntennaPositionInAnyGrid(Position firstOne, Position secondOne, const AntennaMap& data){
    Position diff;
    diff.x = secondOne.x - firstOne.x;
    diff.y = secondOne.y - firstOne.y;

    secondOne = firstOne;

    std::vector<Position> retValue;

    do{
        retValue.push_back(firstOne);
        firstOne.x -= diff.x;
        firstOne.y -= diff.y;

    }while(inBorders(firstOne, data));

    do{
        retValue.push_back(secondOne);
        secondOne.x += diff.x;
        secondOne.y += diff.y;

    }while(inBorders(secondOne, data));

    return retValue;
}

int puzzle1(AntennaMap& data){

    std::unordered_set<Position> anodeSet;

    for(auto& [antenna, aPositions] : data.data){
        for(int i = 0; i < aPositions.size(); i++){
            for(int j = i+1; j < aPositions.size(); j++){
                auto [first, second] = computeAntennaPosition(aPositions[i], aPositions[j], data);
                if(first.has_value())
                    anodeSet.insert(first.value());
                if(second.has_value())
                    anodeSet.insert(second.value());
            }
        }
    }

    return anodeSet.size();
}

int puzzle2(AntennaMap& data){

    std::unordered_set<Position> anodeSet;

    for(auto& [antenna, aPositions] : data.data){
        for(int i = 0; i < aPositions.size(); i++){
            for(int j = i+1; j < aPositions.size(); j++){
                auto retVec = computeAntennaPositionInAnyGrid(aPositions[i], aPositions[j], data);
                for(auto& p : retVec){
                    anodeSet.insert(p);
                }
            }
        }
    }

    return anodeSet.size();
}

int main(int argc, char* argv[]) {

    printf("Day 8\n");

    AntennaMap data = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %d\n", puzzle1(data));
    printf("Puzzle 2: %d\n", puzzle2(data));

    return 0;
}