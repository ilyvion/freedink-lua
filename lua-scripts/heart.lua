-- this script fills life up, touch_damage must be set to -1. (run script mode)

function main()
  current_sprite.seq = 52
  current_sprite.frame = 1 -- so the seq will start
  current_sprite.brain = brain.REPEAT
  current_sprite.touch_damage = -1
  current_sprite.nohit = true
end

function touch()
  global.life = global.lifemax
  dink.playsound(10, 22050, 0, nil, false)
  current_sprite.brain_parm = 5
  current_sprite.brain = brain.BRAIN_PARM_SIZE_MATCH
  current_sprite.touch_damage = 0
  current_sprite.timing = 0
  
  local hold = current_sprite.editor_sprite
  if hold ~= nil then
    -- this was placed by the editor, lets make it not come back
    hold.type = editor_type.KILL_COMPLETELY
  end
end

