package main

type Item struct {
	Name     string `json:"name"`
	Value    string `json:"value,omitempty"`
	Action   string `json:"action,omitempty"`
	NextMenu string `json:"next_menu,omitempty"`
}

type Process struct {
	PID  int32
	Name string
	CPU  float64
	MEM  float64
}

type State struct {
	Menu        string    `json:"menu"`
	Cursor      int       `json:"cursor"`
	Items       []Item    `json:"items"`
	Result      string    `json:"result,omitempty"`
	MenuHistory []string  `json:"MenuHistory,omitempty"`
	Processes   []Process `json:"processes,omitempty"`
}

func NewState() *State {
	return &State{
		Menu:        "MAIN",
		Cursor:      0,
		Items:       menus["MAIN"],
		Result:      "",
		MenuHistory: []string{},
	}
}

var state = NewState()

var menus = map[string][]Item{
	"RESULT": {
		{Name: "", Value: "", Action: "", NextMenu: ""},
		{Name: "Back", Action: "back"},
	},
	"MAIN": {
		{Name: "Machine", NextMenu: "MACHINE_STATUS"},
		{Name: "Process", NextMenu: "PROCESS_LIST"},
	},
	"MACHINE_STATUS": {
		{Name: "SHUTDOWN", Value: "Shutdown", Action: "shutdown"},
		{Name: "CPU", Value: "Cpu", Action: "get_status"},
		{Name: "TEMPORARY", Value: "Temp", Action: "get_temp"},
		{Name: "MEMORY", Value: "Memory", Action: "get_memory"},
		{Name: "PING", Value: "Ping", Action: "get_ping"},
		{Name: "UPTIME", Value: "Uptime", Action: "get_uptime"},
	},
}

var menuHooks = map[string]func(*State) *State{
	"PROCESS_LIST": enterProcessList,
}
