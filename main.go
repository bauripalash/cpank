package main

import (
	"cpank/gopkg"
	"fmt"
)

import "C"

func main(){
	fmt.Println(gopkg.DoParse("show 1+2;"))
}
