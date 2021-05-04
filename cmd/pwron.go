package main

import "rstpwr/cmd"

const FMT = `{"system":"%s","user":"%s","action":"on"}`

func main() {
	cmd.Main("pwron", FMT)
}
