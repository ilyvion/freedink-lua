function talk()
  player:freeze()
  current_sprite:freeze()
  
  local choice_menu = dink.create_choice_menu()
  local choice_home = choice_menu:add_choice("Tell the duck to go home")
  local choice_yell = choice_menu:add_choice("Yell at it")
  
  local choice_result = choice_menu:show()
  
  if choice_result == choice_home then
    dink.wait(500)
    player:say_stop("Hey little duck, you gotta get home to Ethel.")
    dink.wait(500)
    current_sprite:say_stop("`0QUACK!!")
    dink.wait(500)
  elseif choice_result == choice_yell then
    dink.wait(500)
    player:say_stop("You suck little duck guy, not even I run away from home.")
    dink.wait(250)
    player:say_stop("You should be ashamed.")
    dink.wait(500)
    current_sprite:say_stop("`0Bite me")
    dink.wait(250)
    current_sprite:say_stop("`0But fine, I'll go home...")
    dink.wait(500)
    global.old_womans_duck = 2
    current_sprite.speed = 2
    current_sprite.timing = 0
    current_sprite:move_stop(direction.NORTH, -12, true)
    current_sprite.active = false
  end
  
  player:unfreeze()
  current_sprite:unfreeze()
end

function die()
  global.old_womans_duck = 3
  player:say_stop("Haw haw!")
end

