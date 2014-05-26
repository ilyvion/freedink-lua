function main()
  local mx, my = player.x, player.y
  
  if global.old_womans_duck == 1 then
    local hiding = dink.random(4, 1)
    dink.debug("random is "..hiding)
    
    if hiding == 1 then
      global.vision = 2
      player:say("There's that duck!!")
    else
      if dink.random(3, 1) == 1 then
        player:say("Where could Ethel's duck be?")
      end
    end
  end
end

