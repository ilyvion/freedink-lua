function main()
  if global.story > 6 then
    return
  end
  
  global.vision = 1
  local guy = dink.create_sprite(400, 250, brain.MONSTER_DIAGONAL, 373, 4)
  guy:freeze()
  guy.speed = 1
  guy.timing = 33
  guy.base_walk = 270
  guy.script = "s1-brg2"
end

