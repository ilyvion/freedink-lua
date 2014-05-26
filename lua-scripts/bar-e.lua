-- script for barrel with small heart in it

local items = include("_items")

function main()
  dink.preload_seq(173)
end

function hit()
  items.break_barrel(current_sprite, nil)
end

