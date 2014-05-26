-- this script fills life up, from a small food

function main()
  current_sprite.touch_damage = -1
  current_sprite.nohit = true
end

function touch()
  global.life = global.life + 3
  if global.life > global.lifemax then
    global.life = global.lifemax
  end
  dink.playsound(10, 22050, 0, nil, false)
  current_sprite.brain_parm = 10
  player:say("Yum!")
  current_sprite.brain = brain.BRAIN_PARM_SIZE_MATCH
  current_sprite.touch_damage = 0
  current_sprite.timing = 0
  
  local hold = current_sprite.editor_sprite
  if hold ~= nil then
    -- this was placed by the editor, lets make it not come back
    hold.type = editor_type.KILL_COMPLETELY
    -- kill food forever
  end
end

