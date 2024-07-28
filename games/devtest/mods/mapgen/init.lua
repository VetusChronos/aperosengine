--
-- Aliases for map generator outputs
--

-- ESSENTIAL node aliases
-- Basic nodes
aperosengine.register_alias("mapgen_stone", "basenodes:stone")
aperosengine.register_alias("mapgen_water_source", "basenodes:water_source")
aperosengine.register_alias("mapgen_river_water_source", "basenodes:river_water_source")

-- Additional essential aliases for v6
aperosengine.register_alias("mapgen_lava_source", "basenodes:lava_source")
aperosengine.register_alias("mapgen_dirt", "basenodes:dirt")
aperosengine.register_alias("mapgen_dirt_with_grass", "basenodes:dirt_with_grass")
aperosengine.register_alias("mapgen_sand", "basenodes:sand")
aperosengine.register_alias("mapgen_tree", "basenodes:tree")
aperosengine.register_alias("mapgen_leaves", "basenodes:leaves")
aperosengine.register_alias("mapgen_apple", "basenodes:apple")

-- Essential alias for dungeons
aperosengine.register_alias("mapgen_cobble", "basenodes:cobble")

-- Optional aliases for v6 (they all have fallback values in the engine)
if aperosengine.settings:get_bool("devtest_v6_mapgen_aliases", false) then
	aperosengine.register_alias("mapgen_gravel", "basenodes:gravel")
	aperosengine.register_alias("mapgen_desert_stone", "basenodes:desert_stone")
	aperosengine.register_alias("mapgen_desert_sand", "basenodes:desert_sand")
	aperosengine.register_alias("mapgen_dirt_with_snow", "basenodes:dirt_with_snow")
	aperosengine.register_alias("mapgen_snowblock", "basenodes:snowblock")
	aperosengine.register_alias("mapgen_snow", "basenodes:snow")
	aperosengine.register_alias("mapgen_ice", "basenodes:ice")
	aperosengine.register_alias("mapgen_junglegrass", "basenodes:junglegrass")
	aperosengine.register_alias("mapgen_jungletree", "basenodes:jungletree")
	aperosengine.register_alias("mapgen_jungleleaves", "basenodes:jungleleaves")
	aperosengine.register_alias("mapgen_pine_tree", "basenodes:pine_tree")
	aperosengine.register_alias("mapgen_pine_needles", "basenodes:pine_needles")
end
-- Optional alias for mossycobble (should fall back to cobble)
if aperosengine.settings:get_bool("devtest_dungeon_mossycobble", false) then
	aperosengine.register_alias("mapgen_mossycobble", "basenodes:mossycobble")
end
-- Optional aliases for dungeon stairs (should fall back to full nodes)
if aperosengine.settings:get_bool("devtest_dungeon_stairs", false) then
	aperosengine.register_alias("mapgen_stair_cobble", "stairs:stair_cobble")
	if aperosengine.settings:get_bool("devtest_v6_mapgen_aliases", false) then
		aperosengine.register_alias("mapgen_stair_desert_stone", "stairs:stair_desert_stone")
	end
end

--
-- Register biomes for biome API
--

aperosengine.clear_registered_biomes()
aperosengine.clear_registered_decorations()

if aperosengine.settings:get_bool("devtest_register_biomes", true) then
	aperosengine.register_biome({
		name = "mapgen:grassland",
		node_top = "basenodes:dirt_with_grass",
		depth_top = 1,
		node_filler = "basenodes:dirt",
		depth_filler = 1,
		node_riverbed = "basenodes:sand",
		depth_riverbed = 2,
		node_dungeon = "basenodes:cobble",
		node_dungeon_alt = "basenodes:mossycobble",
		node_dungeon_stair = "stairs:stair_cobble",
		y_max = 31000,
		y_min = 4,
		heat_point = 50,
		humidity_point = 50,
	})

	aperosengine.register_biome({
		name = "mapgen:grassland_ocean",
		node_top = "basenodes:sand",
		depth_top = 1,
		node_filler = "basenodes:sand",
		depth_filler = 3,
		node_riverbed = "basenodes:sand",
		depth_riverbed = 2,
		node_cave_liquid = "basenodes:water_source",
		node_dungeon = "basenodes:cobble",
		node_dungeon_alt = "basenodes:mossycobble",
		node_dungeon_stair = "stairs:stair_cobble",
		y_max = 3,
		y_min = -255,
		heat_point = 50,
		humidity_point = 50,
	})

	aperosengine.register_biome({
		name = "mapgen:grassland_under",
		node_cave_liquid = {"basenodes:water_source", "basenodes:lava_source"},
		node_dungeon = "basenodes:cobble",
		node_dungeon_alt = "basenodes:mossycobble",
		node_dungeon_stair = "stairs:stair_cobble",
		y_max = -256,
		y_min = -31000,
		heat_point = 50,
		humidity_point = 50,
	})
end
