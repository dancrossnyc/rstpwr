package main

import "rstpwr/cmd"

const FMT = `{"system":"%s","user":"%s","action":"off"}`

func main() {
	cmd.Main("pwroff", FMT)
}
