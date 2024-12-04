#include <iostream>
#include <regex.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <vector>


#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

#define CHOSE_FILENAME USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME

#define XMAS "XMAS"
#define XMAS_LEN 4

char x_max[4][3][3] = {
    {
        {'M', '.', 'M'},
        {'.', 'A', '.'},
        {'S', '.', 'S'}
    },
    {
        {'M', '.', 'S'},
        {'.', 'A', '.'},
        {'M', '.', 'S'}
    },
    {
        {'S', '.', 'M'},
        {'.', 'A', '.'},
        {'S', '.', 'M'}
    },
    {
        {'S', '.', 'S'},
        {'.', 'A', '.'},
        {'M', '.', 'M'}
    }
};

struct charMat{
    std::vector<std::vector<char>> data;
    unsigned int width = 0;
    unsigned int height = 0;
};

charMat loadMap(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s\n", filename);
        return {};
    }

    charMat cMap;
    char readC;
    std::vector<char> row;
    while((readC = fgetc(fp))){
        if(readC != '\n' && readC != EOF)
            row.push_back(readC);
        else{
            cMap.data.push_back(row);
            cMap.height++;
            row.clear();
        }
        if(readC == EOF)
            break;
    }
    fclose(fp);

    cMap.width = cMap.data[0].size();

    return cMap;
}

int starSearchXmas(charMat& cMat, int cordX, int cordY){

    static const int directions[8][2] = {
        {1, 0},
        {0, 1},
        {1, 1},
        {1, -1},
        {-1, 0},
        {0, -1},
        {-1, -1},
        {-1, 1}
    };

    int foundXmas = 0;

    for (int dir = 0; dir < 8; dir++){
        int x = cordX;
        int y = cordY;
        foundXmas++;
        for(int i = 0; i < XMAS_LEN; i++){

            if(x < 0 || x >= cMat.width || y < 0 || y >= cMat.height){
                foundXmas--;
                break;
            }

            if(cMat.data[y][x] != XMAS[i]){
                foundXmas--;
                break;
            }
            x += directions[dir][0];
            y += directions[dir][1];
        }
    }

    return foundXmas;
}

int X_MAS(charMat& cMat, int cordX, int cordY){

    if(cordX < 0 || cordX > cMat.width - 3 || cordY < 0 || cordY > cMat.height - 3)
        return 0;

    for(int i = 0; i < 4; i++){
        bool mach = true;
        for(int x = 0; x < 3 && mach; x++)
            for(int y = 0; y < 3 && mach; y++)
                if(cMat.data[cordY + y][cordX + x] != x_max[i][y][x] && x_max[i][y][x] != '.')
                    mach = false;
        
        if(mach)
            return 1;
    }
    return 0;
}

int puzzle1(charMat& cMat){
    
    int retValue = 0;

    for(int i = 0; i < cMat.width; i++){
        for(int j = 0; j < cMat.height; j++){
            retValue += starSearchXmas(cMat, i, j);
        }
    }

    return retValue;
}

int puzzle2(charMat& cMat){
    
    int retValue = 0;

    for(int i = 0; i < cMat.width; i++){
        for(int j = 0; j < cMat.height; j++){
            retValue += X_MAS(cMat, i, j);
        }
    }

    return retValue;
}

int main(int argc, char* argv[]) {

    printf("Day 4\n");

    charMat cMap = loadMap(CHOSE_FILENAME);

    printf("Puzzle 1: %d\n", puzzle1(cMap));
    printf("Puzzle 2: %d\n", puzzle2(cMap));

    return 0;
}