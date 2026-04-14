extends Tree

func _ready():
	columns = 2
	column_titles_visible = true
	set_column_title(0, "Název")
	set_column_title(1, "Hodnota")
	hide_root = true # Schováme prázdný root uzel
	
func update_tree_view(all_sections: Array):
	var collapsed_states: Dictionary = {}
	var itemColapse = get_root()
	if itemColapse:
		itemColapse = itemColapse.get_first_child()
		while itemColapse:
			collapsed_states[itemColapse.get_text(0)] = itemColapse.is_collapsed()
			itemColapse = itemColapse.get_next()
	clear()

	var root = create_item()
	hide_root = true
	columns = 2
	set_column_title(0, "Property")
	set_column_title(1, "Value")
	column_titles_visible = true

	for section in all_sections:
		# 1. Vytvoření nadpisu sekce (Terrain)
		var category: TreeItem = create_item(root)
		category.set_text(0, str(section["title"]))
		category.set_selectable(0, false)
		category.set_selectable(1, false)
		category.set_custom_bg_color(0, Color(0.12, 0.12, 0.12))
		category.set_custom_bg_color(1, Color(0.12, 0.12, 0.12))
		category.set_collapsed(false)

		# 2. Přidání všech položek
		for item_data in section["items"]:
			var item: TreeItem = create_item(category)

			item.set_text(0, str(item_data["name"]))
			item.set_text(1, str(item_data["value"]))
			item.set_metadata(0, item_data["id"])

			# Volitelně: lepší zarovnání hodnoty doprava
			item.set_text_alignment(1, HORIZONTAL_ALIGNMENT_RIGHT)
		var title = str(section["title"])
		category.set_collapsed(collapsed_states.get(title, false))

# Příklad volání (např. odjinud nebo pro test):
# update_tree("Zbraně", [{"jmeno": "Dýka", "sila": 5}, {"jmeno": "Meč", "sila": 15}])
