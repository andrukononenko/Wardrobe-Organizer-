👔 Wardrobe Organizer (Clothing Photos) — Multi‑Language Clothing Manager
8 languages, one complete wardrobe manager – catalog your clothing items, attach photo paths, organize by categories, and keep track of your style – right from your terminal.

✨ Features
👗 Add clothing items – name, category, photo path, tags, and purchase date

📂 Categories – organize by Tops, Bottoms, Shoes, Accessories, Outerwear, etc.

📋 List items – filter by category or search by tags

📸 Photo support – store file paths to clothing photos

🔍 Search – by name, category, or tags

📊 Statistics – see how many items per category

💾 Persistent storage – all data saved in wardrobe.json

⭐ Favorites – mark items as favorites

🧰 Supported Languages & Files
Language	File	Dependencies
Python	wardrobe.py	none (stdlib)
Go	wardrobe.go	none (stdlib)
JavaScript (Node)	wardrobe.js	commander (optional)
Ruby	wardrobe.rb	json, date
PHP	wardrobe.php	none (extensions)
Java	Wardrobe.java	Java 8+
C#	Wardrobe.cs	.NET Core 3.1+
C++	wardrobe.cpp	nlohmann/json
🚀 Quick Start
All implementations follow the same CLI pattern:

bash
# Add a clothing item
<command> add "Blue Denim Jacket" --category "Outerwear" --photo ~/photos/jacket.jpg --tags "casual,denim"

# Add with purchase date
<command> add "White Sneakers" --category "Shoes" --photo ~/photos/sneakers.jpg --date 2026-06-15

# List all items
<command> list

# List items by category
<command> list --category Tops

# Search by tag
<command> search "casual"

# Show item details
<command> show 1

# Mark item as favorite
<command> favorite 1

# Show statistics
<command> stats
Commands:

add <name> [--category CAT] [--photo PATH] [--tags TAGS] [--date DATE] – add item

list [--category CAT] – list items (optionally filtered)

search <term> – search by name, category, or tags

show <id> – show item details

favorite <id> – mark/unmark as favorite

stats – show category statistics

📸 Example Output
text
📋 Wardrobe (6 items):
  1. Blue Denim Jacket [Outerwear] ⭐ | casual,denim
  2. White Sneakers [Shoes] | casual
  3. Black T-Shirt [Tops] | basic
  4. Jeans [Bottoms] | casual,denim
  5. Leather Belt [Accessories] | formal
  6. Wool Coat [Outerwear] ⭐ | winter,formal

📊 Statistics:
  Outerwear: 2 items
  Shoes: 1 item
  Tops: 1 item
  Bottoms: 1 item
  Accessories: 1 item
📁 Repository Structure
text
.
├── README.md
├── python/
│   └── wardrobe.py
├── go/
│   └── wardrobe.go
├── javascript/
│   └── wardrobe.js
├── ruby/
│   └── wardrobe.rb
├── php/
│   └── wardrobe.php
├── java/
│   └── Wardrobe.java
├── csharp/
│   └── Wardrobe.cs
└── cpp/
    └── wardrobe.cpp
