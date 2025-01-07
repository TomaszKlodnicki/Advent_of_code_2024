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

struct Computer{
    char name[2];

    bool operator==(const Computer& other) const{
        return name[0] == other.name[0] && name[1] == other.name[1];
    } 

    bool operator>(const Computer& other) const{
        return memcmp(name, other.name, 2) > 0;
    }

    bool operator<(const Computer& other) const{
        return memcmp(name, other.name, 2) < 0;
    }
};

template<>
struct std::hash<Computer>{
    std::size_t operator()(const Computer& c) const noexcept
    {
        return c.name[0] | (c.name[1] << 8);
    }
};

struct Network{
    std::unordered_map<Computer, std::vector<Computer>> connections;
};

Network loadMap(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s\n", filename);
        exit(1);
    }

    Network data;
    Computer conn[2];
    
    while(fscanf(fp, "%c%c-%c%c\n", &conn[0].name[0], &conn[0].name[1], &conn[1].name[0], &conn[1].name[1] ) != EOF){
        data.connections[conn[0]].push_back(conn[1]);
        data.connections[conn[1]].push_back(conn[0]);
    }

    fclose(fp);

    for(auto& conn : data.connections){
        std::sort(conn.second.begin(), conn.second.end());
    }

    return data;
}

struct Trio{
    Computer comp[3];

    bool operator==(const Trio& other) const{
        return comp[0] == other.comp[0] && comp[1] == other.comp[1] && comp[2] == other.comp[2];
    }
};

template<>
struct std::hash<Trio>{
    std::size_t operator()(const Trio& t) const noexcept
    {
        return t.comp[0].name[0] | (t.comp[0].name[1] << 8) | (t.comp[1].name[0] << 16) | (t.comp[1].name[1] << 24) | ((size_t)t.comp[2].name[0] << 32) | ((size_t)t.comp[2].name[1] << 40);
    }
};

uint64_t puzzle1(Network& input){

    std::unordered_set<Trio> visited;

    for(auto& conn : input.connections){
        if((conn.first.name[0] != 't'))
            continue;

        for(auto& comp : conn.second){

            std::vector<Computer> commonComps;
            std::set_intersection(conn.second.begin(), conn.second.end(), input.connections[comp].begin(), input.connections[comp].end(), std::back_inserter(commonComps));

            for(auto& commonC : commonComps){
                std::vector<Computer> compVec = {conn.first, comp, commonC};
                std::sort(compVec.begin(), compVec.end());
                visited.insert({compVec[0], compVec[1], compVec[2]});
            }
        }
    }

    return visited.size();
}

std::vector<Computer> recursiveCommon(Network& input, std::unordered_set<Computer> visited, std::vector<Computer> common, std::vector<Computer> party, uint16_t depth = 1){

    if(common.empty())
        return party;

    std::vector<Computer> retValue = {};

    for(auto& c : common){
        if(visited.find(c) != visited.end())
            continue;
        visited.insert(c);
        std::vector<Computer> newCommon;
        std::vector<Computer> newParty = party;
        newParty.push_back(c);
        std::set_intersection(common.begin(), common.end(), input.connections[c].begin(), input.connections[c].end(), std::back_inserter(newCommon));
        auto nerRet = recursiveCommon(input, visited, newCommon, newParty, depth + 1);
        if(retValue.size() < nerRet.size())
            retValue = nerRet;
    }

    return retValue;
}

uint64_t puzzle2(Network& input){

    std::unordered_map<Computer, std::vector<Computer>> lanParty;
    std::unordered_set<Computer> visited;

    for(auto& conn : input.connections){
        lanParty[conn.first] = recursiveCommon(input, visited, conn.second, {conn.first});
        visited.insert(conn.first);
    }

    std::vector<Computer> max = std::max_element(lanParty.begin(), lanParty.end(), [](auto& a, auto& b){return a.second.size() < b.second.size();})->second;

    std::sort(max.begin(), max.end());
    for(auto& c : max){
        printf("%c%c,", c.name[0], c.name[1]);
    }

    printf("\n");

    return max.size();
}

int main(int argc, char* argv[]) {

    printf("Day 23\n");

    Network data = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %llu\n", puzzle1(data));
    printf("Puzzle 2: %llu\n", puzzle2(data));

    return 0;
}