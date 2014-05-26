function main()
  local cs = cutscene.create_cutscene(250)
  cs:add_participant("l", current_sprite, "2")
  cs:add_participant("d", player)
  
  --this script needs to survive a screen load, lets unattach it from the
  --letter
  dink.script_attach(1000)
  --1000 means can't be killed unless we do it
  
  player:freeze()
  global.letter = 2
  global.story = 6
  dink.wait(2000)
  
  cs:scene({
    {"ss", "l", "Dear Dink,"},
    {"w", 0},
    {"ss", "l", {
      "We've just gotten word of the tragic accident that happened at...",
      "your home a short while ago.  Needless to say we are shocked.",
      "This must be a hard time for you, being so young and suffering...",
      "such a great loss.  You are completely welcome to come and stay...",
      "with us in Terris for a while, I don't think Jack will mind."
    }},
    {"w", 500},
    {"ss", "l", "Sincerely, Aunt Maria Kneedlewood"},
    {"ss", "d", {
      "Hmm... Terris.  I think that is west of here.",
      "Hey!  A map was enclosed!"
    }}
  })
  
  global["s2-map"] = 1
  dink.say_stop_xy("`%(Press M or button 6 for map toggle)", 20, 380)
  dink.fade_down()
  dink.fill_screen(0)
  --move Dink
  global.player_map = 439
  player.x = 362
  player.y = 303
  dink.load_screen()
  dink.draw_screen()
  dink.draw_status()
  dink.fade_up()
  player:unfreeze()
  
  dink.kill_this_task()
end

