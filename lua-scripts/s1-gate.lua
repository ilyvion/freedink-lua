function main()
  if global.little_girl < 2 then
    local ran = dink.random(2, 1)
    if ran == 1 then
      dink.preload_seq(331)
      dink.preload_seq(333)
      dink.preload_seq(337)
      dink.preload_seq(339)
      
      local girl = dink.create_sprite(630, 180, brain.MONSTER_DIAGONAL, 331, 4)
      girl.script = "s1-lg"
      girl.base_walk = 330
      girl.timing = 33
      girl.speed = 1
      girl:move(direction.WEST, 590, true)
    end
  end
  
  if global.story == 1 or global.story == 5 then
    global.vision = 1
  end
end

