#include "trove.h"

// Checks if the found word is the wanted word
bool isWantedWord(char *foundWord, char *wanted){
    if(strcmp(foundWord, wanted) == 0){
        return true;
    }
    return false;
}

// Checks if file has been deleted since the trove-file was built
bool fileExists(char *pathname){
    // Copied from https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
    if (access(pathname, F_OK) == 0) {
    // file exists
        return true;
    }
    // file doesn't exist
    return false;
}

// Reads the trove-file  by using zcat and fetches the word & pathname combos. Returns false if no files containing the wanted word is found.
bool findPathnames(char *trovename, char *wanted){
    // findPathnames() was inspired from https://www.geeksforgeeks.org/readwrite-structure-file-c/
    
    printf("    searching for '%s'\n", wanted);
    
    bool successful = false;
    struct _list list;

    int thepipe[2]; // create a pipe
    if(pipe(thepipe) != 0){
        fprintf(stderr, "cannot create pipe\n");
        exit(EXIT_FAILURE);
    }

    printf("    decompressing trove-file\n");
    switch(fork()){
        case -1:
        fprintf(stderr, "fork() failed\n");
        exit(EXIT_FAILURE);
        break;

        case 0: // child process
        close(thepipe[0]); // child will never read from the pipe
        dup2(thepipe[1], STDOUT_FILENO); // redirect stdout to thepipe[1]
        close(thepipe[1]);

        execl("/usr/bin/zcat", "zcat", trovename, (char *)NULL); // output of zcat goes to thepipe[1]
        perror("/usr/bin/zcat");
        exit(EXIT_FAILURE);
        break;

        default: // parent process
        close(thepipe[1]); //parent will never write to the pipe

        // read pipe contents till end of the pipe
        FILE *fp = fdopen(thepipe[0], "rb");
        if(fp == NULL){
            fprintf(stderr, "cannot open pipe descriptor\n");
            exit(EXIT_FAILURE);
        }

        while(fread(&list, sizeof(LIST), 1, fp)){
            if(isWantedWord(list.word, wanted) && fileExists(list.pathname)){
                successful = true; // word is found in the trove-file
                printf("%s\n", list.pathname);
            }
        }

        fclose(fp);
        close(thepipe[0]);
        break;
    }

    return successful;
}

// Searches the trove-file for pathnames. If at least one found -> return true, if not false.
bool troveSearch(char *trovename, char *wanted){
    printf("    reading trove-file\n");
    if(findPathnames(trovename, wanted)) return true;
    return false;
}