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

enum class Move : char {
    UP = '^',
    RIGHT = '>',
    DOWN = 'v',
    LEFT = '<',
    PRESS = 'A',
    GAP = ' '
};

constexpr Position dirPos(Move dir){
    switch(dir){
        case Move::UP:
            return {0, -1};
        case Move::DOWN:
            return {0, 1};
        case Move::LEFT:
            return {-1, 0};
        case Move::RIGHT:
            return {1, 0};
        case Move::PRESS:
            return {0, 0};
        case Move::GAP:
            return {0, 0};
    }
}

struct NumericKeyPad{
    static const std::unordered_map<char, Position> keypad;
    Position actualPosition = keypad.at('A');
};

const std::unordered_map<char, Position> NumericKeyPad::keypad = {
    {'7', {0, 0}},
    {'8', {1, 0}},
    {'9', {2, 0}},
    {'4', {0, 1}},
    {'5', {1, 1}},
    {'6', {2, 1}},
    {'1', {0, 2}},
    {'2', {1, 2}},
    {'3', {2, 2}},
    {' ', {0, 3}},
    {'0', {1, 3}},
    {'A', {2, 3}}
};

struct DirectionalKeyPad{
    static const std::unordered_map<Move, Position> keypad;

    Position actualPosition = keypad.at(Move::PRESS);
};

const std::unordered_map<Move, Position> DirectionalKeyPad::keypad = {
        {Move::GAP, {0, 0}},
        {Move::UP, {1, 0}},
        {Move::PRESS, {2, 0}},
        {Move::LEFT, {0, 1}},
        {Move::DOWN, {1, 1}},
        {Move::RIGHT, {2, 1}}
    };

struct Inputs{
    std::vector<std::vector<char>> codes;
};

Inputs loadMap(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s\n", filename);
        exit(1);
    }

    Inputs data;
    std::vector<char> row;
    char readChar;

    while((readChar = fgetc(fp)) != EOF){

        switch (readChar)
        {
        case '\n':
            data.codes.push_back(row);
            row.clear();
            break;

        default:
            row.push_back(readChar);
            break;
        }   
    }

    data.codes.push_back(row);
    
    fclose(fp);

    return data;
}

std::vector<Move> getMoves(NumericKeyPad& keypad, char button){
    std::vector<Move> retValue;

    Position hasToMove = NumericKeyPad::keypad.at(button) - keypad.actualPosition;

    if(hasToMove.x < 0 && keypad.actualPosition + (Position){hasToMove.x, 0} != NumericKeyPad::keypad.at(' ')){
        for(; 0 != hasToMove.x; hasToMove.x++){
            retValue.push_back(Move::LEFT);
        }
    }

    if(hasToMove.y > 0 && keypad.actualPosition + (Position){0, hasToMove.y} != NumericKeyPad::keypad.at(' ')){
        for(; 0 != hasToMove.y; hasToMove.y--){
            retValue.push_back(Move::DOWN);
        }
    }

    if(hasToMove.x > 0){
        for(int i = 0; i < hasToMove.x; i++)
            retValue.push_back(Move::RIGHT);
    }
    if(hasToMove.y < 0){
        for(int i = 0; i < -hasToMove.y; i++)
            retValue.push_back(Move::UP);
    } else{
        for(int i = 0; i < hasToMove.y; i++)
            retValue.push_back(Move::DOWN);
    }
    if(hasToMove.x < 0){
        for(int i = 0; i < -hasToMove.x; i++)
            retValue.push_back(Move::LEFT);
    }

    keypad.actualPosition = NumericKeyPad::keypad.at(button);

    return retValue;
}

std::vector<Move> getMoves(DirectionalKeyPad& keypad, Move button){
    std::vector<Move> retValue;

    Position hasToMove = DirectionalKeyPad::keypad.at(button) - keypad.actualPosition;

    if(hasToMove.x < 0 && keypad.actualPosition + (Position){hasToMove.x, 0} != NumericKeyPad::keypad.at(' ')){
        for(; 0 != hasToMove.x; hasToMove.x++){
            retValue.push_back(Move::LEFT);
        }
    }

    if(hasToMove.y < 0 && keypad.actualPosition + (Position){0, hasToMove.y} != NumericKeyPad::keypad.at(' ')){
        for(; 0 != hasToMove.y; hasToMove.y++){
            retValue.push_back(Move::UP);
        }
    }

    if(hasToMove.x > 0){
        for(int i = 0; i < hasToMove.x; i++)
            retValue.push_back(Move::RIGHT);
    }
    if(hasToMove.y > 0){
        for(int i = 0; i < hasToMove.y; i++)
            retValue.push_back(Move::DOWN);
    } else{
        for(int i = 0; i < -hasToMove.y; i++)
            retValue.push_back(Move::UP);
    }
    if(hasToMove.x < 0){
        for(int i = 0; i < -hasToMove.x; i++)
            retValue.push_back(Move::LEFT);
    }

    keypad.actualPosition = DirectionalKeyPad::keypad.at(button);

    return retValue;
}

std::vector<Move> getMoves(NumericKeyPad& keypad, const std::vector<char>& code){
    std::vector<Move> retValue;

    for(int i = 0; i < code.size(); i++){
        std::vector<Move> moves = getMoves(keypad, code[i]);
        retValue.insert(retValue.end(), moves.begin(), moves.end());
        retValue.push_back(Move::PRESS);
    }

    return retValue;
}

std::vector<Move> getMoves(DirectionalKeyPad& keypad, const std::vector<Move>& code){
    std::vector<Move> retValue;

    for(int i = 0; i < code.size(); i++){
        std::vector<Move> moves = getMoves(keypad, code[i]);
        retValue.insert(retValue.end(), moves.begin(), moves.end());
        retValue.push_back(Move::PRESS);
    }

    return retValue;
}

uint32_t puzzle1(Inputs& input){

    uint32_t retValue = 0;

    NumericKeyPad numPad;
    DirectionalKeyPad firstRobot;
    DirectionalKeyPad secondRobot;

    for(int i = 0; i < input.codes.size(); i++){
    
        std::vector<Move> code = getMoves(numPad, input.codes[i]);
        // for(auto c : code)
        //     printf("%c", static_cast<char>(c));
        // printf("\n");
        std::vector<Move> code1 = getMoves(firstRobot, code);

        // for(auto c : code1)
        //     printf("%c", static_cast<char>(c));
        // printf("\n");

        std::vector<Move> code2 = getMoves(secondRobot, code1);

        // for(auto c : code2)
        //     printf("%c", static_cast<char>(c));
        // printf("\n");

        uint32_t codeValue;
        sscanf(input.codes[i].data(), "%uA", &codeValue);
        retValue += codeValue * code2.size();
        printf("Code: %u, %zu\n", codeValue, code2.size());

    }

    return retValue;
}

uint32_t puzzle2(Inputs& input){

    uint32_t retValue = 0;

    return retValue;
}

int main(int argc, char* argv[]) {

    printf("Day 21\n");

    Inputs map = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %u\n", puzzle1(map));
    printf("Puzzle 2: %u\n", puzzle2(map));

    return 0;
}