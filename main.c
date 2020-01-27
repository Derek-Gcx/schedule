#include "schedule.h"

#define DEBUG

/*=============================== extern and inlines ===============================*/
#ifdef DEBUG
extern void show(argInfo*);
#endif

extern void usage(void);
extern void execute(argInfo*);
extern void save_info(argInfo*, pid_t);
extern void delete_info(argInfo*, pid_t);

void panic(int i, char* warning) {
    printf("%s\n", warning);
    exit(i);
}

static int lopt;


/*=============================== parse_arg ===============================*/
void parse_arg(int argc, char* argv[], argInfo* arg) {
    arg->name = NULL;
    arg->prog_name = argv[0];
    arg->dest = NULL;
    arg->latency = 0;

    arg->func = NULL;
    for (int index=0; index<MAXSIZE; index++) arg->expr[index]=NULL;

    struct passwd *pwd;
    pwd = getpwuid(getuid());
    arg->user = pwd->pw_name;

    int opt = 0;
    // int lopt;
    int loptind = 0;
    int temp = 0;
    char pattern[] = "-n:d:u:L:";
    static struct option longOpts[] = {
        {"name", required_argument, NULL, 'n'}, 
        {"dest", required_argument, NULL, 'd'}, 
        {"user", required_argument, NULL, 'u'}, 
        {"latency", required_argument, NULL, 'L'},
        {"help", no_argument, &lopt, 1}, 
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
    printf("Usage: schedule [OPTION] ... [FUNC] ... [EXPR] ...\n");
    printf("Execute and manage tasks.\n\n");
    printf("help:\n\tusage and instructions.\n");
    printf("all:\n\tdisplay all tasks.\n");
    printf("kill: [NAME]\n\tterminate a task specified by name.\n");
    printf("run: [OPTIONS]...[EXPR][EXPR]...\n");
    printf("\t%s, %-20s specify name of the task, default to the action followed\n", "-n", "--name");
    printf("\t%s, %-20s specify the user/owner of the tast, default to current user\n", "-u", "--user");
    printf("\t%s, %-20s place the ouput into <file>\n", "-d", "--dest <file>");
    printf("\t%s, %-20s begin task after latency\n", "-L", "--latency");
    printf("\t%-24s info\n", "--help");

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
    assert(arg->name && arg->func);

    pid_t fpid = fork();
    if (fpid<0) {
        panic(2, "fork error!\n");
    }
    else if (fpid==0) {
        execute(arg);
    }
    else {
        ;
    }
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


/* ============================== utils ==============================*/
void execute(argInfo* arg) {
    /* latency */
    if (arg->latency) {
        printf("%s: task will begin after %d seconds.\n", arg->prog_name, arg->latency);
        sleep((unsigned) arg->latency);
    }

    /* output stream */
    FILE* fp = NULL;
    if (arg->dest) {
        fp = fopen(arg->dest, "a");
        if (fp==NULL) panic(3, "empty dest file pointer!\n");
    }
    else
        fp = stdout;

    /* execute */
    printf("%s: task begins.\n", arg->prog_name);
    for (int index=0; index<MAXSIZE; index++) {
        if (arg->expr[index]==NULL) break;

        FILE* output_stream = popen(arg->expr[index], "r");


        if (output_stream==NULL)
            panic(4, "execution failed!\n");
        
        char buf[BUF];
        fprintf(fp, "\n/*================ command 1 ================*/\n");
        while (!feof(output_stream)) {
            fread(buf, BUF, 1, output_stream);
            fprintf(fp, "%s", buf);
            fflush(fp);
        }

        pclose(output_stream);
        break;
    }

    fflush(fp);
    fclose(fp);
    printf("%s: task completed.\n", arg->prog_name);
}

void save_info(argInfo* arg, pid_t pid) {
    /* initialize task info */
    taskInfo task;
    task.name = arg->name;
    task.user = arg->user;
    task.pid = pid;

    /* open file */
    FILE* pool = fopen("/tmp/schedule_pool", "a");
    fprintf(pool, "%s %d %s\n", task.name, task.pid, task.user);
    fflush(pool);
    fclose(pool);
}

void delete_info(argInfo* arg, pid_t pid) {
    char buf1[64];
    int id;
    char buf2[64];

    /* open file */
    FILE* pool = fopen("/tmp/schedule_pool", "r");
    FILE* pool_backup = fopen("/tmp/.schedule_pool", "w");

    while (!feof(pool)) {
        fscanf(pool, "%s %d %s", buf1, &id, buf2);
        if (id==pid) continue;

        fprintf(pool_backup, "%s %d %s\n", buf1, id, buf2);
    }

    system("rm /tmp/schedule_pool");
    system("mv /tmp/.schedule_pool /tmp/schedule_pool");
}


/*=============================== main ===============================*/
int main(int argc, char* argv[]) {
    argInfo arg;
    taskInfo task;

    parse_arg(argc, argv, &arg);

#ifdef DEBUG
    show(&arg);
    run(&arg);
#endif



    return 0;
}

