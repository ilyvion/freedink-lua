--script for chest with heart in it

local items = include("_items")

function main()
  dink.preload_seq(175)
  current_sprite.brain = brain.NONE
  current_sprite.hitpoints = 0
end

function hit()
  items.open_chest(current_sprite, "heart")
end

