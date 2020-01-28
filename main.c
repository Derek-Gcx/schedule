#include "schedule.h"

// #define DEBUG

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
    arg->back = false;

    arg->func = NULL;
    for (int index=0; index<MAXSIZE; index++) arg->expr[index]=NULL;

    struct passwd *pwd;
    pwd = getpwuid(getuid());
    arg->user = pwd->pw_name;

    int opt = 0;
    // int lopt;
    int loptind = 0;
    int temp = 0;
    char pattern[] = "-n:d:u:L:b";
    static struct option longOpts[] = {
        {"name", required_argument, NULL, 'n'}, 
        {"dest", required_argument, NULL, 'd'}, 
        {"user", required_argument, NULL, 'u'}, 
        {"latency", required_argument, NULL, 'L'},
        {"back", no_argument, NULL, 'b'},
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
            case 'b':
                #ifdef DEBUG
                    printf("option b\n");
                #endif
                arg->back = true; break;
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
    printf("terminate: [NAME]\n\tterminate a task specified by name.\n");
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
    assert(strcmp(arg->func, "all")==0);
    FILE* pool = fopen("/tmp/schedule_pool", "r");
    char buf1[BUF];
    char buf2[BUF];
    int pid;

    printf("%-20s%-20s%-20s\n", "Name", "user", "pid");
    while (!feof(pool)) {
        fscanf(pool, "%s %d %s\n", buf1, &pid, buf2);
        printf("%-20s%-20d%-20s\n", buf1, pid, buf2);
    }
    fflush(pool);
    fclose(pool);
}

void terminate(argInfo* arg) {
    assert(strcmp(arg->func, "terminate")==0);
    assert(arg->name);

    FILE* pool = fopen("/tmp/schedule_pool", "r");
    char buf1[BUF];
    char buf2[BUF];
    int pid;

    while (!feof(pool)) {
        fscanf(pool, "%s %d %s\n", buf1, &pid, buf2);
        if (strcmp(arg->name, buf1)==0) {
            char buf3[BUF];
            sprintf(buf3, "kill %d", pid);
            system(buf3);
            // no need to worry about /tmp/achedule_pool.
            break;
        }
        else assert(0);
    }
    fclose(pool);
}

void run(argInfo* arg) {
    assert(strcmp(arg->func, "run")==0);
    assert(arg->name && arg->func);

    if (arg->back) assert(arg->dest);

    if( arg->back ){
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
    else {
        execute(arg);
    }
    return;
}

struct funcTable {
    char* func_name; 
    void (*func)(argInfo*);
} func_table[] = {
    {"usage", help}, 
    {"help", help}, 
    {"all", all}, 
    {"terminate", terminate}, 
    {"run", run},
};


/* ============================== utils ==============================*/
void execute(argInfo* arg) {
    /* latency */
    if (arg->latency) {
        printf("%s: task will begin after %d seconds.\n", arg->prog_name, arg->latency);
        sleep((unsigned) arg->latency);
    }

    /* redirection via dup */
    if (arg->dest!=NULL) {
        int dest_fd = open(arg->dest, O_WRONLY);
        dup2(dest_fd, 1);
        close(dest_fd);
    }

    /* execute */
    printf("%s: task begins.\n\n", arg->prog_name);
    for (int index=0; index<MAXSIZE; index++) {
        if (arg->expr[index]==NULL) break;

        // '\n' required! or every subprocess will add this string to printf buffer!
        printf("\n/*=============== task %d: %s ===============*/\n\n", index, arg->expr[index]);
        pid_t ffpid = fork();

        if (ffpid<0) panic(5, "fork error!\n");
        else if (ffpid==0){

            /* exevp_str */
            char* execvp_str[MAXSIZE];
            char* temp;
            int i=0;
            while((temp=strsep(&arg->expr[index], " "))!=NULL) {
                execvp_str[i] = temp;
                i++;
            }
            execvp_str[i] = NULL;

            /* execution */
            execvp(execvp_str[0], execvp_str);
            printf("Error while executing command, plz check the expr. Abort.\n");
            exit(0);
        }
        else {
            save_info(arg, ffpid);
            int status;
            waitpid(ffpid, &status, 0);
            delete_info(arg, ffpid);
        }
    }
    printf("\n\n%s: task completed.\n", arg->prog_name);
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
    fclose(pool);
    fclose(pool_backup);
}


/*=============================== main ===============================*/
int main(int argc, char* argv[]) {
    argInfo arg;
    taskInfo task;

    parse_arg(argc, argv, &arg);

#ifdef DEBUG
    show(&arg);
#endif

    /* look up function */

    for (int i=0; i<sizeof(func_table)/sizeof(func_table[0]); i++) {
        if (strcmp(func_table[i].func_name, arg.func)==0) {
            func_table[i].func(&arg);
            break;
        }
    }
    return 0;
}

