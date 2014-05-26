--[[
Make code. Contains code for making items given after a monster dies.

Use like this, e.g.:

local emake = include("_emake")
make.small(x, y)
--]]

local make = include("_make")

local function give_food()
  local mcrap = dink.random(3, 1)
  if mcrap == 1 then
    -- give them food type 1
    make.food(x, y, 1)
  elseif mcrap == 2 then
    -- give them food type 2
    make.food(x, y, 2)
  elseif mcrap == 3 then
    -- give them food type 3
    make.food(x, y, 7)
  elseif mcrap == 4 then
    -- give them food type 4
    make.food(x, y, 8)
  end
end

function small(x, y)
  if dink.random(3, 1) == 1 then
    -- let's give 'em a small heart
    make.sheart(x, y)
  end
end

function medium(x, y)
  local mcrap = dink.random(20, 1)
  
  if mcrap == 1 then
    -- let's give 'em a small heart
    make.sheart(x, y)
  elseif mcrap == 2 then
    -- let's give 'em a large heart
    make.heart(x, y)
  elseif mcrap == 3 then
    give_food()
  else
    -- lets give 'em a random amount of gold
    make.mgold(x, y)
  end
end

function large(x, y)
  local mcrap = dink.random(20, 1)
  
  if mcrap == 2 then
    -- let's give 'em a large heart
    make.heart(x, y)
  elseif mcrap == 3 then
    give_food()
  else
    -- lets give 'em a random amount of gold
    make.lgold(x, y)
  end
end

function xlarge(x, y)
  local mcrap = dink.random(20, 1)
  
  if mcrap == 2 then
    -- let's give 'em a large heart
    make.heart(x, y)
  elseif mcrap == 3 then
    give_food()
  else
    -- lets give 'em a random amount of gold
    make.gold200(x, y)
  end
end

