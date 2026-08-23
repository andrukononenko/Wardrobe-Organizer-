# wardrobe.php
#!/usr/bin/env php
<?php

define('DATA_FILE', 'wardrobe.json');

class ClothingItem {
    public $id;
    public $name;
    public $category;
    public $photo;
    public $tags;
    public $date;
    public $favorite;
    public $created_at;

    function __construct($name, $category = '', $photo = '', $tags = '', $date = '') {
        $this->id = substr(bin2hex(random_bytes(4)), 0, 8);
        $this->name = $name;
        $this->category = $category;
        $this->photo = $photo;
        $this->tags = $tags;
        $this->date = $date ?: date('Y-m-d');
        $this->favorite = false;
        $this->created_at = date('c');
    }

    function toArray() {
        return [
            'id' => $this->id,
            'name' => $this->name,
            'category' => $this->category,
            'photo' => $this->photo,
            'tags' => $this->tags,
            'date' => $this->date,
            'favorite' => $this->favorite,
            'created_at' => $this->created_at
        ];
    }

    static function fromArray($data) {
        $item = new self($data['name'], $data['category'], $data['photo'], $data['tags'], $data['date']);
        $item->id = $data['id'];
        $item->favorite = $data['favorite'] ?? false;
        $item->created_at = $data['created_at'] ?? date('c');
        return $item;
    }
}

class Wardrobe {
    private $items = [];

    function __construct() {
        $this->load();
    }

    function load() {
        if (file_exists(DATA_FILE)) {
            $data = json_decode(file_get_contents(DATA_FILE), true);
            $this->items = array_map(function($d) { return ClothingItem::fromArray($d); }, $data);
        }
    }

    function save() {
        $data = array_map(function($i) { return $i->toArray(); }, $this->items);
        file_put_contents(DATA_FILE, json_encode($data, JSON_PRETTY_PRINT));
    }

    function add($name, $category = '', $photo = '', $tags = '', $date = '') {
        $item = new ClothingItem($name, $category, $photo, $tags, $date);
        $this->items[] = $item;
        $this->save();
        echo "✅ Added: $name (ID: {$item->id})\n";
    }

    function list($category = '') {
        $items = $this->items;
        if ($category) {
            $items = array_filter($items, function($i) use ($category) {
                return strcasecmp($i->category, $category) == 0;
            });
        }
        if (empty($items)) {
            echo "No items.\n";
            return;
        }
        echo "\n📋 Wardrobe (" . count($items) . " items):\n";
        $i = 1;
        foreach ($items as $item) {
            $fav = $item->favorite ? ' ⭐' : '';
            $tags = $item->tags ? " | {$item->tags}" : '';
            echo "  $i. {$item->name} [{$item->category}]{$fav}{$tags}\n";
            $i++;
        }
    }

    function search($term) {
        $lower = strtolower($term);
        $results = array_filter($this->items, function($i) use ($lower) {
            return strpos(strtolower($i->name), $lower) !== false ||
                   strpos(strtolower($i->category), $lower) !== false ||
                   strpos(strtolower($i->tags), $lower) !== false;
        });
        if (empty($results)) {
            echo "No matches.\n";
            return;
        }
        echo "\n🔍 Found " . count($results) . " item(s):\n";
        $i = 1;
        foreach ($results as $item) {
            $fav = $item->favorite ? ' ⭐' : '';
            $tags = $item->tags ? " | {$item->tags}" : '';
            echo "  $i. {$item->name} [{$item->category}]{$fav}{$tags}\n";
            $i++;
        }
    }

    function show($id) {
        foreach ($this->items as $item) {
            if ($item->id == $id) {
                echo "\n👔 {$item->name}\n";
                echo "  ID: {$item->id}\n";
                echo "  Category: " . ($item->category ?: 'Uncategorized') . "\n";
                echo "  Photo: " . ($item->photo ?: 'No photo') . "\n";
                echo "  Tags: " . ($item->tags ?: 'None') . "\n";
                echo "  Date: {$item->date}\n";
                echo "  Favorite: " . ($item->favorite ? 'Yes' : 'No') . "\n";
                echo "  Added: {$item->created_at}\n";
                return;
            }
        }
        echo "Item $id not found.\n";
    }

    function favorite($id) {
        foreach ($this->items as $item) {
            if ($item->id == $id) {
                $item->favorite = !$item->favorite;
                $this->save();
                $status = $item->favorite ? '⭐ added to' : '❌ removed from';
                echo "✅ $status favorites: {$item->name}\n";
                return;
            }
        }
        echo "Item $id not found.\n";
    }

    function stats() {
        if (empty($this->items)) {
            echo "No items.\n";
            return;
        }
        $categories = [];
        foreach ($this->items as $item) {
            $cat = $item->category ?: 'Uncategorized';
            $categories[$cat] = ($categories[$cat] ?? 0) + 1;
        }
        echo "\n📊 Statistics:\n";
        ksort($categories);
        foreach ($categories as $cat => $count) {
            echo "  $cat: $count item(s)\n";
        }
    }
}

if ($argc < 2) {
    die("Usage: php wardrobe.php <command> [options]\n");
}

$w = new Wardrobe();
$cmd = $argv[1];

switch ($cmd) {
    case 'add':
        if ($argc < 3) die("add <name> [--category CAT] [--photo PATH] [--tags TAGS] [--date DATE]\n");
        $name = $argv[2];
        $category = $photo = $tags = $date = '';
        for ($i=3; $i<$argc; $i++) {
            if ($argv[$i] == '--category' && isset($argv[$i+1])) { $category = $argv[++$i]; }
            if ($argv[$i] == '--photo' && isset($argv[$i+1])) { $photo = $argv[++$i]; }
            if ($argv[$i] == '--tags' && isset($argv[$i+1])) { $tags = $argv[++$i]; }
            if ($argv[$i] == '--date' && isset($argv[$i+1])) { $date = $argv[++$i]; }
        }
        $w->add($name, $category, $photo, $tags, $date);
        break;

    case 'list':
        $category = '';
        for ($i=2; $i<$argc; $i++) {
            if ($argv[$i] == '--category' && isset($argv[$i+1])) { $category = $argv[++$i]; }
        }
        $w->list($category);
        break;

    case 'search':
        if ($argc < 3) die("search <term>\n");
        $w->search($argv[2]);
        break;

    case 'show':
        if ($argc < 3) die("show <id>\n");
        $w->show($argv[2]);
        break;

    case 'favorite':
        if ($argc < 3) die("favorite <id>\n");
        $w->favorite($argv[2]);
        break;

    case 'stats':
        $w->stats();
        break;

    default:
        echo "Unknown command. Use add, list, search, show, favorite, stats.\n";
}
?>
