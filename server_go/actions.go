package main

import "fmt"

func buildState(menu string, s *State) *State {
	items := make([]Item, len(menus[menu]))
	copy(items, menus[menu])

	if menu == "RESULT" && len(items) > 0 {
		items[0].Name = s.Result
	} else {
		s.Result = ""
	}

	return &State{
		Menu:        menu,
		Cursor:      0,
		Items:       items,
		Result:      s.Result,
		MenuHistory: s.MenuHistory,
	}
}

type ActionFunc func(*State) *State

var actions = map[string]ActionFunc{
	"shutdown": func(s *State) *State {
		err := shutdown()
		if err != nil {
			s.Result = "FAIL"
		} else {
			s.Result = "OK"
		}

		s.MenuHistory = append(s.MenuHistory, s.Menu)
		return buildState("RESULT", s)
	},
	"get_status": func(s *State) *State {
		val, err := getStatus()
		if err != nil {
			s.Result = err.Error()
		} else {
			s.Result = fmt.Sprintf("CPU %.2f%%", val)
		}

		s.MenuHistory = append(s.MenuHistory, s.Menu)
		return buildState("RESULT", s)
	},
	"get_temp": func(s *State) *State {
		val, err := getTemperature()
		if err != nil {
			s.Result = err.Error()
		} else {
			s.Result = fmt.Sprintf("TEMP %.2f℃", val)
		}

		s.MenuHistory = append(s.MenuHistory, s.Menu)
		return buildState("RESULT", s)
	},
	"get_memory": func(s *State) *State {
		val, err := getMemory()
		if err != nil {
			s.Result = err.Error()
		} else {
			s.Result = fmt.Sprintf("MEM %.2f%%", val)
		}

		s.MenuHistory = append(s.MenuHistory, s.Menu)
		return buildState("RESULT", s)
	},
	"get_ping": func(s *State) *State {
		val := getPing()
		s.Result = fmt.Sprintf("PING %s", val)
		s.MenuHistory = append(s.MenuHistory, s.Menu)
		return buildState("RESULT", s)
	},
	"get_uptime": func(s *State) *State {
		val, err := getUptime()
		if err != nil {
			s.Result = err.Error()
		} else {
			s.Result = fmt.Sprintf("%d", val)
		}
		s.MenuHistory = append(s.MenuHistory, s.Menu)
		return buildState("RESULT", s)
	},
}

var uiActions = map[string]func(*State) *State{
	"up": func(s *State) *State {
		s.Cursor--
		if s.Cursor < 0 {
			s.Cursor = len(s.Items) - 1
		}
		return s
	},
	"down": func(s *State) *State {
		s.Cursor++
		if s.Cursor >= len(s.Items) {
			s.Cursor = 0
		}
		return s
	},
	"select": handleSelect,
	"back": func(s *State) *State {
		if len(s.MenuHistory) == 0 {
			return s
		}

		last := s.MenuHistory[len(s.MenuHistory)-1]
		s.MenuHistory = s.MenuHistory[:len(s.MenuHistory)-1]

		return buildState(last, s)
	},
}

// NOTE:
// ServiceFunc、routes はデバッグ/直接API用
// 将来的に削除可能（actions経由に統一予定）
type ServiceFunc func() (any, error)

var routes = map[string]ServiceFunc{
	"/shutdown": func() (any, error) {
		err := shutdown()
		if err != nil {
			return nil, err
		}
		return "OK", nil
	},
	"/status": func() (any, error) {
		val, err := getStatus()
		if err != nil {
			return nil, err
		}
		return fmt.Sprintf("CPU %.2f%%", val), nil
	},
	"/temp": func() (any, error) {
		val, err := getTemperature()
		if err != nil {
			return nil, err
		}
		return fmt.Sprintf("%.2f", val), nil
	},
	"/memory": func() (any, error) {
		val, err := getMemory()
		if err != nil {
			return nil, err
		}
		return fmt.Sprintf("MEM %.2f%%", val), nil
	},
	"/ping": func() (any, error) {
		val := getPing()
		return fmt.Sprintf("PING %s", val), nil
	},
	"/uptime": func() (any, error) {
		val, err := getUptime()
		if err != nil {
			return nil, err
		}
		return fmt.Sprintf("%d", val), nil
	},
}
