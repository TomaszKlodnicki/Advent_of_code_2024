#include <iostream>
#include <vector>
#include <stdio.h>
#include <unordered_set>
#include <algorithm>

#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

#define CHOSE_FILENAME USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME

#define PRICE_A 3
#define PRICE_B 1

struct Position{
    int64_t x, y;

    Position operator+(const Position& p) const{
        Position newPos;
        newPos.x = x + p.x;
        newPos.y = y + p.y;
        return newPos;
    }

    Position operator+(int64_t p) const{
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

    Position operator*(int64_t i) const{
        Position newPos;
        newPos.x = x * i;
        newPos.y = y * i;
        return newPos;
    }

    bool operator==(Position& p) const{
        return x == p.x && y == p.y;
    }
};

struct Equation{
    Position A;
    Position B;
    Position target;
};

std::vector<Equation> loadMap(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s\n", filename);
        exit(1);
    }

    std::vector<Equation>  data;
    Equation newE;

    while(fscanf(fp, "Button A: X+%lld, Y+%lld\nButton B: X+%lld, Y+%lld\nPrize: X=%lld, Y=%lld\n", &newE.A.x, &newE.A.y, &newE.B.x, &newE.B.y, &newE.target.x, &newE.target.y) != EOF){
        data.push_back(newE);
    }
    
    fclose(fp);


    return data;
}

std::pair<double, double> calcSolution(const Equation& e){
    double b = (e.A.y * e.target.x - e.A.x * e.target.y) / (e.B.x * e.A.y - e.B.y * e.A.x);
    double a = (e.target.y - e.B.y * b) / e.A.y;

    return {a, b};
}

int puzzle1(std::vector<Equation> eVec){

    int retValue = 0;

    for (auto& e : eVec){
        auto [a, b] = calcSolution(e);

        if((e.B * int(b)) + (e.A * int(a)) == e.target && a >= 0 && b >= 0)
            retValue += int(a) * PRICE_A + int(b) * PRICE_B;
        
    }

    return retValue;
}

int64_t puzzle2(std::vector<Equation> eVec){

    int64_t retValue = 0;

    for (auto& e : eVec){
        e.target = e.target + 10000000000000;
        auto [a, b] = calcSolution(e);
        
        if((e.B * int64_t(b)) + (e.A * int64_t(a)) == e.target && a >= 0 && b >= 0)
            retValue += int64_t(a) * PRICE_A + int64_t(b) * PRICE_B;
        
    }

    return retValue;
}

int main(int argc, char* argv[]) {

    printf("Day 13\n");

    std::vector<Equation> eVec = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %d\n", puzzle1(eVec));
    printf("Puzzle 2: %lld\n", puzzle2(eVec));

    return 0;
}