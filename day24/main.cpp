#include <iostream>
#include <vector>
#include <stdio.h>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

#define CHOSE_FILENAME USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME

#define UNSET 0xFF

struct RegName{
    char name[3];

    bool operator==(const RegName& other) const{
        return memcmp(name, other.name, 3) == 0;
    }
};

struct Gate{
    RegName input1;
    RegName input2;
    RegName output;
    void (*operation)(uint8_t&, uint8_t&, uint8_t&);
};

template<>
struct std::hash<RegName>{
    std::size_t operator()(const RegName& name) const{
        return name.name[0] | (name.name[1] << 8) | (name.name[2] << 16);
    }
};

struct System{
    std::unordered_map<RegName, uint8_t> registers;
    std::vector<Gate> gates;
};

void AND(uint8_t& a, uint8_t& b, uint8_t& c){
    c = a & b;
    c &= 0x01;
}

void OR(uint8_t& a, uint8_t& b, uint8_t& c){
    c = a | b;
    c &= 0x01;

}

void XOR(uint8_t& a, uint8_t& b, uint8_t& c){
    c = a ^ b;
    c &= 0x01;
}

System loadMap(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s\n", filename);
        exit(1);
    }

    System data;
    RegName name;
    uint8_t regValue;
    
    while(fscanf(fp, "%c%c%c: %hhu\n", &name.name[0], &name.name[1], &name.name[2], &regValue ) == 4){
        data.registers[name] = regValue;
    }

    RegName input1 = name;
    RegName input2, output;
    char operation[4];

    fscanf(fp, "%3s %c%c%c -> %c%c%c\n",(char*)&operation, &input2.name[0], &input2.name[1], &input2.name[2], &output.name[0], &output.name[1], &output.name[2]);

    do{
        printf("%c%c%c %s %c%c%c -> %c%c%c\n", input1.name[0], input1.name[1], input1.name[2], operation, input2.name[0], input2.name[1], input2.name[2], output.name[0], output.name[1], output.name[2]);

        if(data.registers.find(input1) == data.registers.end())
            data.registers[input1] = UNSET;

        if(data.registers.find(input2) == data.registers.end())
            data.registers[input2] = UNSET;

        if(data.registers.find(output) == data.registers.end())
            data.registers[output] = UNSET;

        if(operation[0] == 'A'){
            data.gates.push_back({input1, input2, output, AND});
        }else if(operation[0] == 'O'){
            data.gates.push_back({input1, input2, output, OR});
        }else if(operation[0] == 'X'){
            data.gates.push_back({input1, input2, output, XOR});
        }
    }while(fscanf(fp, "%c%c%c %3s %c%c%c -> %c%c%c\n", &input1.name[0], &input1.name[1], &input1.name[2], (char*)&operation, &input2.name[0], &input2.name[1], &input2.name[2], &output.name[0], &output.name[1], &output.name[2]) != EOF);

    fclose(fp);

    return data;
}

uint64_t puzzle1(System input){

    std::vector<bool> executed(input.gates.size(), false);
    bool allExecuted = false;
    while(!allExecuted){
        allExecuted = true;
        for(int i = 0; i < input.gates.size(); i++){
            if(executed[i])
                continue;

            Gate& gate = input.gates[i];
            if(input.registers[gate.input1] == UNSET || input.registers[gate.input2] == UNSET)
                continue;

            gate.operation(input.registers[gate.input1], input.registers[gate.input2], input.registers[gate.output]);
            executed[i] = true;
            allExecuted = false;
        }
    }

    uint64_t retValue = 0;

    for(auto& r : input.registers){
        if(r.first.name[0] != 'z')
            continue;

        uint8_t regNum;
        sscanf(r.first.name, "z%hhu", &regNum);
        retValue |= uint64_t(r.second) << regNum;

        printf("%c%c%c: %hhu\n", r.first.name[0], r.first.name[1], r.first.name[2], r.second);
    }

    return retValue;
}

uint64_t puzzle2(System& input){

    return 0;
}

int main(int argc, char* argv[]) {

    printf("Day 24\n");

    System data = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %llu\n", puzzle1(data));
    printf("Puzzle 2: %llu\n", puzzle2(data));

    return 0;
}