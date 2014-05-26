function main()
  current_sprite.nodraw = true
  
  if global.story < 5 then
    current_sprite.touch_damage = -1
  end
end

function touch()
  player:say("I'd best not venture so far from town.  Too dangerous!")
  player:move_stop(direction.SOUTH, 200, true)
end

