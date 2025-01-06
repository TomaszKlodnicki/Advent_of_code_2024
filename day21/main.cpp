#include <iostream>
#include <vector>
#include <stdio.h>
#include <unordered_map>
#include <set>

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

    Position operator-(const Position& p) const{
        Position newPos;
        newPos.x = x - p.x;
        newPos.y = y - p.y;
        return newPos;
    }

    bool operator==(const Position& p) const{
        return x == p.x && y == p.y;
    }
};

enum class Move : char {
    UP = '^',
    RIGHT = '>',
    DOWN = 'v',
    LEFT = '<',
    PRESS = 'A',
    GAP = ' '
};

template<>
struct std::hash<std::pair<Move, Move>> {
    std::size_t operator()(const std::pair<Move, Move>& p) const noexcept
    {
        return static_cast<char>(p.first) | (static_cast<char>(p.second) << 8);
    }
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
    char actualKey = 'A';
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

    Move actualMove = Move::PRESS;
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

std::set<std::vector<Move>> permute(const std::vector<Move>& vec){
    std::set<std::vector<Move>> retValue;

    if(vec.size() == 1){
        retValue.insert(vec);
        return retValue;
    }
    
    for(int i = 0; i < vec.size(); i++){
        std::vector<Move> newVec = vec;
        newVec.erase(newVec.begin() + i);
        auto permuted = permute(newVec);
        for(auto p : permuted){
            p.push_back(vec[i]);
            retValue.insert(p);
        }
    }
    return retValue;
}

std::set<std::vector<Move>> getMoves(NumericKeyPad& keypad, char button){

    std::set<std::vector<Move>> retSet;
    std::vector<Move> retVec;
    Position hasToMove = NumericKeyPad::keypad.at(button) - NumericKeyPad::keypad.at(keypad.actualKey);

    for(; hasToMove.x != 0; (hasToMove.x > 0) ? hasToMove.x-- : hasToMove.x++)
        retVec.push_back(hasToMove.x > 0 ? Move::RIGHT : Move::LEFT);

    for(; hasToMove.y != 0; (hasToMove.y > 0) ? hasToMove.y-- : hasToMove.y++)
        retVec.push_back(hasToMove.y > 0 ? Move::DOWN : Move::UP);

    retSet = permute(retVec);

    for(auto it = retSet.begin(); it != retSet.end();){
        Position pos = NumericKeyPad::keypad.at(keypad.actualKey);
        bool erase = false;
        for(auto& m : *it){
            pos = pos + dirPos(m);
            if(NumericKeyPad::keypad.at(' ') == pos){
                it = retSet.erase(it);
                erase = true;
                break;
            }
        }
        if(!erase)
            it++;
    }

    keypad.actualKey = button;

    return retSet;
}

const std::set<std::vector<Move>>& getMoves(DirectionalKeyPad& keypad, Move button){

    static std::unordered_map<std::pair<Move, Move>, std::set<std::vector<Move>>> saved;
    auto savedRecord = saved.find((std::pair<Move, Move>){keypad.actualMove, button});
    if(savedRecord != saved.end()){
        keypad.actualMove = button;
        return savedRecord->second;
    }

    std::set<std::vector<Move>> retSet;
    std::vector<Move> retVec;
    Position hasToMove = DirectionalKeyPad::keypad.at(button) - DirectionalKeyPad::keypad.at(keypad.actualMove);

    for(; hasToMove.x != 0; (hasToMove.x > 0) ? hasToMove.x-- : hasToMove.x++)
        retVec.push_back(hasToMove.x > 0 ? Move::RIGHT : Move::LEFT);

    for(; hasToMove.y != 0; (hasToMove.y > 0) ? hasToMove.y-- : hasToMove.y++)
        retVec.push_back(hasToMove.y > 0 ? Move::DOWN : Move::UP);

    retSet = permute(retVec);

    for(auto it = retSet.begin(); it != retSet.end();){
        Position pos = DirectionalKeyPad::keypad.at(keypad.actualMove);
        bool erase = false;
        for(auto& m : *it){
            pos = pos + dirPos(m);
            if(DirectionalKeyPad::keypad.at(Move::GAP) == pos){
                it = retSet.erase(it);
                erase = true;
                break;
            }
        }
        if(!erase)
            it++;
    }

    std::pair<Move, Move> record = {keypad.actualMove, button};

    saved.insert({record, retSet});
    keypad.actualMove = button;
    return saved[record];
}

std::set<std::vector<Move>> getMoves(NumericKeyPad& keypad, const std::vector<char>& code){
    std::vector<std::set<std::vector<Move>>> vectors(code.size());
    std::set<std::vector<Move>> retSet;

    for(int i = 0; i < code.size(); i++)
        vectors[i] = getMoves(keypad, code[i]);

    std::function<void(uint8_t, std::vector<Move>)> reqMoves;
    reqMoves = [&vectors, &retSet, &reqMoves](uint8_t depth, std::vector<Move> vec){
        if(depth == vectors.size()){
            retSet.insert(vec);
            return;
        }

        for(auto& v : vectors[depth]){
            std::vector<Move> newVec = vec;
            newVec.insert(newVec.end(), v.begin(), v.end());
            newVec.push_back(Move::PRESS);
            reqMoves(depth + 1, newVec);
        }
    };

    reqMoves(0, {});
    return retSet;
}


struct SaveSize{
    Move start;
    Move end;
    uint8_t depth;

