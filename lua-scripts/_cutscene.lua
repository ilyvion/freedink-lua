--[[
Code for making cutscene scripting a lot less tedious. See s1-wiz.lua for
example usage.


You create a cutscene player by importing the _cutscene.lua script into your
script, and invoking its create_cutscene():

  local cutscene = include("_cutscene")
  local cs = cutscene.create_cutscene()

Alternatively, you can auto-include the script into every script, like so
(typically done in main.lua):

  autoinclude("cutscene", "_cutscene")

This will have the same effect as doing the first of the lines above in every
script in your dmod.

The create_cutscene() function takes two optional parameters, the first one
being the default wait time, and the second one being the default
"ignore hardness" value. If you don't specify these, they default to 0 and
true. Whenever a scene() finishes, these values are reset to these default
values.

Optionally, you can add your cutscene's participants. These are expected to
be sprite objects, either derived from create_sprite(), get_sprite() or be one
of the pre-defined sprite objects, such as player and current_sprite. You give
the participants a short "key", which you can use to refer to each individual
sprite in the cutscene. In this instance, I will add the player sprite and
another sprite that, let's say, was created using create_sprite(). As an
optional parameter, you can set the sprite's default talk color as a third
parameter. Whenever a scene() finishes, the sprite's current talk color will
be reset to its default talk color. I will set my other sprite's default talk
color to "6" in the example here (This default color mechanism is more
thoroughly explained for the "ac" command below):

  cs:add_participant("p", player)
  cs:add_participant("o", other, "6")

You run cutscenes with the scene() method. You can run as many of these as
you want on a single cutscene object, in cases where you want to run a scene,
then do some non-cutscene scripting, and then continue running the scene, that
is entirely possible to do.

A cutscene is described to the cutscene player as a table (which in Lua you
create with {}), consisting of command tables, where each command table
is made up of a command name and the command's parameters.

In the commands listed below, where the command takes an "actor", you can
either specify the shorthands you added with add_participant(), or you can
give it a sprite object directly.

The commands supported out of the box are:
  - {"w", time}, where time is a positive integer, or zero.
    This command sets the wait time between each cut scene command, except
    for itself and a few other commands. After each command is run, the
    cutscene player makes a call to dink.wait(time), except when time is
    zero, in which case there is no wait time.
  
  - {"ih", value}, where value is either true or false.
    This command modifies how the move_stop command is executed. This value
    basically becomes the value given to move_stop's ignore hardness
    parameter. Since this value is typically often the same for a whole
    cutscene, it was given its own setting rather than having to be given
    to each individual move_stop command.
  
  - {"ac", actor, actor_color}, where actor_color is the prefix color given
    to the relevant actor. If you want the actor to always speak with a
    specific color, you can specify it here. The actor_color should be
    specified as the symbol that follows the ` mark in a conventional say
    commands, so for instance, if you'd always want your actor to talk as
    though you were typing "`6Hello!", then set actor_color to "6" for that
    actor, and you can type just "Hello!" in the cutscene say commands.
    Not only does this save you from having to remember which actor uses
    which color, but if you ever decide that it's the wrong color for the
    actor, you only have to change it in one location instead of having to
    change it in every single say_stop command. You can use this as many times
    as needed in your script, so if your actor goes from being nice to evil,
    you could signal that with a color change in the middle of your scene,
    using this command. Whenever a scene() finishes, the sprite's current talk
    color will be reset to its default talk color, as specified in the
    add_participant() method.
  
  - {"ss", actor, text}. This is the say_stop command shorthand. This command
    makes use of the relevant actor_color specified by the "ac" command,
    unless the text given starts with `, in which case that color overrides
    the actor color. The text parameter can also be a table of strings, in
    which case it behaves as though you had done as many "ss" commands as there
    are strings in the table. This is useful if a single character is saying
    a lot of things in order without anything else happening in-between. For
    the non-stop version of say, substitute "say" for "ss".
  
  - {"ass", actors, lines}, where actors and lines are tables, containing the
    actors and the lines they say. This command is short for "alternating
    say_stop", and does exactly what it says on the tin. You specify
    however many actors are involved, and each line in the lines table is
    alternatingly said by the first, second, third, ..., nth actor, until it
    starts again at the first, and around we go. Mostly useful for situations
    where you have a lot of back and forth dialogue. Like the "ss" command,
    the lines table can, in addition to having strings, also have tables of
    strings, which causes the actor whose turn it is to say all the strings
    in that table before the next actor gets to talk again.
  
  - {"msX", destination}. This is the move_stop command shorthand. This is a
    prefix command, meaning that the X in the command must be substituted by
    the desired suffix to work as desired. The supported suffixes for this
    command are "sw", "s", "se", "w", "e", "nw", "n" and "ne", which result
    in move commands heading in the south-west, south, south-east, west, east,
    north-west, north and north-east directions, respectively. For the
    non-stop version of move, substitute "mvX" for "msX".
  
  - {"fdu"} and {"fdd"}. fade_up() and fade_down().
  
  - {"fz", actor} and {"ufz", actor}. actor:freeze() and actor:unfreeze().
  
  - {"fza"} and {"ufza"}. Does freeze() or unfreeze() on all registered
    participants.
  
  - {"pmidi", midi} and {"smidi"}. playmidi(midi) and stopmidi().
  
  - {"fn", function}, where function is a Lua function pointer or anonymous
    function. This lets you run pretty much any Lua code you want anywhere
    you want inside a cutscene. Useful for situations where you want to do
    some one-shot code that doesn't really require its own command. For more
    advanced custom functionality, read about the add_command() method below.

While those are all the commands that are supported out of the box, you can
optionally add extra functionality to the cutscene player, by introducing
custom commands. If there's something that you will be doing repeatedly in
your cutscene, then using this will be helpful. There are four kinds of custom
commands that can be added, and they work like this:

  - cs:add_command("name", func). This is the simplest, it adds a command
    named "name", and using that command in your cutscene will call the
    function you provide. Any parameters given to the name command,
    e.g. {"name", 1, 2, 3} will be passed on to your function, as
    func(1, 2, 3).
  
  - cs:add_actor_command("name", func). Similar to the ordinary command,
    except your command now has the same functionality as those from the
    built-in set that take an actor. Your command is now expected to specify
    an actor as its first parameter. Other than that, it works as before, so
    e.g. {"name", "o", 1, 2, 3} will be passed to your function as
    func(actor_sprite, 1, 2, 3). You can treat this actor_sprite as you would
    treat current_sprite or player; it's a sprite object.
  
  - cs:add_prefix_command("prefix", func). This adds a command that works
    like the built-in "ms" prefix command. If you specify prefix as "jump",
    e.g., then any command that starts with "jump" will call your function,
    e.g. {"jumpfoo", 1, 2, 3} will call your function with the suffix, plus
    any parameters as func("foo", 1, 2, 3).
  
  - cs:add_prefix_actor_command("prefix", func). This command combines the
    prefix command and the actor command. Your func will receive them in this
    order: func("suffix", actor_sprite, ...)

Again, as mentioned at the very top, if you want a more complete example, you
can see s1-wiz.lua, as well as any other cutscene scripts that exist in the
original Dink Smallwood dmod that has been converted to Lua.

An alternate, slightly less verbose method of creating a cutscene is available
if you have a cutscene with only one scene without any extra commands to add.
Doing this:

  cutscene.quick_cutscene({p = player, o = other}, {
    {"w", 250},
    {"ac", "o", "9"},
    {"mse", "p", 120},
    {"ss", "p", "Hello!"},
    {"msw, "o", 200},
    {"ss", "o", "Oh, hi there!"}
  })

Is the same as doing this:

  local cs = cutscene.create_cutscene()
  cs:add_participant("p", player)
  cs:add_participant("o", other)
  cs:scene({
    {"w", 250},
    {"ac", "o", "9"},
    {"mse", "p", 120},
    {"ss", "p", "Hello!"},
    {"msw, "o", 200},
    {"ss", "o", "Oh, hi there!"}
  })

Finally, behind the scenes, all these tables are actually unpacked as method
calls made to a method called command() on the cutscene object. Thus, if you
want to run a single command from the cutscene player, you may call that
method directly, like this:

  cs:command("ss", "p", "Hello!")

Notice that a command executed in this fashion looks exactly like its scene()
variant, except instead of being entries in a table, they are parameters to
a method.

--]]

function quick_cutscene(participants, scene)
  local cs = create_cutscene()
  for name, sprite in pairs(participants) do
    cs:add_participant(name, sprite)
  end
  cs:scene(scene)
end

function create_cutscene(default_wait, default_ih)
  if type(default_wait) ~= "number" or default_wait < 0 then
    default_wait = 0
  end
  if default_ih == nil then
    default_ih = true
  end
  
  return {
    -- The time to wait between commands.
    default_wait_time = default_wait,
    wait_time = default_wait,
    wait = function(self)
      if self.wait_time > 0 then
        dink.wait(self.wait_time)
      end
    end,
    
    -- Whether move commands ignore hardness.
    default_ignore_hardness = default_ih,
    ignore_hardness = default_ih,
    
    -- Participation table
    participants = {},
    add_participant = function(self, key, sprite, color)
      self.participants[key] = {sprite = sprite, color = color, default_color = color}
    end,
    
    -- Custom command table
    custom_commands = {},
    add_command = function(self, key, func)
      self.custom_commands[key] = {"n", func}
    end,
    add_actor_command = function(self, key, func)
      self.custom_commands[key] = {"a", func}
    end,
    add_prefix_command = function(self, key, func)
      self.custom_commands[key] = {"p", func}
    end,
    add_prefix_actor_command = function(self, key, func)
      self.custom_commands[key] = {"pa", func}
    end,
    
    -- The scene function
    scene = function(self, commands)
      for _, c in pairs(commands) do
        self:command(table.unpack(c))
      end
      -- Restore defaults (creation-time values)
      self.wait_time = self.default_wait_time
      self.ignore_hardness = self.default_ignore_hardness
      for _, actor in pairs(self.participants) do
        actor.color = actor.default_color
      end
    end,
    
    -- The command function
    command = function(self, command, ...)
      local arg = {...}
      if command == "w" then
        self.wait_time = arg[1]
      elseif command == "ih" then
        self.ignore_hardness = arg[1]
      elseif command == "pmidi" then
        dink.playmidi(arg[1])
        self:wait()
      elseif command == "smidi" then
        dink.stopmidi()
        self:wait()
      elseif command == "fdu" then
        dink.fade_up()
        self:wait()
      elseif command == "fdd" then
        dink.fade_down()
        self:wait()
      elseif command == "fza" then
        for _, participant in pairs(self.participants) do
          participant.sprite:freeze()
        end
        self:wait()
      elseif command == "ufza" then
        for _, participant in pairs(self.participants) do
          participant.sprite:unfreeze()
        end
        self:wait()
      elseif command == "ass" then
        local actors = arg[1]
        local lines = arg[2]
        
        local actor_count = #actors
        local current_actor = 1
        for _, line in pairs(lines) do
          local actor = actors[current_actor]
          local color
          if type(actor) == "string" then
            color = self.participants[actor].color
            actor = self.participants[actor].sprite
          end
          
          if type(line) == "string" then
            if string.sub(line, 1, 1) == "`" or color == nil then
              actor:say_stop(line)
            else
              actor:say_stop("`"..color..line)
            end
          elseif type(line) == "table" then
            for _, txt in pairs(line) do
              if string.sub(txt, 1, 1) == "`" or color == nil then
                actor:say_stop(txt)
              else
                actor:say_stop("`"..color..txt)
              end
            end
          else
            error("The ass command only accepts string or table lines", 2)
          end
          
          current_actor = current_actor + 1
          if current_actor > actor_count then
            current_actor = 1
          end
          self:wait()
        end
      else
        local actor
        if type(arg[1]) == "string" then
          if self.participants[arg[1]] == nil then
            error("No participant with the name "..arg[1].." has been added.", 2)
          end
          actor = self.participants[arg[1]].sprite
        else
          actor = arg[1]
        end
        
        if command == "ac" then
          if type(arg[1]) ~= "string" then
            error("The ac command can only be used on registered participants.", 2)
          end
          self.participants[arg[1]].color = arg[2]
        elseif command == "fz" then
          actor:freeze()
          self:wait()
        elseif command == "ufz" then
          actor:unfreeze()
          self:wait()
        elseif command == "fn" and type(arg[2]) == "function" then
          arg[2](actor)
          self:wait()
        elseif command == "ss" then
          local text = arg[2]
          local color
          if type(arg[1]) == "string" then
            color = self.participants[arg[1]].color
          end
          if type(text) == "string" then
            if string.sub(text, 1, 1) == "`" or color == nil then
              actor:say_stop(text)
            else
              actor:say_stop("`"..color..text)
            end
          elseif type(text) == "table" then
            for _, txt in pairs(text) do
              if string.sub(txt, 1, 1) == "`" or color == nil then
                actor:say_stop(txt)
              else
                actor:say_stop("`"..color..txt)
              end
            end
          else
            error("The ss command only accepts string or table", 2)
          end
          self:wait()
        elseif command == "say" then
          local text = arg[2]
          local color
          if type(arg[1]) == "string" then
            color = self.participants[arg[1]].color
          end
          if type(text) == "string" then
            if string.sub(text, 1, 1) == "`" or color == nil then
              actor:say(text)
            else
              actor:say("`"..color..text)
            end
          elseif type(text) == "table" then
            for _, txt in pairs(text) do
              if string.sub(txt, 1, 1) == "`" or color == nil then
                actor:say(txt)
              else
                actor:say("`"..color..txt)
              end
            end
          else
            error("The say command only accepts string or table", 2)
          end
        elseif string.sub(command, 1, 2) == "ms" or string.sub(command, 1, 2) == "mv" then
          local command_direction = string.sub(command, 3)
          local dir
          if command_direction == "sw" then
            dir = direction.SOUTH_WEST
          elseif command_direction == "s" then
            dir = direction.SOUTH
          elseif command_direction == "se" then
            dir = direction.SOUTH_EAST
          elseif command_direction == "w" then
            dir = direction.WEST
          elseif command_direction == "e" then
            dir = direction.EAST
          elseif command_direction == "nw" then
            dir = direction.NORTH_WEST
          elseif command_direction == "n" then
            dir = direction.NORTH
          elseif command_direction == "ne" then
            dir = direction.NORTH_EAST
          else
            error("Unknown move direction: "..command, 2)
          end
          if string.sub(command, 1, 2) == "ms" then
            actor:move_stop(dir, arg[2], self.ignore_hardness)
            self:wait()
          else
            actor:move(dir, arg[2], self.ignore_hardness)
          end
        else
          -- Let's go searching in the custom commands
          local cc_found = false
          for cn, cdf in pairs(self.custom_commands) do
            if string.sub(command, 1, string.len(cn)) == cn then
              local ftype, cf = table.unpack(cdf)
              
              if ftype == "n" then
                -- For the curious: This turns the command table into
                -- parameters for the custom command, leaving out the command
                -- name. Thus, if the command we received is {"abc", 1, 2, 3},
                -- for instance, then this ends up as being the same as
                -- running cf(1, 2, 3). (cf is the pointer to the custom function
                -- that was registered with the command using one of the add*command
                -- methods.)
                cf(table.unpack(arg))
              elseif ftype == "a" then
                local actor
                if type(arg[1]) == "string" then
                  actor = self.participants[arg[1]]
                else
                  actor = arg[1]
                end
                cf(actor, select(2, table.unpack(arg)))
              elseif ftype == "p" then
                local suffix = string.sub(command, string.len(cn) + 1)
                cf(suffix, table.unpack(arg))
              elseif ftype == "pa" then
                local suffix = string.sub(command, string.len(cn) + 1)
                local actor
                if type(arg[1]) == "string" then
                  actor = self.participants[arg[1]]
                else
                  actor = arg[1]
                end
                cf(suffix, actor, table.unpack(arg))
              else
                error("This should never happen under normal circumstances.")
              end
              cc_found = true
              break
            end
          end
          if not cc_found then
            error("Unknown scene command "..command, 3)
          else
            self:wait()
          end
        end
      end
    end
  }
end

