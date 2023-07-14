#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

extern char **environ;
char **new_env;

int create_new_env();
int run(const char *program, const char *message, char **new_env);

int main(int argc, char **argv)
{
    create_new_env();
    fprintf(stderr, "\033[1;35m============== C ====================\033[0m\n");
    run("print_all_tmpdir",         "",                                    new_env);
    run("getenv_tmpdir",            "getenv(\"TMPDIR\")",                  new_env);
    fprintf(stderr, "\033[1;35m============== BASH ====================\033[0m\n");
    run("echo_tmpdir.sh",           "echo $TMPDIR",                        new_env);
    run("run_print_all_tmpdir.sh",  "calling print_all_tmpdir",            new_env);
    run("exec_print_all_tmpdir.sh", "exec-ing print_all_tmpdir",           new_env);
    run("env_grep_tmpdir.sh",       "env | grep tmpdir",                   new_env);
    fprintf(stderr, "\033[1;35m============== PYTHON ====================\033[0m\n");
    run("print_tmpdir.py",          "print(os.environ['TMPDIR'])",         new_env);
    run("run_env.py",               "subprocess.run('/usr/bin/env')",      new_env);
    run("run_print_all_tmpdir.py",  "subprocess.run('./print_all_tmpdir'", new_env);
    fprintf(stderr, "\033[1;35m============== GO ====================\033[0m\n");
    run("go_print_all_tmpdir",      "for _, v in os.Environ() {...}",      new_env);
    run("go_getenv_tmpdir",         "os.Getenv(\"TMPDIR\")",               new_env);
    fprintf(stderr, "\033[1;35m============== RUST ====================\033[0m\n");
    run("rust_print_all_tmpdir",    "for (k, v) in std::env::vars() {...}",new_env);
    run("rust_getenv_tmpdir",       "std::env::var(\"TMPDIR\")",           new_env);
    // run("./infinite_loop.sh", "Infinite loop for inspection with htop", new_env);

    return 0;
}


int run(const char *program, const char *message, char **new_env){

    fprintf(stderr, "\033[36mPARENT PROCESS\033[0m: \033[1;32m%-25s\033[0m: \033[33m%s\033[0m\n", program, message);
    int err;
    pid_t pid = fork();
    if(pid == 0){
        err = execle(program, program, NULL, new_env);
        if(err){
            perror("execle");
            exit(1);
        }
    }
    waitpid(pid, NULL, 0);
    return 0;
}


size_t count_env_vars(char **envp);
int create_new_env(){
    size_t nb_vars = count_env_vars(environ);
    new_env = malloc((nb_vars + 1 + 1 + 1) * sizeof(char*));
    // char *new_env[nb_vars+2];
    for(int i = 0; i < nb_vars; i++){
        new_env[i] = environ[i];
    }
    new_env[nb_vars] = strdup("TMPDIR=/tmp/other-tmp-dir");
    new_env[nb_vars+1] = NULL;
    return 0;
}


size_t count_env_vars(char **envp)
{
    size_t nb_vars = 0;
    for(char **varp = envp; *varp != NULL; varp++){
        nb_vars++;
    }
    return nb_vars;
}

