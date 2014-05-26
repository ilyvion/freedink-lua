--for quit button

local crap

function buttonon()
  current_sprite.pframe = 2
  dink.playsound(20, 22050, 0, nil, false)
  crap = dink.create_sprite(446, 417, brain.NONE, 198, 1)
  crap.noclip = true
  crap.reverse = false
  crap.seq = 198
end

function buttonoff()
  current_sprite.pframe = 1
  dink.playsound(21, 22050, 0, nil, false)
  crap.brain = brain.KILL_SEQ_DONE
  crap.reverse = true
  crap.seq = 198
end

function click()
  dink.playsound(17, 22050, 0, nil, false)
  dink.kill_game()
end

