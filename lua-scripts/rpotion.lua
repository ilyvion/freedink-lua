--this script is for the red potion

function main()
  current_sprite.touch_damage = -1
  current_sprite.seq = 56
  current_sprite.brain = brain.REPEAT
end

function touch()
  local hold = current_sprite.editor_sprite
  if hold ~= nil then
    -- this was placed by the editor, lets make it not come back
    hold.type = editor_type.KILL_COMPLETELY
  end
  
  global.strength = global.strength + 1
  dink.playsound(10, 22050, 0, nil, false)
  current_sprite.brain_parm = 5
  current_sprite.brain = brain.BRAIN_PARM_SIZE_MATCH
  current_sprite.touch_damage = 0
  current_sprite.timing = 0
end

