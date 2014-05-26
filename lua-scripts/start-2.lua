--for continue button

local misc = include("_misc")

local crap

function buttonon()
  current_sprite.pframe = 2
  dink.playsound(20, 22050, 0, nil, false)
  crap = dink.create_sprite(358, 93, brain.NONE, 200, 1)
  crap.noclip = true
  crap.seq = 200
  crap.reverse = false
end

function buttonoff()
  current_sprite.pframe = 1
  dink.playsound(20, 22050, 0, nil, false)
  crap.brain = 7
  crap.reverse = true
  crap.seq = 200
end

function load()
  dink.playsound(18, 22050, 0, nil, false)
  
  local save_result = misc.show_savegame_menu()
  
  if save_result == nil then return true end
  if not dink.game_exist(save_result) then return false end
  
  dink.stopmidi()
  dink.stopcd()
  
  player.active = true
  player.x = 334
  player.y = 161
  player.base_walk = 70
  player.base_attack = 100
  player.dir = direction.WEST
  player.brain = brain.PLAYER
  player.que = 0
  player.noclip = false
  dink.set_mode(2)
  
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

  dink.init([[load_sequence_now graphics\dink\hit\normal\ds-h2- 102 75 60 72 -19 -9 19 9]])
  dink.init([[load_sequence_now graphics\dink\hit\normal\ds-h4- 104 75 61 73 -19 -10 19 10]])
  dink.init([[load_sequence_now graphics\dink\hit\normal\ds-h6- 106 75 58 71 -18 -10 18 10]])
  dink.init([[load_sequence_now graphics\dink\hit\normal\ds-h8- 108 75 61 71 -19 -10 19 10]])
  
  dink.load_game(save_result)
  dink.kill_this_task()
  return true
end

function click()
  player.brain = brain.NONE
  local result = load()
  player.brain = brain.MOUSE
  if not result then
    dink.say_xy("`%Try loading a saved game that exists, friend.", 0, 390)
  end
end

