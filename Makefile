CFLAGS += -g -O0
all: start_with_dup_vars print_all_tmpdir getenv_tmpdir go_print_all_tmpdir go_getenv_tmpdir rust_print_all_tmpdir rust_getenv_tmpdir
	./$<

go_print_all_tmpdir: print_all_tmpdir.go
	go build -o $@ $<

go_getenv_tmpdir: getenv_tmpdir.go
	go build -o $@ $<

rust_print_all_tmpdir: print_all_tmpdir.rs
	rustc -o $@ $<

rust_getenv_tmpdir: getenv_tmpdir.rs
	rustc -o $@ $<

clean:
	rm -rf *.dSYM
	rm -f start_with_dup_vars print_all_tmpdir getenv_tmpdir
	rm -f go_* rust_*

