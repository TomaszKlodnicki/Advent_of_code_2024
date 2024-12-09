#include <iostream>
#include <vector>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#define EXAMPLE_FILENAME "example.txt"
#define PUZZLE_FILENAME "puzzle.txt"
#define USE_EXAMPLE false

#define CHOSE_FILENAME USE_EXAMPLE ? EXAMPLE_FILENAME : PUZZLE_FILENAME

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

struct FileS{
    char blocks;
    int index;
};

uint64_t puzzle1(char* fileContent, size_t fileSize){

    uint64_t retValue = 0;

    char* start = fileContent;
    char* end = fileContent + fileSize - 1;

    uint64_t index = 0;
    uint64_t endIndex = (fileSize + (fileSize % 2)) / 2 - 1;
    uint64_t blockPosition = 0;

    while(end + 2 > start){
        for(char place = '0'; place < *start; place++){
            retValue += blockPosition * index;
            blockPosition++;
        }
        start++;
        index++;
        for(char place = '0'; place < *start; place++){
            
            while(*end == '0' && end > start){
                end -= 2;
                endIndex--;
            }

            if(end < start)
                break;

            (*end)--;

            retValue += blockPosition * endIndex;
            blockPosition++;
        }
        start++;
    }

    return retValue;
}


uint64_t puzzle2(char* fileContent, size_t fileSize){

    uint64_t retValue = 0;

    std::vector<FileS> fileS;

    for(int i = 0; i < fileSize; i++){
        fileS.push_back((FileS){(char)(fileContent[i] - '0'), i % 2 ? 0 : i / 2});
    }

    for(int toMove = fileS.size() - 1; toMove > 0; toMove -= 2){
        for (int freeBlock = 1; freeBlock < toMove; freeBlock += 2){
           
            if(fileS[freeBlock].blocks >= fileS[toMove].blocks && fileS[toMove].blocks != 0){

                fileS[freeBlock].blocks -= fileS[toMove].blocks;
                fileS.insert(fileS.begin() + freeBlock, {(FileS){0, 0}, fileS[toMove]});
                toMove += 2;
                fileS[toMove].index = 0;
                break;

            }

        }
    }

    uint64_t blockPosition = 0;

    for(int i = 0; i < fileS.size(); i++){
        for(char j = 0; j < fileS[i].blocks; j++){
            retValue += fileS[i].index * blockPosition;
            blockPosition++;
        }
    }

    return retValue;
}

int main(int argc, char* argv[]) {

    printf("Day 9\n");

    char* fileContent;
    size_t fileSize;

    if(!mapFileToMemory(CHOSE_FILENAME, &fileContent, &fileSize)){
        printf("Could not map file to memory\n");
        return 1;
    }

    char* fCcopy = (char*)malloc(fileSize*sizeof(char));
    memcpy(fCcopy, fileContent, fileSize);

    printf("Puzzle 1: %llu\n", puzzle1(fCcopy, fileSize));

    memcpy(fCcopy, fileContent, fileSize);

    printf("Puzzle 2: %llu\n", puzzle2(fCcopy, fileSize));

    return 0;
}