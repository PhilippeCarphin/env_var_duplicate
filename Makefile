CFLAGS += -g -O0
all: start_with_dup_vars print_all_tmpdir getenv_tmpdir
	./$<

clean:
	rm -rf *.dSYM
	rm -f start_with_dup_vars print_all_tmpdir getenv_tmpdir

