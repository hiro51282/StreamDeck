package main

import (
	"encoding/json"
	"fmt"
	"net/http"
	"os"
	"time"
	"os/exec"
	"github.com/shirou/gopsutil/v3/mem"
	"github.com/shirou/gopsutil/v3/cpu"
	"github.com/shirou/gopsutil/v3/host"
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

func check_token(r *http.Request) bool{
	return r.URL.Query().Get("token") == config.Token
}

func main() {
	http.HandleFunc("/shutdown", func(w http.ResponseWriter, r *http.Request) {
		if ! check_token(r) {
			http.Error(w, "NG", http.StatusForbidden)
			return
		}

		if os.Getenv("IN_DOCKER") == "true" {
			fmt.Fprint(w, "SKIPPED")
			return
		}
		
		cmd := exec.Command("sudo", "/usr/local/bin/streamdeck_shutdown.sh")
		err := cmd.Run()

		if err != nil {
			http.Error(w, "FAIL", http.StatusInternalServerError)
			return
		}

		fmt.Fprint(w, "OK")
	})

	http.HandleFunc("/status", func(w http.ResponseWriter, r *http.Request) {
		if ! check_token(r) {
			http.Error(w, "NG", http.StatusForbidden)
			return
		}

		percent, _ := cpu.Percent(0, false)
		fmt.Fprintf(w, "CPU %.2f%%", percent[0])
	})

	http.HandleFunc("/temp", func(w http.ResponseWriter, r *http.Request) {
		if ! check_token(r) {
			http.Error(w, "NG", http.StatusForbidden)
			return
		}
		
		temps, _ := host.SensorsTemperatures()
		if len(temps) > 0 {
			fmt.Fprintf(w, "%.2f", temps[0].Temperature)
		} else {
			fmt.Fprint(w, "N/A")
		}
	})

	http.HandleFunc("/memory", func(w http.ResponseWriter, r *http.Request) {
		if ! check_token(r) {
			http.Error(w, "NG", http.StatusForbidden)
			return
		}
		info, _ := mem.VirtualMemory()
		fmt.Fprintf(w, "MEM %.2f%%", info.UsedPercent)
	})

	http.HandleFunc("/ping", func(w http.ResponseWriter, r *http.Request) {
		if ! check_token(r) {
			http.Error(w, "NG", http.StatusForbidden)
			return
		}

		fmt.Fprint(w, "ONLINE")
	})

	http.HandleFunc("/uptime", func(w http.ResponseWriter, r *http.Request) {
		if ! check_token(r) {
			http.Error(w, "NG", http.StatusForbidden)
			return
		}
		boot, _ := host.BootTime()
		uptime := time.Now().Unix() - int64(boot)
		fmt.Fprintf(w, "%d", uptime)
	})

	fmt.Println("Server running on :5001")
	http.ListenAndServe(":5001", nil)
}