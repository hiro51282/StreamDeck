package main

import (
	"fmt"
	"os"
	"os/exec"
	"time"

	"github.com/shirou/gopsutil/v3/cpu"
	"github.com/shirou/gopsutil/v3/host"
	"github.com/shirou/gopsutil/v3/mem"
)

func shutdown() error {
	if os.Getenv("IN_DOCKER") == "true" {
		return nil
	}

	if os.Getenv("ALLOW_SHUTDOWN") != "true" {
		return fmt.Errorf("ALLOW_SHUTDOWN DISABLED IF SHUTDOWN OPENED IN PRODUCTION, SET ALLOW_SHUTDOWN=true IN ENV")
	}

	cmd := exec.Command("sudo", "/usr/local/bin/streamdeck_shutdown.sh")
	return cmd.Run()
}

func getStatus() (float64, error) {
	percent, err := cpu.Percent(0, false)
	if err != nil {
		return 0, err
	}
	return percent[0], nil
}

func getTemperature() (float64, error) {
	temps, err := host.SensorsTemperatures()
	if err != nil {
		return 0, err
	}
	if len(temps) == 0 {
		return 0, fmt.Errorf("no temperature sensors")
	}
	return temps[0].Temperature, nil
}

func getMemory() (float64, error) {
	info, err := mem.VirtualMemory()
	if err != nil {
		return 0, err
	}
	return info.UsedPercent, nil
}

func getPing() string {
	return "ONLINE"
}

func getUptime() (int64, error) {
	boot, err := host.BootTime()
	if err != nil {
		return 0, err
	}
	return time.Now().Unix() - int64(boot), nil
}
