-- init.lua

-- Sprite metatable: Handles sprite property assignment/reading
sprite_metatable =
{
  __newindex = function(t, key, value)
    local ok, err = pcall(wrapper.set_sprite_value, t.sprite_number, key, value)
    
    -- error level 2 means that the error will be signaled on 2nd stack frame,
    -- that is the line at which the key has been set, rather than from here
    if not ok then error(err, 2) end
  end,
  __index = function(t, key)
    local ok, value = pcall(wrapper.get_sprite_value, t.sprite_number, key)
    if not ok then error(value, 2) end
    return value
  end
}

-- Called with the desired sprite number, this function returns a
-- lua table with special metatable values that effectively creates
-- "properties" on the sprite objects, in such a way that you can do
-- things like:
--
-- dink = get_sprite(1)
-- dink.strength = 10
function dinkc.get_sprite(sprite_number)
  return setmetatable({sprite_number = sprite_number}, sprite_metatable)
end

-- Sound metatable: Handles soundbank property assignment/reading
soundbank_metatable =
{
  __newindex = function(t, key, value)
    local ok, err = pcall(wrapper.set_sound_value, t.sound_number, key, value)
    if not ok then error(err, 2) end
  end,
  __index = function(t, key)
    local ok, value = pcall(wrapper.get_sound_value, t.sound_number, key)
    if not ok then error(value, 2) end
    return value
  end
}

-- Global metatable: Handles returning/setting global DinkC variables
global_metatable = {
  __newindex = function(t, key, value)
    local ok, err = pcall(wrapper.set_global_dinkc_value, key, value)
    if not ok then error(err, 2) end
  end,
  __index = function(t, key)
    local ok, value = pcall(wrapper.get_global_dinkc_value, key)
    if not ok then error(value, 2) end
    return value
  end
}

function get_sound(sound_number)
  return setmetatable({sound_number = sound_number}, soundbank_metatable)
end

-- Thin wrappers around DinkC functions that return sprite numbers
-- so that they instead return the lua sprite object
function dinkc.create_sprite(x, y, brain, sequence, frame)
  sprite_number = wrapper.create_sprite_dc(x, y, brain, sequence, frame)
  return dinkc.get_sprite(sprite_number)
end

function dinkc.say_xy(string, x, y)
  sprite_number = wrapper.say_xy_dc(string, x, y)
  return dinkc.get_sprite(sprite_number)
end

-- Thin wrappers around functions that should yield
function dinkc.wait(amount)
  wrapper.wait_dc(amount)
  coroutine.yield()
end

function dinkc.kill_game(amount)
  wrapper.kill_game_dc()
  coroutine.yield()
end

-- Other wrappers
function dinkc.playsound(sound_number, min_speed, rand_speed_to_add, sprite, repeat_p)
  local sprite_number
  if sprite == nil then
    sprite_number = 0
  else
    sprite_number = sprite.sprite_number
  end
  
  soundbank = wrapper.playsound_dc(sound_number, min_speed, rand_speed_to_add, sprite_number, repeat_p)
  
  return get_sound(soundbank)
end

-- refinfo cache (currently unused) May be removed when the Dink
-- script modularization is implemented.
refinfo = {}

-- Script cache
script = {}

-- Co-routine cache. Holds a script's co-routines between resumes,
-- to properly yield when Dink expects a script to yield, e.g.
-- when calling the wait() function or the *_stop() functions.
routine = {}

function create_environment(sprite_number)
  -- Creates the environment we run the each Dink lua scripts in.
  -- 'dinkc' is a library to access the DinkC functions,
  -- populated mainly in the lua_dinkc.c file.
  --
  -- Each script runs in its own environment, so that they can
  -- have the same function names and variables without interfering
  -- with each other. If scripts need to communicate, facilities
  -- are (or will be) provided for that purpose.
  dinkc_env =
  {
    -- Rather than allowing scripters to access the dinkc table directly, we give them an empty table and
    -- read-only access through a meta-table.
    dinkc = setmetatable({}, {__index = dinkc, __newindex = function() error("Cannot add or change members in dinkc", 2) end}),
    
    -- Global allow access to the global DinkC variables through globals.<variable_name>.
    global = setmetatable({}, global_metatable),
    
    -- Current sprite is a convenience variable given to each script; should be equivalent to &current_sprite in DinkC
    current_sprite = dinkc.get_sprite(sprite_number),
    
    -- Brain value enumerable, allows doing things like
    --
    -- sprite.brain = brain.DUCK
    brain = setmetatable({}, {__index = {
      NONE = 0,
      PLAYER = 1,
      DUMB_SPRITE_BOUNCER = 2,
      DUCK = 3,
      PIG = 4,
      KILL_SEQ_DONE_LEAVE_LAST_FRAME = 5,
      REPEAT = 6,
      KILL_SEQ_DONE = 7,
      TEXT_SPRITE = 8,
      MONSTER_DIAGONAL = 9,
      MONSTER_NONDIAGONAL = 10,
      MISSILE = 11,
      BRAIN_PARM_SIZE_MATCH = 12,
      MOUSE = 13,
      BUTTON = 14,
      SHADOW = 15,
      SMART_PEOPLE = 16,
      MISSILE_KILL_SEQ_DONE = 17
    }, __newindex = function() error("Cannot add or change members in brain", 2) end}),
    
    -- Direction enumerable, allows doing things like
    --
    -- sprite.dir = direction.NORTH
    direction = setmetatable({}, {__index = {
      SOUTH_WEST = 1,
      SOUTH = 2,
      SOUTH_EAST = 3,
      WEST = 4,
      EAST = 6,
      NORTH_WEST = 7,
      NORTH = 8,
      NORTH_EAST = 9
    }, __newindex = function() error("Cannot add or change members in direction", 2) end})
  }
  
  return dinkc_env
end

function load_script(name, sprite_number, rinfo, code)
  script[name] = create_environment(sprite_number)
  refinfo[name] = rinfo
  local chunk, message = load(code, name..'.l', 't', script[name])
  if not chunk then return nil, message end
  return pcall(chunk)
end

function script_procedure_exists(name, fname)
  return script[name] ~= nil and script[name][fname] ~= nil
end

function run_script(name, fname)
  if routine[name] ~= nil and coroutine.status(routine[name]) == "dead" then
    -- Kill a dead co-routine when trying to start the script again.
    -- If a co-routine isn't dead, then it is assumed to be a script
    -- resumption, not a script start.
    routine[name] = nil
  end
  
  if routine[name] == nil then
    if not script_procedure_exists(name, fname) then
      return false, "run_script cannot run function "..fname.." in "..name..". No such function exists."
    end
    routine[name] = coroutine.create(script[name][fname])
  end
  return coroutine.resume(routine[name])
end

function get_refinfo(name)
  return refinfo[name]
end

-- The function kill_script is implemented entirely in C. See the
-- lua_dink.c file.

