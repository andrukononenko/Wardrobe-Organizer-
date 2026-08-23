// wardrobe.js
#!/usr/bin/env node
const fs = require('fs');
const { program } = require('commander');
const { v4: uuidv4 } = require('uuid');

const DATA_FILE = 'wardrobe.json';

class ClothingItem {
    constructor(name, category = '', photo = '', tags = '', date = '') {
        this.id = uuidv4().slice(0,8);
        this.name = name;
        this.category = category;
        this.photo = photo;
        this.tags = tags;
        this.date = date || new Date().toISOString().slice(0,10);
        this.favorite = false;
        this.created_at = new Date().toISOString();
    }
}

class Wardrobe {
    constructor() {
        this.items = [];
        this.load();
    }

    load() {
        if (fs.existsSync(DATA_FILE)) {
            try {
                this.items = JSON.parse(fs.readFileSync(DATA_FILE));
            } catch (e) {}
        }
    }

    save() {
        fs.writeFileSync(DATA_FILE, JSON.stringify(this.items, null, 2));
    }

    add(name, category, photo, tags, date) {
        const item = new ClothingItem(name, category, photo, tags, date);
        this.items.push(item);
        this.save();
        console.log(`✅ Added: ${name} (ID: ${item.id})`);
    }

    list(category) {
        let items = this.items;
        if (category) {
            items = items.filter(i => i.category.toLowerCase() === category.toLowerCase());
        }
        if (!items.length) {
            console.log('No items.');
            return;
        }
        console.log(`\n📋 Wardrobe (${items.length} items):`);
        items.forEach((item, i) => {
            const fav = item.favorite ? ' ⭐' : '';
            const tags = item.tags ? ` | ${item.tags}` : '';
            console.log(`  ${i+1}. ${item.name} [${item.category}]${fav}${tags}`);
        });
    }

    search(term) {
        const lower = term.toLowerCase();
        const results = this.items.filter(i =>
            i.name.toLowerCase().includes(lower) ||
            i.category.toLowerCase().includes(lower) ||
            i.tags.toLowerCase().includes(lower)
        );
        if (!results.length) {
            console.log('No matches.');
            return;
        }
        console.log(`\n🔍 Found ${results.length} item(s):`);
        results.forEach((item, i) => {
            const fav = item.favorite ? ' ⭐' : '';
            const tags = item.tags ? ` | ${item.tags}` : '';
            console.log(`  ${i+1}. ${item.name} [${item.category}]${fav}${tags}`);
        });
    }

    show(id) {
        const item = this.items.find(i => i.id === id);
        if (!item) {
            console.log(`Item ${id} not found.`);
            return;
        }
        console.log(`\n👔 ${item.name}`);
        console.log(`  ID: ${item.id}`);
        console.log(`  Category: ${item.category || 'Uncategorized'}`);
        console.log(`  Photo: ${item.photo || 'No photo'}`);
        console.log(`  Tags: ${item.tags || 'None'}`);
        console.log(`  Date: ${item.date}`);
        console.log(`  Favorite: ${item.favorite ? 'Yes' : 'No'}`);
        console.log(`  Added: ${item.created_at}`);
    }

    favorite(id) {
        const item = this.items.find(i => i.id === id);
        if (!item) {
            console.log(`Item ${id} not found.`);
            return;
        }
        item.favorite = !item.favorite;
        this.save();
        const status = item.favorite ? '⭐ added to' : '❌ removed from';
        console.log(`✅ ${status} favorites: ${item.name}`);
    }

    stats() {
        if (!this.items.length) {
            console.log('No items.');
            return;
        }
        const categories = {};
        for (const item of this.items) {
            const cat = item.category || 'Uncategorized';
            categories[cat] = (categories[cat] || 0) + 1;
        }
        console.log('\n📊 Statistics:');
        for (const [cat, count] of Object.entries(categories).sort()) {
            console.log(`  ${cat}: ${count} item(s)`);
        }
    }
}

program
    .command('add <name>')
    .option('--category <category>', 'Category')
    .option('--photo <path>', 'Photo path')
    .option('--tags <tags>', 'Comma-separated tags')
    .option('--date <date>', 'Purchase date')
    .action((name, options) => {
        const w = new Wardrobe();
        w.add(name, options.category || '', options.photo || '', options.tags || '', options.date || '');
    });

program
    .command('list')
    .option('--category <category>', 'Filter by category')
    .action((options) => {
        const w = new Wardrobe();
        w.list(options.category || '');
    });

program
    .command('search <term>')
    .action((term) => {
        const w = new Wardrobe();
        w.search(term);
    });

program
    .command('show <id>')
    .action((id) => {
        const w = new Wardrobe();
        w.show(id);
    });

program
    .command('favorite <id>')
    .action((id) => {
        const w = new Wardrobe();
        w.favorite(id);
    });

program
    .command('stats')
    .action(() => {
        const w = new Wardrobe();
        w.stats();
    });

program.parse(process.argv);
