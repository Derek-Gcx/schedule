#include <stdio.h>
#include "schedule.h"


void show(argInfo* arg) {
    printf("prog_name:%s\n", arg->prog_name);
    printf("name:%s\n", arg->name);
    printf("dest:%s\n", arg->dest);
    printf("user:%s\n", arg->user);
    printf("latency:%d\n", arg->latency);
    printf("back:%d\n", arg->back);
    printf("func:%s\n", arg->func);
    for (int index=0; index<MAXSIZE; index++) {
        if (arg->expr[index]==NULL) break;
        printf("    %d: %s\n", index, arg->expr[index]);
    }
}