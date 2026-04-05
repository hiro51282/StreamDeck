package main

import (
	"encoding/json"
	"fmt"
	"net/http"
)

func check_token(r *http.Request) bool {
	return r.URL.Query().Get("token") == config.Token
}

func buildMenu(menu string, s *State) []Item {
	if menu == "PROCESS_LIST" {
		var items []Item

		for _, p := range s.Processes {
			items = append(items, Item{
				Name:   fmt.Sprintf("%s (%.0f/%.0f)", p.Name, p.CPU, p.MEM),
				Action: "kill_process", // 後で使う
				Value:  fmt.Sprintf("%d", p.PID),
			})
		}

		// Back追加
		items = append(items, Item{
			Name:   "Back",
			Action: "back",
		})

		return items
	}

	return menus[menu]
}
func handleSelect(s *State) *State {
	if len(s.Items) == 0 {
		return s
	}

	item := s.Items[s.Cursor]

	// ① Action（ドメイン処理）
	if item.Action != "" {
		handler, ok := actions[item.Action]
		if ok {
			return handler(s)
		}
	}

	// ② NextMenu（遷移）
	if item.NextMenu != "" {
		s.MenuHistory = append(s.MenuHistory, s.Menu)

		next := item.NextMenu

		// フック実行
		if hook, ok := menuHooks[next]; ok {
			s = hook(s)
		}

		return buildState(next, s)
	}
	return s
}

// -----------------------------
// GET /menu
// -----------------------------
func menuHandler(w http.ResponseWriter, r *http.Request) {
	if !check_token(r) {
		http.Error(w, "NG", http.StatusForbidden)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(state)
}

// -----------------------------
// POST /action
// -----------------------------
func actionHandler(w http.ResponseWriter, r *http.Request) {
	if !check_token(r) {
		http.Error(w, "NG", http.StatusForbidden)
		return
	}

	if r.Method != http.MethodPost {
		http.Error(w, "Method Not Allowed", http.StatusMethodNotAllowed)
		return
	}

	var req struct {
		Type string `json:"type"`
	}

	if err := json.NewDecoder(r.Body).Decode(&req); err != nil {
		http.Error(w, "Bad Request", http.StatusBadRequest)
		return
	}

	// ① UI操作
	if handler, ok := uiActions[req.Type]; ok {
		state = handler(state)
		fmt.Fprint(w, "OK")
		return
	}

	// ② ドメイン処理（今は未使用でもOK）
	if handler, ok := actions[req.Type]; ok {
		state = handler(state)
		fmt.Fprint(w, "OK")
		return
	}

	http.Error(w, "Unknown Action", http.StatusBadRequest)
}

// -----------------------------
// Service系（既存）
// -----------------------------
func makeHandler(f ServiceFunc) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {

		if !check_token(r) {
			http.Error(w, "NG", http.StatusForbidden)
			return
		}

		val, err := f()
		if err != nil {
			http.Error(w, "FAIL", 500)
			return
		}

		fmt.Fprint(w, val)
	}
}

func registerHandlers() {
	// 固定API
	http.HandleFunc("/menu", menuHandler)
	http.HandleFunc("/action", actionHandler)

	// Service API
	for path, handler := range routes {
		http.HandleFunc(path, makeHandler(handler))
	}
}
