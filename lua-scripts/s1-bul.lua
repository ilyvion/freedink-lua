function main()
  current_sprite.speed = 2
  current_sprite.timing = 0
  current_sprite.base_walk = 400

  dink.preload_seq(401)
  dink.preload_seq(403)
  dink.preload_seq(407)
  dink.preload_seq(409)
  
  if global.pig_story == 0 then
    local cs = cutscene.create_cutscene(200, true)
    cs:add_participant("d", player)
    cs:add_participant("m", current_sprite, "6")
    
    cs:scene({
      {"fz", "d"},
      {"ss", "d", "Here, piggie piggies."},
      {"mssw", "m", 542},
      {"pmidi", "bullythe.mid"},
      {"ss", "m", "Well, lookie what we got here."},
      {"ss", "d", "What do you want, Milder?"},
      {"ss", "m", "Nothing."},
      {"ss", "m", "'cept to watch you work... Is pig farming fun?"},
      {"ss", "d", "I'm NOT a pig farmer."},
      {"ss", "m", "Are you feeding pigs right now?"},
      {"ss", "d", "Er.."},
      {"ss", "d", "Um.."},
      {"w", 0},
      {"ss", "m", "Bawahahahah!  See you around, squirt."},
      {"msse", "m", 700},
      {"ss", "d", "I *HATE* that guy!"},
      {"ufz", "d"},
      {"smidi"}
    })
    global.pig_story = 1
  end
end

