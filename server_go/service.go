package main

import (
	"fmt"
	"os"
	"os/exec"
	"sort"
	"time"

	"github.com/shirou/gopsutil/v3/cpu"
	"github.com/shirou/gopsutil/v3/host"
	"github.com/shirou/gopsutil/v3/mem"
	"github.com/shirou/gopsutil/v3/process"
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

func getTopProcesses(limit int) ([]Process, error) {
	procs, err := process.Processes()
	if err != nil {
		return nil, err
	}

	var result []Process

	for _, p := range procs {
		name, err := p.Name()
		if err != nil {
			continue
		}

		// CPUは2回呼び（重要）
		p.CPUPercent()
		time.Sleep(50 * time.Millisecond)
		cpu, _ := p.CPUPercent()

		mem, _ := p.MemoryPercent()

		// ノイズ除去（任意）
		if cpu == 0 && mem < 1 {
			continue
		}

		result = append(result, Process{
			PID:  p.Pid,
			Name: name,
			CPU:  cpu,
			MEM:  float64(mem),
		})
	}

	// CPU優先
	sort.Slice(result, func(i, j int) bool {
		return result[i].CPU > result[j].CPU
	})
	// // MEM優先 → CPU補助
	// sort.Slice(result, func(i, j int) bool {
	// 	if result[i].MEM == result[j].MEM {
	// 		return result[i].CPU > result[j].CPU
	// 	}
	// 	return result[i].MEM > result[j].MEM
	// })

	// 上位N件
	if len(result) > limit {
		result = result[:limit]
	}

	return result, nil
}

func killProcess(pid int) error {
	p, err := process.NewProcess(int32(pid))
	if err != nil {
		return err
	}
	return p.Kill()
}
