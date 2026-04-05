package main

import (
	"encoding/json"
	"fmt"
	"net/http"
	"os"
)

type Config struct {
	Token string `json:"token"`
}

var config Config

func init() {
	file, err := os.ReadFile("config.json")
	if err != nil {
		panic(err)
	}

	if err := json.Unmarshal(file, &config); err != nil {
		panic(err)
	}
}

func main() {
	registerHandlers()

	fmt.Println("Server running on :5000")
	http.ListenAndServe(":5000", nil)
}
