#include <stdio.h>
#include <stdlib.h>

int main(void){
    char *tmpdir = getenv("TMPDIR");
    fprintf(stderr, "tmpdir at %p: '%s'\n", tmpdir, tmpdir);
    return 0;
}
