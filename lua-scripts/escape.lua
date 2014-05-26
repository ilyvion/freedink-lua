-- this is run when the escape key is pressed

local misc = include("_misc")

function main()
  dink.playsound(18, 22050, 0, nil, false)
  player:freeze()
  
  -- Prepare escape menu
  local escape_menu = dink.create_choice_menu()
  local escape_load = escape_menu:add_choice("Load a previously saved game")
  local escape_restart = escape_menu:add_choice("Restart")
  local escape_quit = escape_menu:add_choice("Quit to system")
  local escape_help = escape_menu:add_choice("Help")
  local escape_continue = escape_menu:add_choice("Continue")
  local escape_gamepad = escape_menu:add_choice("View/change gamepad buttons")
  local escape_save = escape_menu:add_choice("Save game")
  --escape_save.condition = false -- Disable save-anywhere choice
  
  -- Prepare quit menu
  local quit_menu = dink.create_choice_menu()
  local quit_yes = quit_menu:add_choice("Yes, I really want to quit the game")
  local quit_no = quit_menu:add_choice("I was just kidding, back to the action, please")
  
  -- Prepare help menu
  local help_menu = dink.create_choice_menu()
  help_menu.y = 240
  help_menu.title = 
[[



What would you like help on?]]
  local help_keyboard = help_menu:add_choice("Keyboard commands")
  local help_save = help_menu:add_choice("How to save the game")
  local help_done = help_menu:add_choice("Done")
  
  -- Prepare keyboard help menu
  local keyboard_menu = dink.create_choice_menu()
  keyboard_menu.y = 240
  keyboard_menu.title =
[[Ctrl = Attack/choose
Space = Talk/examine/skip text
Shift = Magic
Enter = Item/magic equip screen


Use the arrow keys to move.  Joystick and control pad are also supported.]]
  keyboard_menu:add_choice("Ok")
  
  -- Prepare save help menu
  local save_help_menu = dink.create_choice_menu()
  save_help_menu.y = 240
  save_help_menu.title =
[[In this quest, saving your game can
only be done at the special 'Save
Machine'. (it hums strangely)
]]
  save_help_menu:add_choice("Ok")
  
  -- Prepare restart menu
  local restart_menu = dink.create_choice_menu()
  local restart_yes = restart_menu:add_choice("Yes, I really want to restart from scratch")
  local restart_no = restart_menu:add_choice("No, go back!")
  
  -- Prepare new button menu
  local newbutton_menu = dink.create_choice_menu()
  newbutton_menu.y = 140
  newbutton_menu:add_choice("Attack")
  newbutton_menu:add_choice("Talk/look")
  newbutton_menu:add_choice("Magic")
  newbutton_menu:add_choice("Item screen")
  newbutton_menu:add_choice("Main menu")
  newbutton_menu:add_choice("View map (if you have one)")
  local newbutton_nevermind = newbutton_menu:add_choice("Nevermind")

  while true do
    local escape_result = escape_menu:show()
    
    if escape_result == escape_continue then
      player:unfreeze()
      dink.kill_this_task()
      return
    elseif escape_result == escape_load then
      local save_result = misc.show_savegame_menu()
      
      if save_result ~= nil and dink.game_exist(save_result) then
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

        dink.init([[load_sequence_now graphics\dink\hit\normal\ds-h2- 102 75 60 72 -19 -9 19 9]])
        dink.init([[load_sequence_now graphics\dink\hit\normal\ds-h4- 104 75 61 73 -19 -10 19 10]])

        dink.init([[load_sequence_now graphics\dink\hit\normal\ds-h6- 106 75 58 71 -18 -10 18 10]])
        dink.init([[load_sequence_now graphics\dink\hit\normal\ds-h8- 108 75 61 71 -19 -10 19 10]])

        player:unfreeze()

        dink.load_game(save_result)
        global.update_status = 1
        dink.draw_status()

        dink.kill_this_task()
      else
        player:unfreeze()
        dink.wait(2000)
        player:say("Wow, this loaded game looks so familiar.")
        dink.kill_this_task()
        return
      end
    elseif escape_result == escape_quit then
      local quit_result = quit_menu:show()
      
      if quit_result == quit_no then
        dink.wait(300)
        player:say("Phew, that was a close one!")
      elseif quit_result == quit_yes then
        dink.kill_game()
      end
      
      player:unfreeze()
      dink.kill_this_task()
    elseif escape_result == escape_help then
      while true do
        local help_result = help_menu:show()
        
        if help_result == help_keyboard then
          keyboard_menu:show()
        elseif help_result == help_save then
          save_help_menu:show()
        else
          break
        end
      end
    elseif escape_result == escape_restart then
      local restart_result = restart_menu:show()
      
      if restart_result == restart_yes then
        player:unfreeze()
        dink.restart_game()
        dink.kill_this_task()
      end
    elseif escape_result == escape_gamepad then
      while true do
        local button_result = misc.show_button_menu()
        if button_result ~= nil then
          -- While the menu was prepared above, the title needs to change each time
          newbutton_menu.title = "What should button "..global.result.." do?"
          
          local newbutton_result = newbutton_menu:show()
          if newbutton_result ~= newbutton_nevermind then
            dink.set_button(button_result, newbutton_result)
          end
        else
          break
        end
      end
    elseif escape_result == escape_save then
      local save_result = misc.show_savegame_menu()
      player:unfreeze()
      if save_result ~= nil then
        dink.save_game(save_result)
        dink.say_xy("`%Game saved", 1, 30)
      end
    end
  end
end

