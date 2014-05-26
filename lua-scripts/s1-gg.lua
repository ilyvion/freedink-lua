function hit()
  local mrandom = dink.random(3, 1)
  if mrandom == 1 then
    current_sprite:say_stop("`4Watch it boy, even my temper may wear eventually.")
  elseif mrandom == 2 then
    current_sprite:say_stop("`4Oh, a warrior are you? <chuckle>")
  elseif mrandom == 3 then
    current_sprite:say_stop("`4Nice... Have you been practicing that on your pigs?")
  end
end

function talk()
  player:move_stop(direction.SOUTH, 193, true)
  player:move_stop(direction.NORTH, 190, true)
  
  if global.story == 5 then
    player:freeze()
    global.letter = 1
    current_sprite:say_stop("`4Sorry about what happened Dink, I hope you're ok.")
    player:say_stop("Thanks, I'll be ok.")
    dink.wait(250)
    current_sprite:say_stop("`4By the way, a letter came for you.  It's at your house,")
    current_sprite:say_stop("`4you should go take a look at it.")
    dink.wait(250)
    player:say_stop("Thanks.")
    player:unfreeze()
    return
  end
  
  player:freeze()
  
  local choice_menu = dink.create_choice_menu()
  local choice_let_out = choice_menu:add_choice("Ask the guard to let you out")
  choice_menu:add_choice("Forget it")
  
  local choice_result = choice_menu:show()
  
  if choice_result == choice_let_out then
    player:say_stop("Hey, guard Renton.  I need to go do something, please let me out.")
    dink.wait(200)
    current_sprite:say_stop("`4It's much too dangerous for a boy out there, Dink.")
    dink.wait(200)
    player:say_stop("Boy?  Please refer to me as warrior from now on.")
    dink.wait(200)
    current_sprite:say_stop("`4Hardly... go tend your livestock. <chuckle>")
  end
  
  player:unfreeze()
end

