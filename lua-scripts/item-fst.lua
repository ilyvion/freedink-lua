--item fists

local items = include("_items")

function use()
  items.use_item(100, 1, 8)
end

function disarm()
  dink.debug("Killed fists")
  dink.kill_this_task()
end

function arm()
  player.attack_hit_sound = 0
  
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
  dink.debug("fists armed")
end

function pickup()
  dink.debug("Player now owns this item.")
  dink.kill_this_task()
end

function drop()
  dink.debug("Item dropped.")
  dink.kill_this_task()
end

