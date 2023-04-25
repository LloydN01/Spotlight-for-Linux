#include "trove.h"

// checks if given file is indeed a file
bool isFile(char *name){
    char path[PATH_MAX];
    char *pathPtr = realpath(name, path);
    if(pathPtr == NULL){
        fprintf(stderr, "cannot open file: '%s'\n", name); // Can we open this file
        exit(EXIT_FAILURE);
    }

    struct stat stat_buffer;
    struct stat *statPtr = &stat_buffer;

    if(stat(path, statPtr) != 0){
        fprintf(stderr, "cannot check '%s' using stat()\n", path);
        exit(EXIT_FAILURE);
    } else if(S_ISREG(statPtr -> st_mode)){
        // Does this pathname belong to a file
        return true;
    }

    return false;
}

// removes the files in the filelist argument from the trove-file
void removeTrove(void){
    PATHNAMES =  calloc(1, sizeof(char *));
    CHECK_ALLOC(PATHNAMES);
    hashtable = hashtable_new();
    traverseFilelist();

    IN_TROVE = calloc(PATH_COUNT, sizeof(bool));
    CHECK_ALLOC(IN_TROVE);
    for(int i = 0; i < PATH_COUNT; i++){
        IN_TROVE[i] = false; // for future use, checks if all files in the filelist argument exists in the original trove-file
    }
    readFromTrove(hashtable, TROVNAME, PATHNAMES, PATH_COUNT);

    for(int i = 0; i < PATH_COUNT; i++){
        if(!IN_TROVE[i]){
            // pathname from filelist argument was not found in the trove-file
            fprintf(stderr, "'%s' is not in trove-file\n", PATHNAMES[i]);
            exit(EXIT_FAILURE);
        }
    }

    writeToTrove(hashtable, TROVNAME);
    free(PATHNAMES);
    free(IN_TROVE);
    free(hashtable);

    printf("    successful removal from trove-file\n");
}