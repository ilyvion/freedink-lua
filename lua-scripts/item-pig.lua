-- item pig food

local items = include("_items")

function use()
  local dir = player.dir
  local mholdx, mholdy = player.x, player.y
  
  items.use_item(520, 250, 13)
  
  local junk
  if dir == direction.WEST then
    mholdy = mholdy - 37
    mholdx = mholdx - 50
    junk = dink.create_sprite(mholdx, mholdy, brain.KILL_SEQ_DONE_LEAVE_LAST_FRAME, 430, 1)
    junk.seq = 430
  elseif dir == direction.EAST then
    mholdy = mholdy - 20
    mholdx = mholdx + 50
    junk = dink.create_sprite(mholdx, mholdy, brain.KILL_SEQ_DONE_LEAVE_LAST_FRAME, 431, 1)
    junk.seq = 431
  elseif dir == direction.NORTH then
    mholdy = mholdy - 50
    mholdx = mholdx + 8
    junk = dink.create_sprite(mholdx, mholdy, brain.KILL_SEQ_DONE_LEAVE_LAST_FRAME, 430, 1)
    junk.seq = 430
  elseif dir == direction.SOUTH then
    -- mholdy = mholdy + 0
    mholdx = mholdx - 2
    junk = dink.create_sprite(mholdx, mholdy, brain.KILL_SEQ_DONE_LEAVE_LAST_FRAME, 431, 1)
    junk.seq = 431
  end
  
  if global.pig_story ~= 0 then return end
  
  if global.player_map == 407 then
    -- they are feding the pigs.. maybe...
    
    if dink.inside_box(mholdx, mholdy, 200, 180, 400, 306) then
      player:freeze()
      dink.wait(200)
      player:say_stop("Come on pigs, eat!")
      -- lets create the bully, and run his script
      junk = dink.create_sprite(680, 200, brain.NONE, 341, 1)
      junk.script = "s1-bul"
    end
  end
end

function disarm()
  dink.kill_this_task()
end

function arm()
  dink.init([[load_sequence_now graphics\dink\walk\ds-w1- 71 35 38 72]])
  dink.init([[load_sequence_now graphics\dink\walk\ds-w2- 72 35 37 69 -13 -9 13 9]])
  dink.init([[load_sequence_now graphics\dink\walk\ds-w3- 73 35 38 72]])
  dink.init([[load_sequence_now graphics\dink\walk\ds-w4- 74 35 38 72]])

  dink.init([[load_sequence_now graphics\dink\walk\ds-w6- 76 35 38 72]])
  dink.init([[load_sequence_now graphics\dink\walk\ds-w7- 77 35 38 72]])
  dink.init([[load_sequence_now graphics\dink\walk\ds-w8- 78 35 37 69 -13 -9 13 9]])
  dink.init([[load_sequence_now graphics\dink\walk\ds-w9- 79 35 38 72]])

  dink.init([[load_sequence_now graphics\dink\idle\ds-i2- 12 250 33 70 -12 -9 12 9]])
  dink.init([[load_sequence_now graphics\dink\idle\ds-i4- 14 250 30 71 -11 -9 11 9]])
  dink.init([[load_sequence_now graphics\dink\idle\ds-i6- 16 250 36 70 -11 -9 11 9]])
  dink.init([[load_sequence_now graphics\dink\idle\ds-i8- 18 250 32 68 -12 -9 12 9]])

  dink.preload_seq(522)
  dink.preload_seq(524)
  dink.preload_seq(526)
  dink.preload_seq(528)
  dink.preload_seq(430)
  dink.preload_seq(431)
end

function pickup()
  dink.kill_this_task()
end

function drop()
  dink.kill_this_task()
end

