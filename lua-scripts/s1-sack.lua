--sack of grain

function main()
  current_sprite.touch_damage = -1
end

function hit()
  player:say("I hate you, sack of feed!")
end

function talk()
  player:say("A big sack.")
end

function touch()
  dink.playsound(10, 22050, 0, nil, false)
  current_sprite.brain_parm = 10
  current_sprite.brain = brain.BRAIN_PARM_SIZE_MATCH
  current_sprite.touch_damage = 0
  current_sprite.timing = 0
  dink.add_item("item-pig", 438, 2)
  player:say("I now have a sack of pig feed.")
  
  -- kill this item so it doesn't show up again for this player
  local hold = current_sprite.editor_sprite
  if hold ~= nil then
    hold.type = editor_type.KILL_COMPLETELY
  end
end

