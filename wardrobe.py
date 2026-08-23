# wardrobe.py
import json
import os
import argparse
import uuid
from datetime import datetime

DATA_FILE = "wardrobe.json"

class ClothingItem:
    def __init__(self, name, category="", photo="", tags="", date="", item_id=None):
        self.id = item_id or str(uuid.uuid4())[:8]
        self.name = name
        self.category = category
        self.photo = photo
        self.tags = tags
        self.date = date or datetime.now().strftime("%Y-%m-%d")
        self.favorite = False
        self.created_at = datetime.now().isoformat()

    def to_dict(self):
        return {
            "id": self.id,
            "name": self.name,
            "category": self.category,
            "photo": self.photo,
            "tags": self.tags,
            "date": self.date,
            "favorite": self.favorite,
            "created_at": self.created_at
        }

    @classmethod
    def from_dict(cls, data):
        item = cls(
            data["name"],
            data.get("category", ""),
            data.get("photo", ""),
            data.get("tags", ""),
            data.get("date", ""),
            data.get("id")
        )
        item.favorite = data.get("favorite", False)
        item.created_at = data.get("created_at", datetime.now().isoformat())
        return item

class Wardrobe:
    def __init__(self):
        self.items = []
        self.load()

    def load(self):
        if os.path.exists(DATA_FILE):
            with open(DATA_FILE, "r") as f:
                data = json.load(f)
                self.items = [ClothingItem.from_dict(item) for item in data]

    def save(self):
        with open(DATA_FILE, "w") as f:
            json.dump([item.to_dict() for item in self.items], f, indent=2)

    def add(self, name, category="", photo="", tags="", date=""):
        item = ClothingItem(name, category, photo, tags, date)
        self.items.append(item)
        self.save()
        print(f"✅ Added: {name} (ID: {item.id})")
        return item

    def list(self, category=None):
        items = self.items
        if category:
            items = [i for i in items if i.category.lower() == category.lower()]
        if not items:
            print("No items.")
            return
        print(f"\n📋 Wardrobe ({len(items)} items):")
        for i, item in enumerate(items, 1):
            fav = " ⭐" if item.favorite else ""
            tags = f" | {item.tags}" if item.tags else ""
            print(f"  {i}. {item.name} [{item.category}]{fav}{tags}")

    def search(self, term):
        term_lower = term.lower()
        results = []
        for item in self.items:
            if (term_lower in item.name.lower() or
                term_lower in item.category.lower() or
                term_lower in item.tags.lower()):
                results.append(item)
        if not results:
            print("No matches.")
            return
        print(f"\n🔍 Found {len(results)} item(s):")
        for i, item in enumerate(results, 1):
            fav = " ⭐" if item.favorite else ""
            tags = f" | {item.tags}" if item.tags else ""
            print(f"  {i}. {item.name} [{item.category}]{fav}{tags}")

    def show(self, item_id):
        for item in self.items:
            if item.id == item_id:
                print(f"\n👔 {item.name}")
                print(f"  ID: {item.id}")
                print(f"  Category: {item.category or 'Uncategorized'}")
                print(f"  Photo: {item.photo or 'No photo'}")
                print(f"  Tags: {item.tags or 'None'}")
                print(f"  Date: {item.date}")
                print(f"  Favorite: {'Yes' if item.favorite else 'No'}")
                print(f"  Added: {item.created_at}")
                return
        print(f"Item {item_id} not found.")

    def favorite(self, item_id):
        for item in self.items:
            if item.id == item_id:
                item.favorite = not item.favorite
                self.save()
                status = "⭐ added to" if item.favorite else "❌ removed from"
                print(f"✅ {status} favorites: {item.name}")
                return
        print(f"Item {item_id} not found.")

    def stats(self):
        if not self.items:
            print("No items.")
            return
        categories = {}
        for item in self.items:
            cat = item.category or "Uncategorized"
            categories[cat] = categories.get(cat, 0) + 1
        print("\n📊 Statistics:")
        for cat, count in sorted(categories.items()):
            print(f"  {cat}: {count} item(s)")

def main():
    parser = argparse.ArgumentParser(description="Wardrobe Organizer")
    subparsers = parser.add_subparsers(dest="cmd", required=True)

    add_parser = subparsers.add_parser("add")
    add_parser.add_argument("name")
    add_parser.add_argument("--category", default="")
    add_parser.add_argument("--photo", default="")
    add_parser.add_argument("--tags", default="")
    add_parser.add_argument("--date", default="")

    list_parser = subparsers.add_parser("list")
    list_parser.add_argument("--category", default="")

    search_parser = subparsers.add_parser("search")
    search_parser.add_argument("term")

    show_parser = subparsers.add_parser("show")
    show_parser.add_argument("item_id")

    fav_parser = subparsers.add_parser("favorite")
    fav_parser.add_argument("item_id")

    subparsers.add_parser("stats")

    args = parser.parse_args()
    wardrobe = Wardrobe()

    if args.cmd == "add":
        wardrobe.add(args.name, args.category, args.photo, args.tags, args.date)
    elif args.cmd == "list":
        wardrobe.list(args.category)
    elif args.cmd == "search":
        wardrobe.search(args.term)
    elif args.cmd == "show":
        wardrobe.show(args.item_id)
    elif args.cmd == "favorite":
        wardrobe.favorite(args.item_id)
    elif args.cmd == "stats":
        wardrobe.stats()

if __name__ == "__main__":
    main()
