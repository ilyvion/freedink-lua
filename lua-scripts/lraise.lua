--raise their level

local choice_menu = dink.create_choice_menu()
choice_menu.y = 240
choice_menu.title = "YOU ARE NOW LEVEL " .. global.level .. [[

You may increase one of your attributes.]]
local choice_attack = choice_menu:add_choice("Increase Attack")
local choice_defense = choice_menu:add_choice("Increase Defense")
local choice_magic = choice_menu:add_choice("Increase Magic")

function raise()
  dink.playsound(33, 22000, 0, nil, false)
  dink.script_attach(1000)
  
  dink.playsound(10, 22050, 0, nil, false)
  dink.wait(1000)
  if global.level < 32 then
    dink.stop_entire_game(true)
    global.level = global.level + 1
    
    local choice_result = choice_menu:show()
    if choice_result == choice_attack then
      global.strength = global.strength + 1
    elseif choice_result == choice_defense then
      global.defense = global.defense + 1
    elseif choice_result == choice_magic then
      global.magic = global.magic + 1
    end
    
    global.lifemax = global.lifemax + 3
  else
	  global.exp = 0
	  player:say("What a gyp!")
  end
  
  dink.draw_status()
  dink.kill_this_task()
end

