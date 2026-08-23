# wardrobe.rb
#!/usr/bin/env ruby
require 'json'
require 'securerandom'
require 'date'

DATA_FILE = 'wardrobe.json'

class ClothingItem
  attr_accessor :id, :name, :category, :photo, :tags, :date, :favorite, :created_at

  def initialize(name, category = '', photo = '', tags = '', date = '')
    @id = SecureRandom.hex(4)
    @name = name
    @category = category
    @photo = photo
    @tags = tags
    @date = date.empty? ? Date.today.to_s : date
    @favorite = false
    @created_at = Time.now.iso8601
  end

  def to_hash
    {
      id: @id, name: @name, category: @category, photo: @photo,
      tags: @tags, date: @date, favorite: @favorite, created_at: @created_at
    }
  end

  def self.from_hash(h)
    item = new(h['name'], h['category'], h['photo'], h['tags'], h['date'])
    item.id = h['id']
    item.favorite = h['favorite'] || false
    item.created_at = h['created_at'] || Time.now.iso8601
    item
  end
end

class Wardrobe
  attr_reader :items

  def initialize
    @items = []
    load
  end

  def load
    if File.exist?(DATA_FILE)
      data = JSON.parse(File.read(DATA_FILE))
      @items = data.map { |h| ClothingItem.from_hash(h) }
    end
  end

  def save
    File.write(DATA_FILE, JSON.pretty_generate(@items.map(&:to_hash)))
  end

  def add(name, category = '', photo = '', tags = '', date = '')
    item = ClothingItem.new(name, category, photo, tags, date)
    @items << item
    save
    puts "✅ Added: #{name} (ID: #{item.id})"
  end

  def list(category = '')
    items = @items
    unless category.empty?
      items = items.select { |i| i.category.downcase == category.downcase }
    end
    if items.empty?
      puts "No items."
      return
    end
    puts "\n📋 Wardrobe (#{items.size} items):"
    items.each_with_index do |item, i|
      fav = item.favorite ? ' ⭐' : ''
      tags = item.tags.empty? ? '' : " | #{item.tags}"
      puts "  #{i+1}. #{item.name} [#{item.category}]#{fav}#{tags}"
    end
  end

  def search(term)
    lower = term.downcase
    results = @items.select do |i|
      i.name.downcase.include?(lower) ||
      i.category.downcase.include?(lower) ||
      i.tags.downcase.include?(lower)
    end
    if results.empty?
      puts "No matches."
      return
    end
    puts "\n🔍 Found #{results.size} item(s):"
    results.each_with_index do |item, i|
      fav = item.favorite ? ' ⭐' : ''
      tags = item.tags.empty? ? '' : " | #{item.tags}"
      puts "  #{i+1}. #{item.name} [#{item.category}]#{fav}#{tags}"
    end
  end

  def show(id)
    item = @items.find { |i| i.id == id }
    unless item
      puts "Item #{id} not found."
      return
    end
    puts "\n👔 #{item.name}"
    puts "  ID: #{item.id}"
    puts "  Category: #{item.category.empty? ? 'Uncategorized' : item.category}"
    puts "  Photo: #{item.photo.empty? ? 'No photo' : item.photo}"
    puts "  Tags: #{item.tags.empty? ? 'None' : item.tags}"
    puts "  Date: #{item.date}"
    puts "  Favorite: #{item.favorite ? 'Yes' : 'No'}"
    puts "  Added: #{item.created_at}"
  end

  def favorite(id)
    item = @items.find { |i| i.id == id }
    unless item
      puts "Item #{id} not found."
      return
    end
    item.favorite = !item.favorite
    save
    status = item.favorite ? '⭐ added to' : '❌ removed from'
    puts "✅ #{status} favorites: #{item.name}"
  end

  def stats
    if @items.empty?
      puts "No items."
      return
    end
    categories = Hash.new(0)
    @items.each { |i| categories[i.category.empty? ? 'Uncategorized' : i.category] += 1 }
    puts "\n📊 Statistics:"
    categories.sort.each { |cat, count| puts "  #{cat}: #{count} item(s)" }
  end
end

if ARGV.empty?
  puts "Usage: wardrobe.rb <command> [options]"
  exit
end

w = Wardrobe.new
cmd = ARGV.shift

case cmd
when 'add'
  name = ARGV.shift
  if name.nil?
    puts "add <name> [--category CAT] [--photo PATH] [--tags TAGS] [--date DATE]"
    exit
  end
  category = ''
  photo = ''
  tags = ''
  date = ''
  while ARGV.any?
    case ARGV[0]
    when '--category'
      ARGV.shift
      category = ARGV.shift || ''
    when '--photo'
      ARGV.shift
      photo = ARGV.shift || ''
    when '--tags'
      ARGV.shift
      tags = ARGV.shift || ''
    when '--date'
      ARGV.shift
      date = ARGV.shift || ''
    else
      break
    end
  end
  w.add(name, category, photo, tags, date)

when 'list'
  category = ''
  if ARGV.include?('--category')
    idx = ARGV.index('--category')
    category = ARGV[idx+1] if idx
  end
  w.list(category)

when 'search'
  term = ARGV.shift
  if term.nil?
    puts "search <term>"
    exit
  end
  w.search(term)

when 'show'
  id = ARGV.shift
  if id.nil?
    puts "show <id>"
    exit
  end
  w.show(id)

when 'favorite'
  id = ARGV.shift
  if id.nil?
    puts "favorite <id>"
    exit
  end
  w.favorite(id)

when 'stats'
  w.stats

else
  puts "Unknown command. Use add, list, search, show, favorite, stats."
end
