// wardrobe.go
package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"os"
	"strings"
	"time"
	"github.com/google/uuid"
)

type ClothingItem struct {
	ID        string `json:"id"`
	Name      string `json:"name"`
	Category  string `json:"category"`
	Photo     string `json:"photo"`
	Tags      string `json:"tags"`
	Date      string `json:"date"`
	Favorite  bool   `json:"favorite"`
	CreatedAt string `json:"created_at"`
}

type Wardrobe struct {
	Items []ClothingItem `json:"items"`
}

var dataFile = "wardrobe.json"

func (w *Wardrobe) load() {
	data, err := os.ReadFile(dataFile)
	if err != nil {
		return
	}
	json.Unmarshal(data, w)
}

func (w *Wardrobe) save() {
	data, _ := json.MarshalIndent(w, "", "  ")
	os.WriteFile(dataFile, data, 0644)
}

func (w *Wardrobe) add(name, category, photo, tags, date string) {
	if date == "" {
		date = time.Now().Format("2006-01-02")
	}
	item := ClothingItem{
		ID:        uuid.New().String()[:8],
		Name:      name,
		Category:  category,
		Photo:     photo,
		Tags:      tags,
		Date:      date,
		Favorite:  false,
		CreatedAt: time.Now().Format(time.RFC3339),
	}
	w.Items = append(w.Items, item)
	w.save()
	fmt.Printf("✅ Added: %s (ID: %s)\n", name, item.ID)
}

func (w *Wardrobe) list(category string) {
	items := w.Items
	if category != "" {
		var filtered []ClothingItem
		for _, i := range items {
			if strings.EqualFold(i.Category, category) {
				filtered = append(filtered, i)
			}
		}
		items = filtered
	}
	if len(items) == 0 {
		fmt.Println("No items.")
		return
	}
	fmt.Printf("\n📋 Wardrobe (%d items):\n", len(items))
	for idx, item := range items {
		fav := ""
		if item.Favorite {
			fav = " ⭐"
		}
		tags := ""
		if item.Tags != "" {
			tags = " | " + item.Tags
		}
		fmt.Printf("  %d. %s [%s]%s%s\n", idx+1, item.Name, item.Category, fav, tags)
	}
}

func (w *Wardrobe) search(term string) {
	termLower := strings.ToLower(term)
	var results []ClothingItem
	for _, item := range w.Items {
		if strings.Contains(strings.ToLower(item.Name), termLower) ||
			strings.Contains(strings.ToLower(item.Category), termLower) ||
			strings.Contains(strings.ToLower(item.Tags), termLower) {
			results = append(results, item)
		}
	}
	if len(results) == 0 {
		fmt.Println("No matches.")
		return
	}
	fmt.Printf("\n🔍 Found %d item(s):\n", len(results))
	for idx, item := range results {
		fav := ""
		if item.Favorite {
			fav = " ⭐"
		}
		tags := ""
		if item.Tags != "" {
			tags = " | " + item.Tags
		}
		fmt.Printf("  %d. %s [%s]%s%s\n", idx+1, item.Name, item.Category, fav, tags)
	}
}

func (w *Wardrobe) show(id string) {
	for _, item := range w.Items {
		if item.ID == id {
			fmt.Printf("\n👔 %s\n", item.Name)
			fmt.Printf("  ID: %s\n", item.ID)
			fmt.Printf("  Category: %s\n", item.Category)
			fmt.Printf("  Photo: %s\n", item.Photo)
			fmt.Printf("  Tags: %s\n", item.Tags)
			fmt.Printf("  Date: %s\n", item.Date)
			fmt.Printf("  Favorite: %v\n", item.Favorite)
			fmt.Printf("  Added: %s\n", item.CreatedAt)
			return
		}
	}
	fmt.Printf("Item %s not found.\n", id)
}

func (w *Wardrobe) favorite(id string) {
	for i := range w.Items {
		if w.Items[i].ID == id {
			w.Items[i].Favorite = !w.Items[i].Favorite
			w.save()
			status := "⭐ added to"
			if !w.Items[i].Favorite {
				status = "❌ removed from"
			}
			fmt.Printf("✅ %s favorites: %s\n", status, w.Items[i].Name)
			return
		}
	}
	fmt.Printf("Item %s not found.\n", id)
}

func (w *Wardrobe) stats() {
	if len(w.Items) == 0 {
		fmt.Println("No items.")
		return
	}
	categories := make(map[string]int)
	for _, item := range w.Items {
		cat := item.Category
		if cat == "" {
			cat = "Uncategorized"
		}
		categories[cat]++
	}
	fmt.Println("\n📊 Statistics:")
	for cat, count := range categories {
		fmt.Printf("  %s: %d item(s)\n", cat, count)
	}
}

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Usage: wardrobe <command> [options]")
		return
	}
	w := &Wardrobe{}
	w.load()
	cmd := os.Args[1]

	switch cmd {
	case "add":
		addCmd := flag.NewFlagSet("add", flag.ExitOnError)
		name := addCmd.String("name", "", "")
		category := addCmd.String("category", "", "")
		photo := addCmd.String("photo", "", "")
		tags := addCmd.String("tags", "", "")
		date := addCmd.String("date", "", "")
		addCmd.Parse(os.Args[2:])
		if *name == "" && len(addCmd.Args()) > 0 {
			*name = addCmd.Args()[0]
		}
		if *name == "" {
			fmt.Println("add requires a name")
			return
		}
		w.add(*name, *category, *photo, *tags, *date)

	case "list":
		listCmd := flag.NewFlagSet("list", flag.ExitOnError)
		category := listCmd.String("category", "", "")
		listCmd.Parse(os.Args[2:])
		w.list(*category)

	case "search":
		if len(os.Args) < 3 {
			fmt.Println("search <term>")
			return
		}
		w.search(os.Args[2])

	case "show":
		if len(os.Args) < 3 {
			fmt.Println("show <id>")
			return
		}
		w.show(os.Args[2])

	case "favorite":
		if len(os.Args) < 3 {
			fmt.Println("favorite <id>")
			return
		}
		w.favorite(os.Args[2])

	case "stats":
		w.stats()

	default:
		fmt.Println("Unknown command. Use add, list, search, show, favorite, stats.")
	}
}
