#include "schedule.h"

#define DEBUG

/*=============================== extern and inlines ===============================*/
#ifdef DEBUG
extern void show(argInfo*);
#endif

extern void usage(void);
void panic(int i, char* warning) {
    printf("%s\n", warning);
    exit(i);
}


/*=============================== parse_arg ===============================*/
void parse_arg(int argc, char* argv[], argInfo* arg) {

    arg->prog_name = argv[0];
    arg->dest = NULL;
    arg->latency = 0;

    arg->func = NULL;
    for (int index=0; index<MAXSIZE; index++) arg->expr[index]=NULL;

    struct passwd *pwd;
    pwd = getpwuid(getuid());
    arg->user = pwd->pw_name;

    int opt = 0;
    int lopt;
    int loptind = 0;
    int temp = 0;
    char pattern[] = "-n:d:u:L:";
    static struct option longOpts[] = {
        {"name", required_argument, NULL, 'n'}, 
        {"dest", required_argument, NULL, 'd'}, 
        {"user", required_argument, NULL, 'u'}, 
        {"latency", required_argument, NULL, 'L'},
        {"help", no_argument, NULL, 1}, 
        {0, 0, 0, 0}
    };

    while((opt = getopt_long(argc, argv, pattern, longOpts, &loptind)) != -1) {
        switch (opt) {
            case 0: {
                switch (lopt) {
                    case 1: 
                        #ifdef DEBUG
                            printf("option loption 1\n");
                        #endif
                        usage(); break;
                    default: usage(); panic(0, "unexpected option. Abort."); break;
                }; break;
            }
            case 'n': 
                #ifdef DEBUG
                    printf("option n\n");
                #endif
                arg->name = optarg; break;
            case 'u': 
                #ifdef DEBUG
                    printf("option u\n");
                #endif
                arg->user = optarg; break;
            case 'd': 
                #ifdef DEBUG
                    printf("option d\n");
                #endif
                arg->dest = optarg; break;
            case 'L': 
                #ifdef DEBUG
                    printf("option L\n");
                #endif
                arg->latency = atoi(optarg); break;
            case 1: {
                #ifdef DEBUG
                    printf("option 1\n");
                #endif
                if (arg->func==NULL) arg->func = optarg;
                else {
                    arg->expr[temp] = optarg;
                    temp++;
                };
                break;
            }
            default: usage(); panic(1, "unexpected option. Abort."); break;
        }
    }

    if (arg->name==NULL) arg->name = arg->func;
}

/*=============================== functions ===============================*/
void usage() {
    // printf("Usage: schedule [OPTION] ... [FUNC] ... [EXPR] ...\n");
    // printf("Execute and manage tasks.\n\n");
    // printf("help:\n\tusage and instructions.\n");
    // printf("all:\n\tdisplay all tasks.\n");
    // printf("kill: [NAME]\n\tterminate a task specified by name.\n");
    // printf("run: [OPTIONS]...[EXPR][EXPR]...\n\t-n, --name")

;

}

void help(argInfo* arg) {
    usage();
}

void all(argInfo* arg) {
    ;
}

void kill(argInfo* arg) {
    ;
}

void run(argInfo* arg) {
    ;
}

struct funcTable {
    char* func_name; 
    void (*func)(argInfo*);
} func_table[] = {
    {"usage", help}, 
    {"help", help}, 
    {"all", all}, 
    {"kill", kill}, 
    {"run", run},
};


/*=============================== main ===============================*/
int main(int argc, char* argv[]) {
    argInfo arg;
    taskInfo task;

    parse_arg(argc, argv, &arg);

#ifdef DEBUG
    show(&arg);
#endif



    return 0;
}

