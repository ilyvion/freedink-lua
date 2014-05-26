-- wizard cut scene

function main()
  player:freeze()
  dink.playmidi("wanderer.mid")
  current_sprite.speed = 1
  
  local cs = cutscene.create_cutscene()
  cs:add_participant("d", player)
  cs:add_participant("w", current_sprite, "0")
  
  -- Makes a fire-ring appear with a woosh-sound at the specified coordinates
  cs:add_command("whoosh", function(x, y)
    local mcrap = dink.create_sprite(x, y, brain.KILL_SEQ_DONE, 167, 1)
    mcrap.seq = 167
    dink.playsound(24, 22052, 0, nil, false)
  end)
  
  -- Makes the wizard look left and right, respectively
  cs:add_command("wll", function() current_sprite.pseq = 561 end)
  cs:add_command("wlr", function() current_sprite.pseq = 563 end)
  
  cs:scene({
    {"whoosh", 78, 319},
    {"w", 300},
    {"mse", "w", 120},
    {"ss", "d", "What the..."},
    {"w", 0},
    {"msn", "d", 377},
    {"mse", "d", 341},
    {"msn", "d", 319},
    {"mss", "d", 319},
    {"w", 300},
    {"msw", "d", 280},
    {"w", 200},
    {"ss", "d", "Who are you?"},
    {"wll"},
    {"wlr"},
    {"ss", "w", "I am a great magician."},
    {"ss", "d", "No way! You're so cute and tiny!"},
    {"mse", "w", 161},
    {"ss", "w", "I am nothing of the sort!"},
    {"mse", "w", 192},
    {"ss", "w", "You cannot measure magic by size!"},
    {"ss", "d", "I just have to pet you!"},
    {"msw", "d", 145},
    {"ss", "d", "Huh?  I just walked right through you."},
    {"w", 50},
    {"mse", "d", 304},
    {"w", 200},
    {"msw", "d", 289},
    {"ss", "d", "You are not really here, are you!"},
    {"ss", "w", "Of course I am.. just not physically."},
    {"wll"},
    {"wlr"},
    {"ass", {"w", "d"}, {
      "If you would like to learn more.. Come to my hidden cabin.",
      "How am I supposed to find it if it's hidden?",
      "Good point.  It lies behind some trees north-east of here.",
      "Ok, I may drop by later.. are you in a circus?",
      "DON'T ANGER ME, HUMAN!"
    }},
    {"w", 0},
    {"fn", "w", function(w) w.speed = 10; w.timing = 0 end},
    {"ufz", "d"},
    {"msw", "w", 71},
    {"msn", "w", 213},
    {"mse", "w", 323},
    {"mss", "w", 295},
    {"msw", "w", 145},
    {"msn", "w", 217},
    {"mse", "w", 316},
    {"mss", "w", 336},
    {"mse", "w", 545},
    {"msn", "w", 241},
    {"msw", "w", 359},
    {"whoosh", 359, 241},
  })
  current_sprite.active = false
  return
end

