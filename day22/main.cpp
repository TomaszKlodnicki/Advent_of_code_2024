#include <iostream>
#include <vector>
#include <stdio.h>
#include <unordered_map>
#include <unordered_set>
#include <cstring>

#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

#define CHOSE_FILENAME USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME

std::vector<uint64_t> loadMap(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s\n", filename);
        exit(1);
    }

    std::vector<uint64_t> data;
    uint64_t readNumber;

    while(fscanf(fp, "%llu\n", &readNumber) != EOF)
        data.push_back(readNumber);

    fclose(fp);

    return data;
}

#define MIX(secret, value) secret ^ value
#define PRUNE(secret) secret & 16777215ULL

#define MUL64(secret) secret << 6
#define DIV32(secret) secret >> 5
#define MUL2048(secret) secret << 11

uint64_t evolveSecret(uint64_t secret){
    secret = PRUNE(MIX(secret, MUL64(secret)));
    secret = PRUNE(MIX(secret, DIV32(secret)));
    return PRUNE(MIX(secret, MUL2048(secret)));
}

uint64_t iterateEvolve(uint64_t secret, uint16_t iterations){
    for(uint16_t i = 0; i < iterations; i++)
        secret = evolveSecret(secret);
    return secret;
}

uint64_t puzzle1(std::vector<uint64_t>& input){

    uint64_t retValue = 0;

    for(auto& i : input){
        retValue += iterateEvolve(i, 2000);
    }

    return retValue;
}

struct Code{
    int8_t change[4] = {};

    bool operator==(const Code& other) const {
        return memcmp(change, other.change, 4) == 0;
    }
};

template<>
struct std::hash<Code> {
    std::size_t operator()(const Code& code) const noexcept
    {
        return code.change[0] | (code.change[1] << 8) | (code.change[2] << 16) | (code.change[3] << 24);
    }
};

struct Buyer{
    uint64_t inputCode;
    std::unordered_map<Code, int8_t> sel;
};

uint64_t puzzle2(std::vector<uint64_t>& input){

    uint64_t retValue = 0;

    std::vector<Buyer> buyers (input.size());
    std::unordered_set<Code> possibleCodes;

    for(int i = 0; i < input.size(); i++){
        buyers[i].inputCode = input[i];
        uint64_t secret = input[i];
        uint64_t lastSecret = secret;
        Code code;
        for(int16_t j = 0; j < 2000; j++){
            secret = evolveSecret(secret);
            memmove(code.change, code.change + 1, 3);
            code.change[3] = (secret % 10) - (lastSecret % 10);
            lastSecret = secret;
            if(j > 2 && buyers[i].sel.find(code) == buyers[i].sel.end()){
                buyers[i].sel[code] = secret % 10;
                possibleCodes.insert(code);
            }
        }
    }

    Code retCode;

    for(auto& code : possibleCodes){
        uint64_t possibleValue = 0;
        for(auto& buyer : buyers){
            auto it = buyer.sel.find(code);
            if(it == buyer.sel.end())
                continue;

            possibleValue += it->second;
        }
        if(possibleValue > retValue){
            retValue = possibleValue;
            retCode = code;
        }
    }

    for(int i = 0; i < 4; i++)
        printf("%d ", retCode.change[i]);

    return retValue;
}

int main(int argc, char* argv[]) {

    printf("Day 22\n");

    std::vector<uint64_t> data = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %llu\n", puzzle1(data));
    printf("Puzzle 2: %llu\n", puzzle2(data));

    return 0;
}