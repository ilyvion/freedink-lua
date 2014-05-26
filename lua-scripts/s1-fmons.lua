function main()
  current_sprite.hitpoints = 30
  current_sprite.base_walk = 130
  current_sprite.exp = 30
  
  if global.farmer_quest == 1 then
    current_sprite:say_stop("`9You cannot defeat us Mr. Smallwood, we are too much for you.")
    current_sprite:say_stop("`9Try your best but you risk your life doing so...")
  end
  
  if dink.random(3, 1) == 1 then
    current_sprite:say_stop("`9RRrrr ar a rar a  arrgghhh.")
    current_sprite.speed = 9
    current_sprite.timing = 0
    dink.wait(4000)
    current_sprite.speed = 1
    current_sprite.timing = 0
  end
end

function hit(enemy_sprite)
  current_sprite.target = enemy_sprite
  dink.playsound(30, 17050, 4000, current_sprite, false)
  
  current_sprite:say("`9You cannot win, Smallwood.")
  current_sprite.speed = 9
  current_sprite.timing = 0
  dink.wait(2000)
  current_sprite.speed = 1
  current_sprite.timing = 33
end

function die()
  global.farmer_quest = 2
  global.exp = global.exp + 40
  current_sprite:say("`9Damn you ... damn .. you.")
end

