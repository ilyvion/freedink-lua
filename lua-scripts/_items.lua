--[[
Common item code. In here goes code that lots of items (both on-the-ground
items and equipped items) share.
--]]

-- We'll make use of the make functions, so let's import them.
local make = include("_make")

--[[
Common use-item code. Restricts the player's direction to non-diagonal
directions, runs an animation sequence based on a base sequence plus
the direction the player is facing, waits a certain amount of time,
then plays a specific sound effect.
--]]
function use_item(base_seq, use_wait, sound_number)
  -- disallow diagonal punches
  if player.dir == direction.SOUTH_WEST or player.dir == direction.SOUTH_EAST then
    player.dir = direction.SOUTH
  elseif player.dir == direction.NORTH_WEST or player.dir == direction.NORTH_EAST then
    player.dir = direction.NORTH
  end
  
  player.seq = base_seq + player.dir -- base_seq is the 'base' for the hit animations, we just add the direction
  player.frame = 1 -- reset seq to 1st frame
  player:kill_wait() -- make sure dink will play the animaition right away
  player.nocontrol = true -- dink can't move until anim is done!
  dink.wait(use_wait)
  dink.playsound(sound_number, 8000, 0, nil, false)
end

-- Used from bar-e.lua, bar-sh.lua and bar-f1.lua
function break_barrel(barrel_sprite, make_item, make_item_arg)
  -- play noise
  dink.playsound(37, 22050, 0, nil, false)
  
  local hold = barrel_sprite.editor_sprite
  if hold ~= nil then
    -- this was placed by the editor, lets make the barrel stay flat
    hold.type = editor_type.DRAW_BACKGROUND_WITHOUT_HARDNESS
    hold.seq = 173
    hold.frame = 6
  end
  
  barrel_sprite.seq = 173
  barrel_sprite.brain = brain.KILL_SEQ_DONE_LEAVE_LAST_FRAME
  barrel_sprite.notouch = true
  barrel_sprite.nohit = true
  
  if make_item ~= nil then
    --[[
      At this point, remember (or learn) that table.entry is really just
      syntactic sugar for table["entry"], which makes this equivalent to
      calling the function named whatever make_item is set to in the
      _make.lua file, e.g. make.sheart(x, y).
    --]]
    if make_item_arg == nil then
      make[make_item](barrel_sprite.x, barrel_sprite.y)
    else
      make[make_item](barrel_sprite.x, barrel_sprite.y, make_item_arg)
    end
  end
  
  barrel_sprite.hard = true
  -- sprite ain't hard no more!  Let's redraw the hard map in one area
  barrel_sprite:draw_hard()
  dink.kill_this_task()
end

-- Used from ch1-gh.lua
function open_chest(chest_sprite, make_item, make_item_arg)
  -- play noise
  
  local hold = chest_sprite.editor_sprite
  if hold ~= nil then
    -- this was placed by the editor, lets make the chest stay open
    hold.type = editor_type.DRAW_WITH_HARDNESS
    hold.seq = 175
    hold.frame = 4
  end
  
  if make_item ~= nil then
    --[[
      At this point, remember (or learn) that table.entry is really just
      syntactic sugar for table["entry"], which makes this equivalent to
      calling the function named whatever make_item is set to in the
      _make.lua file, e.g. make.sheart(x, y).
    --]]
    if make_item_arg == nil then
      make[make_item](chest_sprite.x, chest_sprite.y)
    else
      make[make_item](chest_sprite.x, chest_sprite.y, make_item_arg)
    end
  end
  
  chest_sprite.seq = 175
  chest_sprite.script = ""
  chest_sprite.notouch = true
  chest_sprite.nohit = true
  chest_sprite.script = ""
  
  dink.kill_this_task()
end

