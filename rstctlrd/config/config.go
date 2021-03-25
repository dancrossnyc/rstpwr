package config

import (
	"encoding/json"
	"io/ioutil"
	"log"
)

type Config map[string]int

type ConfigRec struct {
	System string `json:"system"`
	Port   int    `json:"port"`
}

func Setup(file string) Config {
	data, err := ioutil.ReadFile(file)
	if err != nil {
		log.Fatal("cannot read config file: ", err)
	}
	records := make([]ConfigRec, 0)
	err = json.Unmarshal(data, &records)
	if err != nil {
		log.Fatal("malformed config: ", err)
	}
	config := make(Config)
	for k := range records {
		rec := records[k]
		config[rec.System] = rec.Port
	}
	return config
}
