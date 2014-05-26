--item elixer

function use()
  global.life = global.life + 2
  if global.life > global.lifemax then
    global.life = global.lifemax
  end
  
  player:say("Yum, tasted good.")
  dink.kill_cur_item()
  -- nothing more will be read here, put stuff in disarm and drop if you need
  -- something done
end

function disarm()
  dink.debug("but disarmed.")
  
  dink.kill_this_task()
end

function arm()
  dink.preload_seq(169)
  
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
end

function pickup()
  dink.kill_this_task()
end

function drop()
  dink.debug("elixer removed");
  dink.draw_status()
  dink.kill_this_task()
end

