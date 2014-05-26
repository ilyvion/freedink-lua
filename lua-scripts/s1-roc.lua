function main()
  current_sprite.speed = 1
  if global.rock_placement == 1 then
    current_sprite.x = 350
    dink.draw_hard_map()
  end
end

function talk()
  player:say_stop("Hey, looks like there's an opening behind this rock.")
end

function push()

  if global.strength < 4 then
    player:say_stop("It's too heavy for me.  If I was a little stronger...")
    return
  end
  
  if global.rock_placement == 0 and player.dir == direction.EAST then
    player:say("It's .. it's moving...")
    player:freeze()
    current_sprite:move_stop(direction.EAST, 350, true)
    player:unfreeze()
    dink.draw_hard_map()
    global.rock_placement = 1
  elseif global.rock_placement == 1 and player.dir == direction.WEST then
    player:say("..heavy..heavy..")
    player:freeze()
    player:move_stop(direction.WEST, 285, true)
    player:unfreeze()
    dink.draw_hard_map()
    global.rock_placement = 0
  else
    player:say("It won't budge from this angle.")
  end
end

