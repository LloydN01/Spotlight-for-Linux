#include "trove.h"

// updates the trove-file by adding files in filelist argument, if new "-l" is given, apply the new word length only to the files in the filelist argument
void updateTrove(void){
    if(!fileExists(TROVNAME)){
        // If trove-file does not exists, update will need to create one
        UPDATE_MODE = false;
        BUILD_MODE = true;
        buildTrove();
    } else{
        // If trove-file already exists, update the existing trove-file
        PATHNAMES = calloc(1, sizeof(char *));
        CHECK_ALLOC(PATHNAMES);
        hashtable = hashtable_new();
        traverseFilelist();
        readFromTrove(hashtable, TROVNAME, PATHNAMES, PATH_COUNT);

        for(int i = 0; i < PATH_COUNT; i++){
            printf("    finding words in '%s'\n", PATHNAMES[i]);
            openFile(PATHNAMES[i]);
        }

        writeToTrove(hashtable, TROVNAME);
        free(PATHNAMES);
        free(hashtable);
    }
    printf("    trove-file updated\n");
}