    bool operator==(const SaveSize& s) const{
        return start == s.start && end == s.end && depth == s.depth;
    }
};

template<>
struct std::hash<SaveSize>{
    std::size_t operator()(const SaveSize& s) const noexcept
    {
        return static_cast<char>(s.start) | (static_cast<char>(s.end) << 8) | (s.depth << 16);
    }
};

#define MAX_DEEP 25
uint64_t getCodeSize(Move input, uint8_t depth = 0){
    static DirectionalKeyPad robots[MAX_DEEP];
    static std::unordered_map<SaveSize, uint64_t> saved;

    auto savedRecord = saved.find((SaveSize){robots[depth].actualMove, input, depth});
    if(savedRecord != saved.end()){
        robots[depth].actualMove = input;
        for(int i = MAX_DEEP - 1; i > depth; i--){
            robots[i].actualMove = Move::PRESS;
        }
        return savedRecord->second;
    }

    SaveSize record = {robots[depth].actualMove, input, depth};

    const std::set<std::vector<Move>>& set = getMoves(robots[depth], input);

    if(depth == MAX_DEEP - 1){
        saved.insert({record, set.size() != 0 ? set.begin()->size() + 1 : 1});
        return saved[record];
    }

    uint64_t retValue = UINT64_MAX;
    
    for(const auto& vec : set){
        uint64_t partialRetValue = 0;
        for(int i = 0; i < vec.size(); i++){
            partialRetValue += getCodeSize(vec[i], depth + 1);
        }
        partialRetValue += getCodeSize(Move::PRESS, depth + 1);
        if(partialRetValue < retValue)
            retValue = partialRetValue;
    }

    if(set.size() == 0)
        retValue = getCodeSize(Move::PRESS, depth + 1);

    saved.insert({record, retValue});

    return retValue;
}

uint64_t computeForIterations(Inputs& input, uint8_t iterations){

    uint64_t retValue = 0;

    NumericKeyPad numPad;

    for(int i = 0; i < input.codes.size(); i++){
        
        uint64_t codeValue;
        uint64_t cV = UINT64_MAX;
        sscanf(input.codes[i].data(), "%lluA", &codeValue);

        auto codes = getMoves(numPad, input.codes[i]);

        for(auto& possibility : codes){
            uint64_t cVPossibility = 0;
            for(auto c : possibility){
                uint64_t codeSize = getCodeSize(c, MAX_DEEP - iterations);
                cVPossibility += codeSize;
            }
            if(cVPossibility < cV)
                cV = cVPossibility;
        }
        retValue += cV * codeValue;
    }

    return retValue;
}

uint32_t puzzle1(Inputs& input){

    return computeForIterations(input, 2);
}

uint64_t puzzle2(Inputs& input){

    return computeForIterations(input, MAX_DEEP);
}

int main(int argc, char* argv[]) {

    printf("Day 21\n");

    Inputs map = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %u\n", puzzle1(map));
    printf("Puzzle 2: %llu\n", puzzle2(map));

    return 0;
}