local man

function main()
  man = dink.get_editor_sprite(23).sprite
  if global.farmer_quest > 1 then
    current_sprite.active = false
    return
  end
  current_sprite.touch_damage = -1
end

function touch()
  player:freeze()
  if player.dir == direction.NORTH or player.dir == direction.NORTH_WEST then
    player:move_stop(direction.SOUTH, 177, true)
  elseif player.dir == direction.WEST then
    player:move_stop(direction.EAST, 166, true)
  end
  man:say("`7Not so fast, Smallwood!")
  player:unfreeze()
end

