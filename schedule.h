#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <assert.h>
// #include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <pwd.h>

/*=============== types ===============*/
#define MAXSIZE 10
#define BUF 4096
#define true 1
#define false 0

typedef struct argInfo argInfo;
typedef struct taskInfo taskInfo;
typedef __pid_t pid_t;
typedef char bool;


struct argInfo {
    char* name;
    char* prog_name;
    char* dest;
    char* user;
    int latency;
    char* func;
    char* expr[MAXSIZE];
    bool back;
};

struct taskInfo {
    char* name;
    char* user;
    pid_t pid;
};