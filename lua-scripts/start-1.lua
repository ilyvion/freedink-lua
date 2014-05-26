-- for start button

local crap

function buttonon()
  current_sprite.pframe = 2
  dink.playsound(20, 22050, 0, nil, false)
  crap = dink.create_sprite(204, 86, brain.NONE, 199, 1)
  crap.reverse = false
  crap.noclip = true
  crap.seq = 199
end

function buttonoff()
  current_sprite.pframe = 1
  dink.playsound(21, 22050, 0, nil, false)
  crap.reverse = true
  crap.seq = 199
  crap.brain = brain.KILL_SEQ_DONE
end

function click()
  --lets start a new game
  dink.say_xy("`%Creating new game...", 0, 390)
  dink.wait(1)
  
  player.x = 334
  player.y = 161
  player.base_walk = 70
  player.base_attack = 100
  dink.set_mode(2) --turn game on
  dink.reset_timer()
  player.dir = direction.WEST
  player.brain = brain.PLAYER
  player.que = 0
  player.noclip = false
  --lets give him fists to start out with
  dink.add_item("item-fst", 438, 1)
  global.cur_weapon = 1
  --arm them for him too
  --dink.initfont("SWEDISH")
  dink.arm_weapon()
  --heed this too
  dink.kill_this_task()
end

