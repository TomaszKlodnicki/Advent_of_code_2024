#include <iostream>
#include <vector>
#include <stdio.h>
#include <string>
#include <unordered_map>

#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

#define CHOSE_FILENAME USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME

struct Patterns{
    std::vector<std::string> patterns;
    std::vector<std::string> designs;
};

Patterns loadMap(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s\n", filename);
        exit(1);
    }

    Patterns data;
    char buffer[100] = "";

    while(fscanf(fp, " %99[^,\n]", buffer) != EOF){
        data.patterns.push_back(std::string(buffer));
        if(fgetc(fp) == '\n')
            break;
    }

    while(fscanf(fp, "%99s", buffer) != EOF){
        data.designs.push_back(buffer);
    }
        
    fclose(fp);

    return data;
}

bool checkDesign(std::string design, const std::vector<std::string>& patterns){
    
    static std::unordered_map<std::string, bool> checkedDesigns = {{"", true}};

    if(checkedDesigns.find(design) != checkedDesigns.end())
        return checkedDesigns[design];

    for(auto& pattern : patterns){
        size_t dIndex = design.find(pattern);
        if(dIndex != std::string::npos){
            std::string nextDesign = std::string(design.begin() + dIndex + pattern.size(), design.end());
            std::string prevDesign = std::string(design.begin(), design.begin() + dIndex);
            if(checkDesign(prevDesign, patterns) && checkDesign(nextDesign, patterns)){
                checkedDesigns[design] = true;
                return true;
            }
        }
    }

    checkedDesigns[design] = false;
    return false;
}

uint64_t countDesignPatterns(std::string design, const std::vector<std::string>& patterns){
    
    static std::unordered_map<std::string, uint64_t> checkedDesigns;

    if(checkedDesigns.find(design) != checkedDesigns.end())
        return checkedDesigns[design];

    checkedDesigns[design] = 0;

    for(auto& pattern : patterns){
        if(design.find(pattern) == 0){
            if(pattern.size() == design.size()){
                checkedDesigns[design]++;
                continue;
            }
            std::string nextDesign = std::string(design.begin() + pattern.size(), design.end());

            uint64_t nextCount = countDesignPatterns(nextDesign, patterns);
            checkedDesigns[design] += nextCount;
        }
    }

    return checkedDesigns[design];
}

uint32_t puzzle1(Patterns patterns){

    uint32_t retValue = 0;

    for(auto& design : patterns.designs){
        if(checkDesign(design, patterns.patterns)){
            retValue++;
        }
    }

    return retValue;
}

uint64_t puzzle2(Patterns& patterns){

    uint64_t retValue = 0;

    for(auto& design : patterns.designs){
        retValue += countDesignPatterns(design, patterns.patterns);
    }

    return retValue;
}

int main(int argc, char* argv[]) {

    printf("Day 19\n");

    Patterns patterns = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %u\n", puzzle1(patterns));
    printf("Puzzle 2: %llu\n", puzzle2(patterns));

    return 0;
}