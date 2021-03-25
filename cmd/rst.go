package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"net"
	"os"
	"os/user"
)

var usockPath string

func init() {
	flag.StringVar(&usockPath, "s", "/var/run/rstctlrd.sock", "Path to Unix domain socket")
}

const FMT = `{"system":"%s","user":"%s","action":"reset"}`

type Message struct {
	Status bool   `json:"status"`
	Error  string `json:"error"`
}

func main() {
	flag.Parse()

	if len(os.Args) != 2 {
		log.Fatal("usage: hyreset system")
	}
	system := os.Args[1]
	user, uerr := user.Current()
	if uerr != nil {
		log.Fatal("Who are you?")
	}
	login := user.Username
	c, cerr := net.Dial("unix", usockPath)
	if cerr != nil {
		log.Fatal("cannot dial controller: ", cerr)
	}
	defer c.Close()
	msg := fmt.Sprintf(FMT, system, login)
	_, werr := c.Write([]byte(msg))
	if werr != nil {
		log.Fatal("cannot write message: ", werr)
	}
	var m Message
	derr := json.NewDecoder(c).Decode(&m)
	if derr != nil {
		log.Fatal("cannot read response: ", derr)
	}
	if m.Status != true {
		log.Fatal("controller error: ", m.Error)
	}
}
