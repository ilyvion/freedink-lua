function main()
  if global.story == 4 then
    local cs = cutscene.create_cutscene(250)
    cs:add_participant("d", player)
  
    -- the old lady
    local person1 = dink.create_sprite(520, 300, 0, 231, 5)
    person1.speed = 1
    person1.base_walk = 230
    cs:add_participant("o", person1, "3")
    
    -- girl
    local person2 = dink.create_sprite(520, 120, 0, 221, 5)
    person2.speed = 1
    person2.base_walk = 220
    cs:add_participant("g1", person2, "4")
    
    if global.old_womans_duck ~= 3 and global.old_womans_duck ~= 5 then
      local duck1 = dink.create_sprite(470, 270, 0, 24, 2)
    end
    
    -- Silver knight?
    local person3 = dink.create_sprite(600, 310, 0, 411, 1)
    person3.speed = 1
    person3.base_walk = 410
    cs:add_participant("k", person3, "6")
    -- Girl2 at bottom
    local person4 = dink.create_sprite(545, 360, 0, 257, 2)
    person4.speed = 1
    person4.base_walk = 250
    cs:add_participant("g2", person4)
    
    global.vision = 1
    
    cs:scene({
      {"w", 0},
      {"fz", "d"},
      {"msw", "o", 500},
      {"mss", "g1", 210},
      {"msw", "k", 570},
      {"ss", "g1", "Dink!!!"},
      {"msw", "g2", 515},
      {"mse", "d", 437},
      {"w", 500},
      {"ss", "d", {"I .. I couldn't save her", "I was too late."}},
      {"w", 250},
      {"ss", "o", "It's not your fault Dink."}
    })
    
    if global.old_womans_duck == 3 then
      person1:say_stop("`3I hope my duck wasn't in there!")
    elseif global.old_womans_duck == 5 then
      cs:scene({
        {"ass", {"o", "d"}, {
          "But, if you hadn't killed my duck, your mom might still be alive.",
          "And just what is that supposed to mean, Ethel?",
          "Oh.. nothing..."
        }}
      })
    end
    
    cs:scene({
      {"ss", "g1", "There was nothing you could do.."},
      {"w", 750},
      {"ss", "k", "Don't blame yourself kid."},
      {"w", 250},
      {"fdd"},
      {"fn", nil, function() global.story = 5; dink.force_vision(2) end},
      {"fdu"},
      {"ufz", "d"}
    })
    
    -- force vision keep this task alive, now we need to kill it manually
    dink.kill_this_task()
    return
  elseif global.story > 4 then
    global.vision = 2
  elseif global.story == 3 then
    global.vision = 1
    cutscene.quick_cutscene({d = player}, {
      {"fz", "d"},
      {"msw", "d", 570},
      {"pmidi", "insper.mid"},
      {"w", 500},
      {"ss", "d", {"What, the house, mother nooooo!!!", "She's still in there!!"}},
      {"w", 0},
      {"ufz", "d"}
    })
    dink.kill_this_task()
  end
end

