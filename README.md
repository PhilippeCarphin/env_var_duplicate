# Duplicate environemnt variables

Launch various programs with `execle(prog, arg0, NULL, envp)` with an array
`envp` containing two strings beginning with `"TMPDIR="` creating a situation
with an ambiguous value for `TMPDIR` in the child processes.

Run
```
make
```
and look at the output.

# Explanations

> The variable environ points to an array of pointers to strings called
> the "environment".  The last pointer in this array has the value NULL.
> This array of strings is made available to the process by the execve(2)
> call when a new program is started.  When a child process is created via
> fork(2), it inherits a copy of its parent's environment.
>
> By convention, the strings in environ have the form "name=value".  The
> name is case-sensitive and may not contain the character "=".  The value
> can be anything that can be represented as a string.  The name and the
> value may not contain an embedded null byte ('\0'), since this is
> assumed to terminate the string.

From [man 7 environ](https://man7.org/linux/man-pages/man7/environ.7.html).

We see that the environment has very little structure and that nothing in that
structure itself prevents us from having two strings beginning with `TMPDIR=`.

In situations where the parent process must craft a new environment for a child
process that it wants to create to pass as the `envp` argument of `execve` or
`execle`, the programmer must ensure that there are not more than one entry
beginning with `TMPDIR=` or risk creating undefined behavior.

Different languages provide functions that access the environment in different
ways that give the same behavior if there are no duplicate definitions of the
same environemnt variables but will give different results if there are duplicate
definitions.

The demonstration shows the result and the following sections explain why those
results are the way they are.

# C

The standard library function `getenv` loops over the array and returns when it
finds a result.  It therefore returns the first value of `TMPDIR`.

# Python

During its initalization, the `os` module creates a Python `dict` object.  The
code loops over the strings of `environ` (see [`convertenviron`](https://github.com/python/cpython/blob/main/Modules/posixmodule.c#L1563)):

``` C
extern char **environ;
PyObject *convertenviron(void){
    PyObject *d = PyDict_New();
    for(char **e = environ; *e != NULL; e++){
        const char *eq = strchr(*e, '=');
        // create a python string object from the part of the C string *e
        // that goes up to the first equal sign.
        PyObject *k = PyBytes_FromStringAndSize(*e, (int)(p - *e));
        // Create a Python string object from the part of the C string *e
        // that comes after the first equal sign
        PyObject *v = PyBytes_FromStringAndSize(eq+1, strlen(eq+1));

        PyDict_SetDefault(d, k, v);
    }
}
```

Since [`PyDict_SetDefault(d, k, v)`](https://docs.python.org/3/c-api/dict.html#c.PyDict_SetDefault) sets the value for the key `k` in the dictionary *only if `k` is not present*, this means that `os.environ['TMPDIR']` will have the "first" value of `TMPDIR`.

I believe that [`PyDict_SetDefault(d, k, v)`](https://docs.python.org/3/c-api/dict.html#c.PyDict_SetDefault) was chosen instead of [`PyDict_SetItem(d, k, v)`](https://docs.python.org/3/c-api/dict.html#c.PyDict_SetItem) to get the same behavior as the C library function `getenv()`.

## BASH

BASH, like Python, reads the environment into one of its own data structures
at startup.

We have [`shell.c`](https://github.com/bminor/bash/blob/master/shell.c#L370) which contains
``` C
static char **shell_environment;
int main(int argc, char **argv, char **env){
    // [...]
    shell_environment = env;
    // [...]
    shell_initialize();
    // [...]
}

static void shell_initialize(){
    initialize_shell_variables(shell_environment, ...);
}
```

Then in [`variables.c`](https://github.com/bminor/bash/blob/master/variables.c#L363), we find the iteration over the strings of the environment:
``` C
void initialize_shell_variables(char **env, int privmode){
    for (string_index = 0; env && (string = env[string_index++]); ){
        char *name = string;
        while(*string++ != '=') ; 
        /* Now string points after the first '=' */
        char_index = string - name - 1;
        name[char_index] = '\0';
        /* Now 'string' is the variable's value and 'name' is the variable's name */
        if(/* name is BASH_FUNC_<funcname>%% */){
            /* define a shell function from variables string */
        } else if(/* name is BASH_ARRAY_<arrayname>%% */){
            /* Define a bash array from string */
        } else if(/* name is BASH_ASSOC_<arrayname>%% */){
            /* Define an associative array from string */
        } else {
            // [...]
            if(legal_identifier(name)){
                temp_var = bind_variable(name, string, 0);
                // [...]
            }
            // [...]
        }
        name[char_index] = '=';  Leave the real environment unmodified
        // [...]
        }
    // [...]
}
```

Now we need to look at `bind_variable`:
```
SHELL_VAR *bind_variable(const char *name, char *value, int flags){
  /* bind_variable_internal will handle nameref resolution in this case */
  return (bind_variable_internal (name, value, global_variables->table, 0, flags));
}
static SHELL_VAR *bind_variable_internal (
    const char *name, char *value, HASH_TABLE *table, int hflags, int aflags)
{
    // [...]
}
```

Looking at the code of `bind_variable_internal()` I couldn't find anything that
seemed like it looked to see if there was already an entry with key 'name' in
the `HASH_TABLE`.

This leads me to believe that the reason why BASH gives us the second value of
`TMPDIR` is that `bind_variable()` replaces the value associated to the key
`name` if that key is already present.

Looking at the code of BASH while searching for the functions mentionned above,
I believe that BASH never lets child processes inherit from its environment but
instead, uses its internal `HASH_MAP` to create an environment for child processes.

This is because when I launch a BASH script that calls my program
`print_all_tmpdir`, the output we get shows only a single `TMPDIR` with the
second value.  That is `print_all_tmpdir` gets a cleaned up environment in
which the second value of variables was chosen.

This could be confirmed by looking in `execute_cmd.c` but I have not been able
to find the exact bits of code that would confirm my suspicion.

## Go

TODO: Find the go code where the real `char **environ` is read into the array
of go strings returned by `os.Environ()` and the code of `os.Getenv`.  And explain
why there are no duplicates in the array returned by `os.Environ` and why
`os.Getenv` returns the first value.

- [`os.Environ()`](https://cs.opensource.google/go/go/+/master:src/os/env.go;l=137;bpv=0;bpt=0): The function that users are meant to call to get a slise of strigs.
- [`syscall.Environ()`](https://cs.opensource.google/go/go/+/refs/tags/go1.20.3:src/syscall/env_unix.go;l=34): `os.Environ()` delegates to this function from the syscall package.
- [`syscall.copyenv()`](https://cs.opensource.google/go/go/+/master:src/syscall/env_unix.go;l=139;bpv=0;bpt=0): `syscall.Environ()` starts by calling `copyenv`
which takes a slice of strings given by the `runtime` package.  This is the
function that removes duplicates.

The `syscall.copyenv()` function takes environment data from the global
variable `envs []string` which is initialized by calling a function from the
runtime package.   is probably where we could find some code to convert the
C-strings from `char **environ` to actual Go strings.

For getenv:
- [`os.Getenv()`](https://cs.opensource.google/go/go/+/master:src/os/env.go;l=101;bpv=0;bpt=0): Delegates to `syscall.Getenv`.
- [`syscall.Getenv()`](https://cs.opensource.google/go/go/+/refs/heads/master:src/syscall/env_unix.go;drc=f58c6cccc44752146aabcd50a30865e34817a4b4;bpv=0;bpt=0;l=69): This function ensures `syscall.copyenv()` has been called and iterates

## Rust

TODO: Find the rust code for the std::env package.

The std::env::vars() iterator probably just loops through the real environ array
and returns string slices


