package main

import "rstpwr/cmd"

const FMT = `{"system":"%s","user":"%s","action":"reset"}`

func main() {
	cmd.Main("rst", FMT)
}
