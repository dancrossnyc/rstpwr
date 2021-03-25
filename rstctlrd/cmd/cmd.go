package cmd

import (
        "fmt"
)

func cksum(cmd string) byte {
        sum := byte(0)
        for _, b := range cmd[:4] {
                sum += byte(b)
        }
        return -sum
}

func fmtcmd(cmd string) string {
        sum := fmt.Sprintf("%02X", cksum(cmd))
        return cmd + sum + "\n"
}

func On(port int) string {
        return fmtcmd(fmt.Sprintf("I%02X#", port))
}

func Off(port int) string {
        return fmtcmd(fmt.Sprintf("O%02X#", port))
}

func Rst() string {
        return fmtcmd("RST#")
}
