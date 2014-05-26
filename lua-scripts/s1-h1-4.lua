
function main()
  if global.story > 3 then
    for i = 20,24 do
      local who = dink.get_editor_sprite(i).sprite
      if who ~= nil then
        who.active = false
      end
    end
    
    current_sprite.active = false
    dink.draw_hard_map()
  end
end

function talk()
  if global.story >= 3 then
    player:say("It'll catch fire soon.")
    return
  end
  player:say("I'm not hungry right now..")
end

function hit()
  if global.story >= 3 then
    player:say("Ahh, I can't save it.")
    return
  end
  player:say("Why must I attack furniture?")
end

function push()
  dink.wait(500)
  
  if global.story >= 3 then
    player:say("It's too big too move!")
  end
  player:say("This table must be bolted to the ground.")
end

