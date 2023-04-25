#define _POSIX_C_SOURCE     200809L
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <limits.h> // for PATH_MAX
#include <ctype.h>
#include <stdint.h>

#define OPTLIST ":f:brul:" // If the first character of optstring is a ':', then getopt() likewise does not print an error message.  
#define	CHECK_ALLOC(p) if(p == NULL) { perror(__func__); exit(EXIT_FAILURE); } // A HELPFUL PREPROCESSOR MACRO TO CHECK IF ALLOCATIONS WERE SUCCESSFUL -> Credit to Chris (Workshop-7)

extern char **FILELIST;
extern char **PATHNAMES;
extern int PATH_COUNT;
extern bool *IN_TROVE;
extern int ARG_COUNT;
extern char *PROGNAME; // Program name
extern char *TROVNAME;
extern int WORDLEN;

extern bool BUILD_MODE;
extern bool UPDATE_MODE;
extern bool REMOVE_MODE;


/************************************************************/

//  OUR SIMPLE LIST DATATYPE - A DATA ITEM, AND A POINTER TO ANOTHER LIST
typedef struct _list {
    char           word[BUFSIZ];
    char           pathname[PATH_MAX];
    struct _list   *next;
} LIST;

//  WE DEFINE A HASHTABLE AS A (WILL BE, DYNAMICALLY ALLOCATED) ARRAY OF LISTs
typedef	LIST * HASHTABLE;

extern HASHTABLE *hashtable;

#define	HASHTABLE_SIZE		997 // Arbitrary number. Can be different if needed. Program takes into account collision anyways.

/************************************************************/
// trove.c
extern void usage(void);
extern void optionChosen(void);

/************************************************************/
// findfiles.c
extern void traverseFilelist(void);
extern void findFiles(char *);
extern char *getWord(char *, int);
extern void openFile(char *);

/************************************************************/
// hashtable.c
extern LIST *list_new(void);
extern bool list_find(LIST *, char *, char *);
extern LIST *list_new_item(char *, char *);
extern LIST *list_add(LIST *, char *, char *);
extern uint32_t hash_string(char *);
extern HASHTABLE *hashtable_new(void);
extern void hashtable_add(HASHTABLE *, char *, char *);

/************************************************************/
// build.c
extern void writeToTrove(HASHTABLE *, char *);
extern void readFromTrove(HASHTABLE *, char*,char **, int );
extern void buildTrove(void);

/************************************************************/
// search.c
extern bool isWantedWord(char *, char *);
extern bool fileExists(char *);
extern bool findPathnames(char *, char *);
extern bool troveSearch(char *, char *);

/************************************************************/
// update.c
extern void updateTrove(void);

/************************************************************/
// remove.c
extern bool isFile(char *pathname);
extern void removeTrove(void);