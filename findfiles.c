#include "trove.h"

char **FILELIST; // stores he strings given through filelist argument
char **PATHNAMES; // stores all the pathnames to files that belongs to the file/directory in filelist argument
int PATH_COUNT = 0; // Keeps the number of pathnames stored in PATHNAMES array
bool *IN_TROVE; // Used in remove_mode to check if all pathnames in filelist argument actually exists in the trove-file
int ARG_COUNT;
int WORD_COUNT = 0; // This is for debugging purposes only. Used to check how many words were found in each file

// Helper function to findFiles(): It passes the filelist arguments to findFiles() to recursively traverse all the files that are within the given dir
void traverseFilelist(){
    for(int i = 0; i < ARG_COUNT; i++){
        if(UPDATE_MODE){
            printf("    updating '%s' in trove-file\n", FILELIST[i]);
        } else if(REMOVE_MODE){
            printf("    removing '%s' from trove-file\n", FILELIST[i]);
            if(!isFile(FILELIST[i])){
                // filelist argument is not a file and instead is a direcotory -> throw error when in remove mode
                fprintf(stderr, "'%s' is not in trove-file\n", FILELIST[i]);
                exit(EXIT_FAILURE);
            }
        }
        findFiles(FILELIST[i]);
    }
}

// Function is called when building/modifying a trove-file, (recursively) traverse the filelist to find all of the files to be indexed.
void findFiles(char *name){
    char path[PATH_MAX];
    char *pathPtr = realpath(name, path);
    if(pathPtr == NULL){
        fprintf(stderr, "cannot find the realpath of '%s'\n", name); // Can we open this file
        exit(EXIT_FAILURE);
    }

    struct stat stat_buffer;
    struct stat *statPtr = &stat_buffer;
    
    if(stat(path, statPtr) != 0){
        fprintf(stderr, "cannot check '%s' using stat()\n", path); // Can we open this file
        exit(EXIT_FAILURE);
    } else if(S_ISREG(statPtr -> st_mode)){
        // If this name belongs to a file

        if(BUILD_MODE){
            printf("    finding words in '%s'\n", path);
            openFile(path);
        } else{ // Not in build mode -> in update or remove mode
            PATHNAMES = realloc(PATHNAMES, ++PATH_COUNT * sizeof(char *));
            CHECK_ALLOC(PATHNAMES);
            PATHNAMES[PATH_COUNT - 1] = strdup(path);
            CHECK_ALLOC(PATHNAMES[PATH_COUNT - 1]);
        }
    } else if(S_ISDIR(statPtr -> st_mode)){
        // If this name belongs to a directory

        printf("    reading directory '%s'\n", path);

        DIR *dirp = opendir(path);
        CHECK_ALLOC(dirp);
        struct dirent *dp;
        while((dp = readdir(dirp)) != NULL){
            if(strcmp(dp -> d_name, ".") != 0 && strcmp(dp -> d_name, "..") != 0){
                char fullpath[PATH_MAX];
                strcpy(fullpath, path);
                strcat(fullpath, "/");
                strcat(fullpath, dp -> d_name); // fullpath == absolute pathname to the newly found file/dir
                findFiles(fullpath);
            }
        }
        closedir(dirp);
    }
}

// returns the alphanumeric word
char *getWord(char *word, int currLen){
    WORD_COUNT++;
    word = realloc(word, currLen + 1);
    CHECK_ALLOC(word);
    word[currLen] = '\0';

    return word;
}

// opens the given file and acquires the alphanumeric words by building char by char
void openFile(char *filePath){
    FILE *fp = fopen(filePath, "rb");
    if(fp == NULL){
        fprintf(stderr, "cannot open file '%s'\n", filePath);
        exit(EXIT_FAILURE);
    }
    char ch;
    size_t got;
    int currLen = 0;
    char *word = calloc(1, 1);
    CHECK_ALLOC(word);
    while((got = fread(&ch, 1, 1, fp)) > 0){
        if(isalnum(ch)){
            word[currLen++] = ch;
            word = realloc(word, currLen + 1);
            CHECK_ALLOC(word);
        } else{
            // if current charcater is not a alphanumeric characters
            if(currLen >= WORDLEN){
                word = getWord(word, currLen);
                hashtable_add(hashtable, word, filePath);
            }
            currLen = 0;
        }
    }
    if(currLen >= WORDLEN){ // make word one last time for the possible word at the EOF
        word = getWord(word, currLen);
        hashtable_add(hashtable, word, filePath);
    }
    free(word);
    fclose(fp);
    
    printf("    found %i words\n", WORD_COUNT);
    WORD_COUNT = 0;
}