package main

import (
	"fmt"
	"go.cs.palashbauri.in/pankti/gopkg"
)

import "C"

func main() {
	fmt.Println(gopkg.DoParse("show 1+2;"))
}
