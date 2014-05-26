local misc = include("_misc")

local choice_menu = dink.create_choice_menu()
local choice_load = choice_menu:add_choice("Load a previously saved game")
local choice_restart = choice_menu:add_choice("Restart game")
local choice_quit = choice_menu:add_choice("Quit to system")

function die()
  player:freeze()
  global.update_status = 0
  
  player.seq = 436
  player.base_idle = -1
  dink.wait(3000)
  player.nohit = true
  player.brain = brain.NONE
  
  while true do
    local choice_result = choice_menu:show()
    
    if choice_result == choice_load then
      dink.playsound(18, 22050, 0, nil, false)
      local save_result = misc.show_savegame_menu(true)
      if dink.game_exist(save_result) then
        player.brain = brain.PLAYER
        player.nohit = false
        dink.stopmidi()
        dink.script_attach(1000)
        
        --script now can't die when the load is preformed..
        dink.init([[load_sequence_now graphics\dink\walk\ds-w1- 71 43 38 72 -14 -9 14 9]])
        dink.init([[load_sequence_now graphics\dink\walk\ds-w2- 72 43 37 69 -13 -9 13 9]])
        dink.init([[load_sequence_now graphics\dink\walk\ds-w3- 73 43 38 72 -14 -9 14 9]])
        dink.init([[load_sequence_now graphics\dink\walk\ds-w4- 74 43 38 72 -12 -9 12 9]])

        dink.init([[load_sequence_now graphics\dink\walk\ds-w6- 76 43 38 72 -13 -9 13 9]])
        dink.init([[load_sequence_now graphics\dink\walk\ds-w7- 77 43 38 72 -12 -10 12 10]])
        dink.init([[load_sequence_now graphics\dink\walk\ds-w8- 78 43 37 69 -13 -9 13 9]])
        dink.init([[load_sequence_now graphics\dink\walk\ds-w9- 79 43 38 72 -14 -9 14 9]])

        dink.init([[load_sequence_now graphics\dink\idle\ds-i2- 12 250 33 70 -12 -9 12 9]])
        dink.init([[load_sequence_now graphics\dink\idle\ds-i4- 14 250 30 71 -11 -9 11 9]])
        dink.init([[load_sequence_now graphics\dink\idle\ds-i6- 16 250 36 70 -11 -9 11 9]])
        dink.init([[load_sequence_now graphics\dink\idle\ds-i8- 18 250 32 68 -12 -9 12 9]])
        
        dink.load_game(save_result)
        --loading a game kills ALL tasks
        dink.kill_this_task()
      end
    elseif choice_result == choice_restart then
      player.nohit = false
      dink.restart_game()
    elseif choice_result == choice_quit then
      dink.kill_game()
    end
  end
end

