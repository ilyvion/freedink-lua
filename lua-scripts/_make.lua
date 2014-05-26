--[[
Make code. Contains code for making items.

Use like this, e.g.:

local make = include("_make")
make.mgold(x, y)
--]]

local function create(x, y, seq, frame, script)
  local crap = dink.create_sprite(x, y, brain.REPEAT, seq, frame)
  crap.script = script
end

function sheart(x, y)
  create(x, y + 1, 54, 1, "sheart")
end

function mgold(x, y)
  create(x, y + 1, 178, 4, "mgold")
end

function lgold(x, y)
  create(x, y + 1, 178, 4, "lgold")
end

function mpotion(x, y)
  create(x, y + 1, 178, 4, "ppotion")
end

function rpotion(x, y)
  create(x, y + 1, 178, 4, "rpotion")
end

function spotion(x, y)
  create(x, y + 1, 178, 4, "rpotion")
end

function dpotion(x, y)
  create(x, y + 1, 178, 4, "bpotion")
end

function apotion(x, y)
  create(x, y, 178, 4, "apotion")
end

function gold200(x, y)
  create(x, y, 178, 4, "gold200")
end

function gold500(x, y)
  create(x, y, 178, 4, "gold500")
end

function sfb(x, y)
  create(x, y, 422, 7, "get-sfb")
end

function lheart(x, y)
  create(x, y, 54, 1, "sheart")
end

function gheart(x, y)
  create(x, y, 53, 1, "gheart")
end

function heart(x, y)
  create(x, y, 52, 1, "heart")
end

function food(x, y, t)
  create(x, y, 421, t, "sfood")
end

function foodduck(x, y)
  local rrand = dink.random(2, 1)
  
  local crap
  if rrand == 1 then
    crap = dink.create_sprite(x, y, brain.REPEAT, 421, 5)
  else
    crap = dink.create_sprite(x, y, brain.REPEAT, 421, 6)
  end
  
  --crap.script = "sfood"
end

