//  CITS2002 Project 2 2022
//  Student1:   23433196   NA   LLOYD   50
//  Student2:   22874102   TAN   BRENDAN   50

#include "trove.h"

char *PROGNAME;
char *TROVNAME = "/tmp/trove";
int WORDLEN = 4;
bool BUILD_MODE = false;
bool UPDATE_MODE = false;
bool REMOVE_MODE = false;

// To be used whenever the program is given incorrect options
void usage(void)
{
    fprintf(stderr, "Usage: %s [-f filename]  [-b  |  -r  |  -u]  [-l length]  filelist\nor     %s [-f filename] word\n%s%s%s", PROGNAME, PROGNAME,
                    "where options are:\n  -b		build a new trove-file\n  -f filename	provide the name of the trove-file to be built or searched\n",
                    "  -l length	specify the minimum-length of words added to the trove-file\n  -r		remove information from the trove-file\n",
                    "  -u		update information in the trove-file\n");
    exit(EXIT_FAILURE);
}

// Checks which option was chosen and executes the option
void optionChosen(void){
    if(BUILD_MODE){
        // Executes build option
        printf("    building trove-file\n");
        buildTrove();
    } else if(REMOVE_MODE){
        // Executes remove option
        printf("    reading trove-file\n");
        removeTrove();
    } else if(UPDATE_MODE){
        // Executes update option
        printf("    reading trove-file\n");
        updateTrove();
    }
}

// main function: acquires the command-line options, saves the filelist arguments if required
int main(int argc, char *argv[])
{
    PROGNAME = argv[0];
    int opt;
    bool lInvoked = false;

    while((opt = getopt(argc, argv, OPTLIST)) != -1){
        if(opt == 'f'){
            TROVNAME = strdup(optarg);
        } else if(opt == 'b' && !REMOVE_MODE && !UPDATE_MODE){
            // build a new trove-file from the contents of a filelist. The new trove-file will replace an existing trove-file of the same name. 
            BUILD_MODE = true;
        } else if(opt == 'r' && !BUILD_MODE && !UPDATE_MODE){
            // if any of the files from the filelist appear in the trove-file, remove all of their information from the trove-file. 
            REMOVE_MODE = true;
        } else if(opt == 'u' && !BUILD_MODE && !REMOVE_MODE){
            // update the trove-file with the contents of all files in the filelist (Remove old information first if exists)
            UPDATE_MODE = true;
        } else if(opt == 'l'){
            // only words of at least the indicated positive length should be added to the trove-file (Default is 4)
            lInvoked = true;
            WORDLEN = atoi(optarg);
            if(WORDLEN <= 0) usage(); // word length needs to be greater than 0 
        } else{
            // Option needs a value following it (optarg == ':') or Unknown Option (optarg == '?')
            usage();
        }
    }

    argc -= optind;
    argv += optind;

    if(BUILD_MODE || UPDATE_MODE || REMOVE_MODE){
        // trove is required to build/modify a trove-file
        if(argc > 0){
            ARG_COUNT = argc;
            FILELIST = malloc(ARG_COUNT * sizeof(char *));
            CHECK_ALLOC(FILELIST)
        } else{
            usage(); // error since we are trying to build/modify the tove-file without filelist argument
        }
        for(int i = 0; i < ARG_COUNT; i++){
            FILELIST[i] = strdup(argv[i]);
            CHECK_ALLOC(FILELIST[i]);
        }
        optionChosen();
    } else{
        // trove is required to search for the word in the trove-file
        if(argc != 1 || lInvoked){
            usage(); // error since trove takes only one word as an argument when looking for words & '-l' option should not apply when looking for words
        }
        if(troveSearch(TROVNAME, argv[0])) return (EXIT_SUCCESS); // If at least one pathname is found, success. If not failure.
        else return (EXIT_FAILURE);
    }
    
    if(argc > 0) free(FILELIST);
    return (EXIT_SUCCESS);
}