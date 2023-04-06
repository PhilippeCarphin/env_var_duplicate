#include <stdio.h>
#include <string.h>
#include <unistd.h>

extern char **environ;

int main(void)
{
    char tmpdir_name[] = "TMPDIR=";
    size_t len = strlen(tmpdir_name);
    for(char **varp = environ; *varp != NULL; varp++){
        char *var = *varp;
        if(strncmp(var, tmpdir_name, len) == 0){
            fprintf(stderr, "TMPDIR entry at %p: '%s'\n", var, var);
        }
        // fprintf(stderr, "var entry at %p: '%s'\n", var, var);
    }
    return 0;
}

