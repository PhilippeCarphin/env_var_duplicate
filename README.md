# Duplicate environemnt variables

Launch various programs with `execle(prog, arg0, NULL, envp)` with an array
`envp` containing two strings beginning with `"TMPDIR="` creating a situation
with an ambiguous value for `TMPDIR` in the child processes.

Run
```
make
```
and look at the output.

