-- main.lua run when dink is started

function main()
  --[[
    let's init all our globals
    These globals are REQUIRED by dink.exe (it directly uses them, removing
    any of these could result in a game crash.
  ]]
  
  global.create("exp", 0)
  global.create("strength", 3)
  global.create("defense", 0)
  global.create("cur_weapon", 0)
  global.create("cur_magic", 0)
  global.create("gold", 0)
  global.create("magic", 0)
  global.create("magic_level", 0)
  global.create("vision", 0)
  global.create("result", 0)
  global.create("speed", 1)
  global.create("timing", 0)
  global.create("lifemax", 10)
  global.create("life", 10)
  global.create("level", 1)
  global.create("player_map", 1)
  global.create("last_text", 0)
  global.create("update_status", 0)
  global.create("missile_target", 0)
  global.create("enemy_sprite", 0)
  global.create("magic_cost", 0)
  global.create("missle_source", 0)
  
  -- These globals are stuff we added, which will all be saved with the player
  -- file

  global.create("story", 0)
  global.create("old_womans_duck", 0)
  global.create("nuttree", 0)
  global.create("letter", 0)
  global.create("little_girl", 0)
  global.create("farmer_quest", 0)
  global.create("save_x", 0)
  global.create("save_y", 0)
  global.create("safe", 0)
  global.create("pig_story", 0)
  global.create("wizard_see", 0)
  global.create("mlibby", 0)
  global.create("wizard_again", 0)
  global.create("snowc", 0)
  global.create("duckgame", 0)

  global.create("gossip", 0)
  global.create("robbed", 0)
  global.create("dinklogo", 0)
  global.create("rock_placement", 0)
  global.create("temphold", 0)
  global.create("temp1hold", 0)
  global.create("temp2hold", 0)
  global.create("temp3hold", 0)
  global.create("temp4hold", 0)
  global.create("temp5hold", 0)
  global.create("temp6hold", 0)
  global.create("town1", 0)
  global.create("s2-milder", 0)
  global.create("thief", 0)
  global.create("caveguy", 0)
  global.create("s2-aunt", 0)
  global.create("tombob", 0)
  global.create("mayor", 0)
  global.create("hero", 0)
  global.create("s2-nad", 0)
  global.create("gobpass", 0)
  global.create("bowlore", 0)
  global.create("s4-duck", 0)
  global.create("s5-jop", 0)
  global.create("s7-boat", 0)
  global.create("s2-map", 0)
  
  -- Autoincludes are scripts that are included in every other Lua script
  autoinclude("cutscene", "_cutscene")

  -- crap needed for misc
  player:set_speed(3)
  player.frame_delay = 0 -- This command is ignored (as it also is in DinkC) because
                         -- sprite 1 isn't active yet when the main script is run.
                         -- Leaving it in just for completeness.

  -- Let's preload that sprite that's been bugging the hell out of Pap
  dink.preload_seq(373)

  -- if run with -debug option this will be written to debug.txt.
  dink.debug("Dink started. Time to fight for your right to party.")
  --playmidi("story.mid")
  dink.kill_this_task()
end

