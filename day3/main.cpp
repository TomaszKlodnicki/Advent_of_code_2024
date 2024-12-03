#include <iostream>
#include <regex.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>


#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

#define CHOSE_FILENAME USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME

#define REGEX_PATTERN_MUL "^(mul\\([0-9]{1,3},[0-9]{1,3}\\))"
#define REGEX_PATTERN_DO "^(do\\(\\))"
#define REGEX_PATTERN_DO_NOT "^(don't\\(\\))"

bool mapFileToMemory(const char* filename, char** fileContent, size_t* fileSize){

    int fd = open(filename, O_RDONLY, S_IRUSR | S_IWUSR);
    struct stat sb;

    if(fstat(fd, &sb) == -1){
        printf("Could not get file size\n");
        return false;
    }

    *fileContent = (char*)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    *fileSize = sb.st_size;
    return true;
}

regex_t compileRegex(const char* pattern){
    regex_t regex;
    int ret = regcomp(&regex, pattern, REG_EXTENDED);
    if(ret != 0){
        printf("Could not compile regex\n");
        exit(1);
    }
    return regex;
}

int countMul(char* cString, size_t len){
    int a, b, retValue = 0;
    static regex_t regex = compileRegex(REGEX_PATTERN_MUL);

    char* search = NULL;
    while((search = (char*)memchr(cString, 'm', len)) != NULL){
        if(regexec(&regex, search, 0, NULL, 0) == 0){
            sscanf(search, "mul(%d,%d)", &a, &b);
            retValue += a * b;
        }
        len -= search - cString + 1;
        cString = search + 1;
    }

    return retValue;
}

int puzzle1(char* fileContent, size_t fileSize){
    
    return countMul(fileContent, fileSize);
}

int puzzle2(char* fileContent, size_t fileSize){
    
    char* search;
    int retValue = 0;

    regex_t regexDo = compileRegex(REGEX_PATTERN_DO);
    regex_t regexDoNot = compileRegex(REGEX_PATTERN_DO_NOT);
    regex_t* inUse = &regexDoNot;

    char* mulSearch = fileContent;
    size_t startFileSize = fileSize;
    char* start = fileContent;

    while((search = (char*)memchr(fileContent, 'd', fileSize)) != NULL){
        if(regexec(inUse, search, 0, NULL, 0) == 0){
            if(inUse ==  &regexDoNot){
                inUse = &regexDo;
                retValue += countMul(mulSearch, search - mulSearch);
            }
            else{
                inUse = &regexDoNot;
                mulSearch = search;
            }
        }
        fileSize -= search - fileContent + 1;
        fileContent = search + 1;
    }

    if(inUse ==  &regexDoNot)
        retValue += countMul(mulSearch, startFileSize - (mulSearch - start));

    return retValue;
}

int main(int argc, char* argv[]) {

    printf("Day 3\n");

    char* fileContent;
    size_t fileSize;

    if(!mapFileToMemory(CHOSE_FILENAME, &fileContent, &fileSize)){
        printf("Could not map file to memory\n");
        exit(1);
    }

    printf("Puzzle 1: %d\n", puzzle1(fileContent, fileSize));
    printf("Puzzle 2: %d\n", puzzle2(fileContent, fileSize));

    return 0;
}