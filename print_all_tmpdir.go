package main

import (
	"fmt"
	"os"
	"strings"
)

func main() {
	for _, e := range os.Environ() {
		pair := strings.SplitN(e, "=", 2)
		if pair[0] == "TMPDIR" {
			fmt.Printf("TMPDIR = %s\n", pair[1])
		}
	}
}
