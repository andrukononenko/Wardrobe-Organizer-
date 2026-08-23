// Wardrobe.cs
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.Json;
using System.Text.Json.Serialization;

class ClothingItem
{
    [JsonPropertyName("id")]
    public string Id { get; set; }
    [JsonPropertyName("name")]
    public string Name { get; set; }
    [JsonPropertyName("category")]
    public string Category { get; set; }
    [JsonPropertyName("photo")]
    public string Photo { get; set; }
    [JsonPropertyName("tags")]
    public string Tags { get; set; }
    [JsonPropertyName("date")]
    public string Date { get; set; }
    [JsonPropertyName("favorite")]
    public bool Favorite { get; set; }
    [JsonPropertyName("created_at")]
    public string CreatedAt { get; set; }

    public ClothingItem() { }
    public ClothingItem(string name, string category, string photo, string tags, string date)
    {
        Id = Guid.NewGuid().ToString().Substring(0,8);
        Name = name;
        Category = category;
        Photo = photo;
        Tags = tags;
        Date = string.IsNullOrEmpty(date) ? DateTime.Now.ToString("yyyy-MM-dd") : date;
        Favorite = false;
        CreatedAt = DateTime.Now.ToString("o");
    }
}

class Wardrobe
{
    private List<ClothingItem> items = new List<ClothingItem>();
    private readonly string dataFile = "wardrobe.json";
    private readonly JsonSerializerOptions options = new JsonSerializerOptions { WriteIndented = true };

    public Wardrobe() => Load();

    private void Load()
    {
        if (!File.Exists(dataFile)) return;
        string json = File.ReadAllText(dataFile);
        items = JsonSerializer.Deserialize<List<ClothingItem>>(json) ?? new List<ClothingItem>();
    }

    private void Save()
    {
        string json = JsonSerializer.Serialize(items, options);
        File.WriteAllText(dataFile, json);
    }

    public void Add(string name, string category, string photo, string tags, string date)
    {
        var item = new ClothingItem(name, category, photo, tags, date);
        items.Add(item);
        Save();
        Console.WriteLine($"✅ Added: {name} (ID: {item.Id})");
    }

    public void List(string category)
    {
        var filtered = items;
        if (!string.IsNullOrEmpty(category))
        {
            filtered = items.Where(i => string.Equals(i.Category, category, StringComparison.OrdinalIgnoreCase)).ToList();
        }
        if (!filtered.Any())
        {
            Console.WriteLine("No items.");
            return;
        }
        Console.WriteLine($"\n📋 Wardrobe ({filtered.Count} items):");
        for (int i = 0; i < filtered.Count; i++)
        {
            var item = filtered[i];
            string fav = item.Favorite ? " ⭐" : "";
            string tags = !string.IsNullOrEmpty(item.Tags) ? $" | {item.Tags}" : "";
            Console.WriteLine($"  {i+1}. {item.Name} [{item.Category}]{fav}{tags}");
        }
    }

    public void Search(string term)
    {
        var lower = term.ToLower();
        var results = items.Where(i =>
            i.Name.ToLower().Contains(lower) ||
            i.Category.ToLower().Contains(lower) ||
            (i.Tags != null && i.Tags.ToLower().Contains(lower))
        ).ToList();
        if (!results.Any())
        {
            Console.WriteLine("No matches.");
            return;
        }
        Console.WriteLine($"\n🔍 Found {results.Count} item(s):");
        for (int i = 0; i < results.Count; i++)
        {
            var item = results[i];
            string fav = item.Favorite ? " ⭐" : "";
            string tags = !string.IsNullOrEmpty(item.Tags) ? $" | {item.Tags}" : "";
            Console.WriteLine($"  {i+1}. {item.Name} [{item.Category}]{fav}{tags}");
        }
    }

    public void Show(string id)
    {
        var item = items.FirstOrDefault(i => i.Id == id);
        if (item == null)
        {
            Console.WriteLine($"Item {id} not found.");
            return;
        }
        Console.WriteLine($"\n👔 {item.Name}");
        Console.WriteLine($"  ID: {item.Id}");
        Console.WriteLine($"  Category: {string.IsNullOrEmpty(item.Category) ? "Uncategorized" : item.Category}");
        Console.WriteLine($"  Photo: {string.IsNullOrEmpty(item.Photo) ? "No photo" : item.Photo}");
        Console.WriteLine($"  Tags: {string.IsNullOrEmpty(item.Tags) ? "None" : item.Tags}");
        Console.WriteLine($"  Date: {item.Date}");
        Console.WriteLine($"  Favorite: {item.Favorite ? "Yes" : "No"}");
        Console.WriteLine($"  Added: {item.CreatedAt}");
    }

    public void Favorite(string id)
    {
        var item = items.FirstOrDefault(i => i.Id == id);
        if (item == null)
        {
            Console.WriteLine($"Item {id} not found.");
            return;
        }
        item.Favorite = !item.Favorite;
        Save();
        string status = item.Favorite ? "⭐ added to" : "❌ removed from";
        Console.WriteLine($"✅ {status} favorites: {item.Name}");
    }

    public void Stats()
    {
        if (!items.Any())
        {
            Console.WriteLine("No items.");
            return;
        }
        var categories = items.GroupBy(i => string.IsNullOrEmpty(i.Category) ? "Uncategorized" : i.Category)
                              .OrderBy(g => g.Key)
                              .ToDictionary(g => g.Key, g => g.Count());
        Console.WriteLine("\n📊 Statistics:");
        foreach (var kv in categories)
        {
            Console.WriteLine($"  {kv.Key}: {kv.Value} item(s)");
        }
    }

    static void Main(string[] args)
    {
        if (args.Length < 1)
        {
            Console.WriteLine("Usage: Wardrobe <command> [options]");
            return;
        }
        var w = new Wardrobe();
        var parsed = ParseArgs(args);
        string cmd = args[0];
        switch (cmd)
        {
            case "add":
                if (args.Length < 2) { Console.WriteLine("add <name> [--category CAT] [--photo PATH] [--tags TAGS] [--date DATE]"); return; }
                w.Add(args[1], parsed.GetValueOrDefault("category", ""),
                      parsed.GetValueOrDefault("photo", ""),
                      parsed.GetValueOrDefault("tags", ""),
                      parsed.GetValueOrDefault("date", ""));
                break;
            case "list":
                w.List(parsed.GetValueOrDefault("category", ""));
                break;
            case "search":
                if (args.Length < 2) { Console.WriteLine("search <term>"); return; }
                w.Search(args[1]);
                break;
            case "show":
                if (args.Length < 2) { Console.WriteLine("show <id>"); return; }
                w.Show(args[1]);
                break;
            case "favorite":
                if (args.Length < 2) { Console.WriteLine("favorite <id>"); return; }
                w.Favorite(args[1]);
                break;
            case "stats":
                w.Stats();
                break;
            default:
                Console.WriteLine("Unknown command. Use add, list, search, show, favorite, stats.");
                break;
        }
    }

    static Dictionary<string, string> ParseArgs(string[] args)
    {
        var dict = new Dictionary<string, string>();
        for (int i = 1; i < args.Length; i++)
        {
            if (args[i].StartsWith("--") && i + 1 < args.Length)
            {
                dict[args[i].Substring(2)] = args[++i];
            }
        }
        return dict;
    }
}
