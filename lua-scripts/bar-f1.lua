-- script for barrel with food in it

local items = include("_items")

function main()
  dink.preload_seq(173)
  dink.preload_seq(421)
end

function hit()
  items.break_barrel(current_sprite, "food", 1)
end

