// Wardrobe.java
import java.io.*;
import java.nio.file.*;
import java.time.*;
import java.util.*;
import java.util.stream.*;
import com.google.gson.*;

class ClothingItem {
    String id;
    String name;
    String category;
    String photo;
    String tags;
    String date;
    boolean favorite;
    String created_at;

    ClothingItem() {}
    ClothingItem(String name, String category, String photo, String tags, String date) {
        this.id = UUID.randomUUID().toString().substring(0,8);
        this.name = name;
        this.category = category;
        this.photo = photo;
        this.tags = tags;
        this.date = date.isEmpty() ? LocalDate.now().toString() : date;
        this.favorite = false;
        this.created_at = Instant.now().toString();
    }
}

class Wardrobe {
    private List<ClothingItem> items = new ArrayList<>();
    private final String dataFile = "wardrobe.json";
    private final Gson gson = new GsonBuilder().setPrettyPrinting().create();

    public Wardrobe() { load(); }

    private void load() {
        try {
            Path path = Paths.get(dataFile);
            if (Files.exists(path)) {
                String json = new String(Files.readAllBytes(path));
                ClothingItem[] arr = gson.fromJson(json, ClothingItem[].class);
                items = Arrays.asList(arr);
            }
        } catch (Exception e) {}
    }

    private void save() {
        try {
            Files.write(Paths.get(dataFile), gson.toJson(items).getBytes());
        } catch (Exception e) {}
    }

    public void add(String name, String category, String photo, String tags, String date) {
        ClothingItem item = new ClothingItem(name, category, photo, tags, date);
        items.add(item);
        save();
        System.out.printf("✅ Added: %s (ID: %s)%n", name, item.id);
    }

    public void list(String category) {
        List<ClothingItem> filtered = items;
        if (category != null && !category.isEmpty()) {
            filtered = items.stream()
                .filter(i -> i.category.equalsIgnoreCase(category))
                .collect(Collectors.toList());
        }
        if (filtered.isEmpty()) {
            System.out.println("No items.");
            return;
        }
        System.out.printf("\n📋 Wardrobe (%d items):\n", filtered.size());
        for (int i = 0; i < filtered.size(); i++) {
            ClothingItem item = filtered.get(i);
            String fav = item.favorite ? " ⭐" : "";
            String tags = item.tags != null && !item.tags.isEmpty() ? " | " + item.tags : "";
            System.out.printf("  %d. %s [%s]%s%s%n", i+1, item.name, item.category, fav, tags);
        }
    }

    public void search(String term) {
        String lower = term.toLowerCase();
        List<ClothingItem> results = items.stream()
            .filter(i -> i.name.toLowerCase().contains(lower) ||
                         i.category.toLowerCase().contains(lower) ||
                         (i.tags != null && i.tags.toLowerCase().contains(lower)))
            .collect(Collectors.toList());
        if (results.isEmpty()) {
            System.out.println("No matches.");
            return;
        }
        System.out.printf("\n🔍 Found %d item(s):\n", results.size());
        for (int i = 0; i < results.size(); i++) {
            ClothingItem item = results.get(i);
            String fav = item.favorite ? " ⭐" : "";
            String tags = item.tags != null && !item.tags.isEmpty() ? " | " + item.tags : "";
            System.out.printf("  %d. %s [%s]%s%s%n", i+1, item.name, item.category, fav, tags);
        }
    }

    public void show(String id) {
        for (ClothingItem item : items) {
            if (item.id.equals(id)) {
                System.out.printf("\n👔 %s%n", item.name);
                System.out.printf("  ID: %s%n", item.id);
                System.out.printf("  Category: %s%n", item.category.isEmpty() ? "Uncategorized" : item.category);
                System.out.printf("  Photo: %s%n", item.photo.isEmpty() ? "No photo" : item.photo);
                System.out.printf("  Tags: %s%n", item.tags.isEmpty() ? "None" : item.tags);
                System.out.printf("  Date: %s%n", item.date);
                System.out.printf("  Favorite: %s%n", item.favorite ? "Yes" : "No");
                System.out.printf("  Added: %s%n", item.created_at);
                return;
            }
        }
        System.out.printf("Item %s not found.%n", id);
    }

    public void favorite(String id) {
        for (ClothingItem item : items) {
            if (item.id.equals(id)) {
                item.favorite = !item.favorite;
                save();
                String status = item.favorite ? "⭐ added to" : "❌ removed from";
                System.out.printf("✅ %s favorites: %s%n", status, item.name);
                return;
            }
        }
        System.out.printf("Item %s not found.%n", id);
    }

    public void stats() {
        if (items.isEmpty()) {
            System.out.println("No items.");
            return;
        }
        Map<String, Integer> categories = new HashMap<>();
        for (ClothingItem item : items) {
            String cat = item.category.isEmpty() ? "Uncategorized" : item.category;
            categories.put(cat, categories.getOrDefault(cat, 0) + 1);
        }
        System.out.println("\n📊 Statistics:");
        categories.entrySet().stream()
            .sorted(Map.Entry.comparingByKey())
            .forEach(e -> System.out.printf("  %s: %d item(s)%n", e.getKey(), e.getValue()));
    }

    public static void main(String[] args) {
        if (args.length < 1) {
            System.out.println("Usage: Wardrobe <command> [options]");
            return;
        }
        Wardrobe w = new Wardrobe();
        String cmd = args[0];
        Map<String, String> params = new HashMap<>();
        for (int i=1; i<args.length; i++) {
            if (args[i].startsWith("--") && i+1 < args.length) {
                params.put(args[i].substring(2), args[++i]);
            }
        }
        switch (cmd) {
            case "add":
                if (args.length < 2) { System.out.println("add <name> [--category CAT] [--photo PATH] [--tags TAGS] [--date DATE]"); return; }
                w.add(args[1], params.getOrDefault("category", ""),
                      params.getOrDefault("photo", ""),
                      params.getOrDefault("tags", ""),
                      params.getOrDefault("date", ""));
                break;
            case "list":
                w.list(params.get("category"));
                break;
            case "search":
                if (args.length < 2) { System.out.println("search <term>"); return; }
                w.search(args[1]);
                break;
            case "show":
                if (args.length < 2) { System.out.println("show <id>"); return; }
                w.show(args[1]);
                break;
            case "favorite":
                if (args.length < 2) { System.out.println("favorite <id>"); return; }
                w.favorite(args[1]);
                break;
            case "stats":
                w.stats();
                break;
            default:
                System.out.println("Unknown command. Use add, list, search, show, favorite, stats.");
        }
    }
}
