package main

import (
	"fmt"
	"os"
)

func main() {
	fmt.Printf("os.Getenv(\"TMPDIR\") -> %s\n", os.Getenv("TMPDIR"))
}
