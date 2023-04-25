#include "trove.h"

HASHTABLE *hashtable;

// reads the hashtable struct by struct and compresses data using gzip -> adds the compressed data to trove-file
void writeToTrove(HASHTABLE *hashtable, char *trovename){
    // writeToTrove() was inspired from https://www.geeksforgeeks.org/readwrite-structure-file-c/

    printf("    writing trove-file\n");

    FILE *trove = fopen (trovename, "wb"); // open file for writing
    if (trove == NULL){
        fprintf(stderr, "cannot open file: '%s'\n", trovename); // Can we open this file
        exit(EXIT_FAILURE);
    }

    int thepipe[2]; // create a pipe
    if(pipe(thepipe) != 0){
        fprintf(stderr, "cannot create pipe\n");
        exit(EXIT_FAILURE);
    }

    printf("    compressing trove-file\n");
    switch(fork()){
        case -1:
        fprintf(stderr, "fork() failed\n");
        exit(EXIT_FAILURE);
        break;

        case 0: // child process
        close(thepipe[1]); // child will never write to pipe
        dup2(thepipe[0], STDIN_FILENO); // copies thepipe[0] into STDIN_FILENO
        close(thepipe[0]); // close the reading end of the pipe

        dup2(fileno(trove), STDOUT_FILENO); // redirect stdout to trove-file, now gzip's compressed output will go to trove-file

        execl("/usr/bin/gzip", "gzip", (char *)NULL);
        perror("/usr/bin/gzip"); // execl should have replaced the calling process image with the new process image. If you reach this step -> error
        exit(EXIT_FAILURE);
        break;

        default: // parent process
        close(thepipe[0]); // parent will never read from pipe

        for(int i = 0; i < HASHTABLE_SIZE; i++){
            LIST *list = hashtable[i];

            if(list != NULL) {
                    while(list != NULL) {

                        // printf("    word: %s | pathname: %s\n", list->word, list->pathname); // For debugging only -> Allows you to check which word and pathname combo is stored in trove-file

                        write(thepipe[1], list, sizeof(LIST));
                        list = list->next;
                    }
            }
        }
        close(thepipe[1]); //inform the child that no more data is coming

        wait(NULL);
        break;
    }
    fclose (trove);
}

// reads the trove-file and rebuilds the hashtable by adding decompressed data
void readFromTrove(HASHTABLE *hashtable, char *trovename, char **pathnames, int pathname_count){
    // readFromTrove() was inspired from https://www.geeksforgeeks.org/readwrite-structure-file-c/
    
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

        execl("/usr/bin/zcat", "zcat", trovename, (char *)NULL);
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

        // read file contents till end of file
        while(fread(&list, sizeof(LIST), 1, fp)){
            bool needsRemoval = false;
            for(int i = 0; i < pathname_count; i++){
                if(strcmp(list.pathname, pathnames[i]) == 0){
                    // If pathname of file in filelist argument is in trove-file -> Ignore -> Practically removing it from future trove-file
                    needsRemoval = true;
                    if(REMOVE_MODE){
                        IN_TROVE[i] = true; // if file is in the filelist -> true, if file is not in the filelist -> leave as false
                    }
                    break;
                }
            }
            if(!needsRemoval){ // If current list's pathname is not in the filelist argument -> Don't remove, add to hashtable again.
                hashtable_add(hashtable, list.word, list.pathname);
            }
        }

        fclose(fp);
        close(thepipe[0]);
        break;
    }
}

// builds a compressed trove-file by creating a hashtable that indexes the words and writes that compressed data to trove-file
void buildTrove(void){
    hashtable = hashtable_new();
    traverseFilelist();
    writeToTrove(hashtable, TROVNAME);
    free(hashtable);
    printf("    trove-file built\n");
}