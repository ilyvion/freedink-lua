function main()
  current_sprite.touch_damage = -1
end

function touch()
  if dink.free_items() < 1 then
    player:say("I'm full!  I can't pick up anything else.")
    return
  end
  
  dink.playsound(10, 22050, 0, nil, false)
  dink.add_item("item-nut", 438, 19)
  
  if global.nuttree < 1 then
    global.nuttree = 3
    global.story = 3
  end
  
  player:say("I picked up a nut!")
  current_sprite.active = false
end

