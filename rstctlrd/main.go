package main

import (
	"bufio"
	"encoding/json"
	"errors"
	"flag"
	"log"
	"net"
	"os"
	"time"

	"rstctlrd/cmd"
	"rstctlrd/config"

	"github.com/tarm/serial"
)

type systems map[string]chan *request

type request struct {
	cmd string
	r   chan string
}

var (
	configFile string
	uartDev    string
	usockPath  string
)

func init() {
	flag.StringVar(&configFile, "c", "/etc/rstctlrd.json", "Path to config file")
	flag.StringVar(&uartDev, "d", "", "Path to UART device file")
	flag.StringVar(&usockPath, "s", "/var/run/rstctlrd.sock", "Path to Unix domain socket")
}

func driver(c chan *request) {
	cfg := &serial.Config{Name: uartDev, Baud: 9600}
	s, err := serial.OpenPort(cfg)
	if err != nil {
		log.Fatal(err)
	}
	scanner := bufio.NewScanner(s)
	for req := range c {
		_, rerr := s.Write([]byte(req.cmd))
		if rerr != nil {
			log.Fatal(rerr)
		}
		if !scanner.Scan() {
			log.Fatal("scanner EOT")
		}
		line := scanner.Text()
		req.r <- line
	}
}

func send(c chan *request, cmd string) error {
	r := make(chan string)
	req := &request{cmd: cmd, r: r}
	c <- req
	s := <-r
	if s != "+ok" {
		return errors.New("error")
	}
	return nil
}

func pulse(c chan *request, pin int, duration time.Duration) error {
	if err := send(c, cmd.On(pin)); err != nil {
		return err
	}
	time.Sleep(duration)
	return send(c, cmd.Off(pin))
}

func on(driver chan *request, port int) error {
	pin := 2 * port
	return pulse(driver, pin, 1*time.Second)
}

func off(driver chan *request, port int) error {
	pin := 2 * port
	return pulse(driver, pin, 5*time.Second)
}

func reset(driver chan *request, port int) error {
	pin := 2*port + 1
	return pulse(driver, pin, 500*time.Millisecond)
}

func resetAll(c chan *request) error {
	return send(c, cmd.Rst())
}

func startup() (chan *request, systems) {
	c := make(chan *request)
	cfg := config.Setup(configFile)
	go driver(c)
	systems := make(systems)
	for s := range cfg {
		sys := make(chan *request)
		go runner(c, sys, cfg[s])
		systems[s] = sys
	}
	return c, systems
}

func runner(d chan *request, c chan *request, port int) {
	for req := range c {
		var err error
		switch req.cmd {
		case "reset":
			err = reset(d, port)
		case "on":
			err = on(d, port)
		case "off":
			err = off(d, port)
		}
		if err != nil {
			req.r <- "false"
		} else {
			req.r <- "true"
		}
	}
}

type Message struct {
	User   string `json:"user"`
	Action string `json:"action"`
	System string `json:"system"`
}

func serve(conn net.Conn, systems systems) {
	defer conn.Close()
	f := json.NewDecoder(conn)

	var m Message
	for err := f.Decode(&m); err == nil; {
		if c, ok := systems[m.System]; ok {
			r := make(chan string)
			req := &request{cmd: m.Action, r: r}
			c <- req
			res := <-r
			resp := `{"status":` + res + "}"
			conn.Write([]byte(resp))
			log.Println("action", m.Action, "by user", m.User, "result", res)
		} else {
			conn.Write([]byte(`{"status":false, "error":"bad system"}`))
			log.Println("bad system:", m.System)
		}
		err = f.Decode(&m)
	}
}

func main() {
	c, systems := startup()
	flag.Parse()

	resetAll(c)
	os.Remove(usockPath)
	l, err := net.Listen("unix", usockPath)
	if err != nil {
		log.Fatal("listen failed: ", err)
	}
	defer os.Remove(usockPath)

	for {
		conn, err := l.Accept()
		if err != nil {
			log.Fatal("", err)
		}
		go serve(conn, systems)
	}
}
