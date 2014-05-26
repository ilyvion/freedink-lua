local misc = include("_misc")

function main()
  current_sprite.seq = 449
  current_sprite.sound = 34
  current_sprite.brain = brain.REPEAT
  current_sprite.hitpoints = 0
end

function hit()
  player:say("Die, strange machine that doesn't belong here!")
end

function talk()
  dink.playsound(18, 22050, 0, nil, false)
  
  player:freeze()
  
  local choice_menu = dink.create_choice_menu()
  local choice_save = choice_menu:add_choice("Save your game")
  local choice_leave = choice_menu:add_choice("Leave the strange machine")
  
  local choice_result = choice_menu:show()
  
  if choice_result == choice_leave then
    player:unfreeze()
    return
  end
  
  local save_result = misc.show_savegame_menu()
  player:unfreeze()
  if save_result ~= nil then
    dink.save_game(save_result)
    dink.say_xy("`%Game saved", 1, 30)
  end
end

