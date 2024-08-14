package main

import (
	"os"
	"os/exec"
)

func main(){

	/*
	 * The origin of this test is from the gitlab runner that started
	 * showed some weird behavior when I was trying out the custom
	 * executor.
	 *
	 * I had a BASH version and a Python version.  They were both
	 * supposed to work exactly the same as they were just me showing
	 * the same thing in two languages.
	 *
	 * However they were not working the same.  In one of them, the
	 * value of TMPDIR was one thing and in the other it was something
	 * else.
	 *
	 * This is because the go code was doing something like this where
	 * we were combining the process environment with some other stuff
	 * from the executor which included a TMPDIR=...
	 *
	 * This caused the array to have two strings 'TMPDIR=...' and the
	 * difference in behavior is because $TMPDIR in BASH gave the second
	 * value and os.environ['TMPDIR'] in Python gave the second value.
	 *
	 * Note that the exec package now has deduplication built in
	 * (https://github.com/golang/go/blob/master/src/os/exec/exec.go#L1225)
	 */

	cmd := exec.Command("./print_all_tmpdir")
	cmd.Env = append(os.Environ(), "TMPDIR=asdfasdf" /* Executor settings */)
	cmd.Stderr = os.Stderr
	cmd.Stdout = os.Stdout

	cmd.Run()
}
