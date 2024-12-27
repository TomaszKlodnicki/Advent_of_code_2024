#include <iostream>
#include <vector>
#include <stdio.h>

#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

#define CHOSE_FILENAME USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME

struct Computer{
    uint64_t A, B, C;
    std::vector<uint8_t> instructions;
    std::vector<uint8_t> output;
    int16_t instructionPointer = 0;
};

Computer loadMap(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s\n", filename);
        exit(1);
    }

    Computer data;
    uint8_t readData;

    fscanf(fp, "Register A: %lld\nRegister B: %lld\nRegister C: %lld\n", &data.A, &data.B, &data.C);
    fscanf(fp, "Program: ");
    while(fscanf(fp, "%hhu,", &readData) != EOF){
        data.instructions.push_back(readData);
    }

    fclose(fp);

    return data;
}

uint64_t& getOperand(Computer& comp){

    static uint64_t temp;
    temp = comp.instructions[comp.instructionPointer+1];

    switch(comp.instructions[comp.instructionPointer+1]){
        case 0:
        case 1:
        case 2:
        case 3:
            return temp;
        case 4:
            return comp.A;
        case 5:
            return comp.B;
        case 6:
            return comp.C;
    }

    printf("Error: Invalid operand\n");
    return temp;
}

uint64_t getLiteral(Computer& comp){
    return comp.instructions[comp.instructionPointer+1];
}

void adv(Computer& comp){
    if(getOperand(comp) >= 64)
        comp.A = 0;
    else
        comp.A = comp.A >> getOperand(comp);
}

void bxl(Computer& comp){
    comp.B = comp.B xor getLiteral(comp);
}

void bst(Computer& comp){
    comp.B = getOperand(comp) & 0x07llu;
}

void jnz(Computer& comp){
    if(comp.A == 0)
        return;
    comp.instructionPointer = getLiteral(comp) - 2;
}

void bxc(Computer& comp){
    comp.B = comp.B xor comp.C;
}

void out(Computer& comp){
    comp.output.push_back(getOperand(comp) & 0x07llu);
}

void bdv(Computer& comp){
    if(getOperand(comp) >= 64)
        comp.A = 0;
    else
        comp.B = comp.A >> (getOperand(comp) & 64);
}

void cdv(Computer& comp){
    if(getOperand(comp) >= 64)
        comp.A = 0;
    else
        comp.C = comp.A >> getOperand(comp);
}

void(*instructionsSet[8])(Computer& comp) = {adv, bxl, bst, jnz, bxc, out, bdv, cdv};

bool executeInstruction(Computer& comp){

    if(comp.instructionPointer >= comp.instructions.size())
        return false;

    instructionsSet[comp.instructions[comp.instructionPointer]](comp);

    comp.instructionPointer += 2;

    return true;
}

void puzzle1(Computer comp){

    while(executeInstruction(comp));

    for(auto& c : comp.output)
        printf("%hhu,", c);
}

std::vector<uint64_t> getOutput(const Computer& comp, uint8_t output, bool exitNode = false){
    std::vector<uint64_t> ret;
    for(uint64_t A = 0; A < 8; A++){
        Computer cComp = {};
        cComp.A = comp.A | A;
        cComp.instructions = comp.instructions;
        cComp.instructionPointer = 0;

        while(instructionsSet[cComp.instructions[cComp.instructionPointer]] != jnz)
            executeInstruction(cComp);

        if(cComp.output.back() == output && (exitNode ? cComp.A == 0 : comp.A != 0))
            ret.push_back(A);
    }

    return ret;
}

void reverseSolve(std::vector<uint8_t> instructions, std::vector<uint64_t>& solutions, uint8_t outputIndex = 0, uint64_t initA = 0){

    if(outputIndex >= instructions.size()){
        solutions.push_back(initA);
        return;
    }

    Computer comp = {
        .A = initA << 3,
        .instructions = instructions,
        .instructionPointer = 0
    };

    uint8_t output = instructions[instructions.size()-outputIndex-1];

    std::vector<uint64_t> possibleA = getOutput(comp, output, outputIndex == 0);
    if(possibleA.empty())
        return;

    for(auto& a : possibleA){
        reverseSolve(instructions, solutions, outputIndex+1, comp.A | a);
    }
}

uint64_t puzzle2(Computer comp){

    std::vector<uint64_t> solutions;
    reverseSolve(comp.instructions, solutions);

    if(solutions.empty())
        printf("Can't find solution!\n");
    else
        printf("Found %lu solutions\n", solutions.size());

    std::sort(solutions.begin(), solutions.end());

    for(auto& s : solutions){
        Computer cCopy = comp;
        cCopy.A = s;
        while(executeInstruction(cCopy));
        
        printf("Solution: %llu\n", s);

        for(auto& c : cCopy.output)
            printf("%hhu,", c);

        printf("\n");

        if(cCopy.output == cCopy.instructions)
            return s;
    }

    printf("Can't find any solution!\n");
    return 0;
}

int main(int argc, char* argv[]) {

    printf("Day 17\n");

    Computer comp = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: ");
    puzzle1(comp);
    printf("\n");

    printf("Puzzle 2: %llu\n", puzzle2(comp));

    return 0;
}