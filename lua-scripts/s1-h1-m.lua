local function prepare_cutscene(wait_time)
  local cs = cutscene.create_cutscene(wait_time)
  cs:add_participant("m", current_sprite, "#")
  cs:add_participant("d", player)
  return cs
end

function main()
  local whob = dink.get_editor_sprite(26).sprite
  current_sprite.hitpoints = 0
  
  if global.story > 2 then
    whob.active = false
    return
  end
  
  local cs = prepare_cutscene(0)
  
  if global.old_womans_duck > 2 then
    if global.pig_story == 0 then
      cs:scene({
        {"mss", 160},
        {"w", 200},
        {"fz", "m"},
        {"ss", "m", "Dink, it's getting late!  No food until you feed the pigs!"},
        {"ufz", "m"}
      })
      return
    end
    global.story = 2
    
    cs:scene({
      {"fza"},
      {"msn", "d", 370},
      {"w", 1000},
      {"mss", "m", 200},
      {"w", 500},
      {"ass", {"m", "d"}, {
        "Dink, can you do something for me?",
        "Yes, what is it?",
        {"Can you go out to the woods and see if you can get,", "some AlkTree nuts, I think they're in season."},
        "No problem, I'll be right back.",
        "You're a dear."
      }},
      {"ufza"}
    })
    return
  end
  
  if global.story == 0 then
    cs:scene({
      {"pmidi", "dance.mid"},
      {"w", 1000},
      {"fza"},
      {"w", 200},
      {"ss", "m", "Dink, would you go feed the pigs?"},
      {"ss", "d", "What, now?"},
      {"w", 0},
      {"ss", "m", "YES, NOW."},
      {"ufza"}
    })
    dink.playsound(22, 22050, 0, nil, false)
    global.update_status = 1
    dink.draw_status()
    global.story = 1
    return
  end
end
  
local choice_menu = dink.create_choice_menu()
local choice_pig_feeding  = choice_menu:add_choice("Ask about pig feeding")
local choice_father       = choice_menu:add_choice("Ask about your father")
local choice_village_info = choice_menu:add_choice("Get info about the village")
local choice_angry        = choice_menu:add_choice("Get angry for no reason")
local choice_fed_pigs     = choice_menu:add_choice("Tell her you fed the pigs", global.pig_story == 1)
local choice_leave        = choice_menu:add_choice("Leave")

function talk()
  player:freeze()
  current_sprite:freeze()
  
  local choice_result = choice_menu:show()
  
  local lines
  if choice_result == choice_pig_feeding then
    lines = {
      "Mother, how do I feed the pigs?  I forgot!",
      {"That's very amusing, Dink.  You get the sack of feed, and you", "sprinkle it in the pig pen.  And don't tease them!"}
    }
  elseif choice_result == choice_father then
    lines = {
      "What kind of a man was father?",
      "He was a peasant like us.",
      "Was he good with the sword?",
      "Of course not.  He was a wonderful farmer and husband."
    }
  elseif choice_result == choice_village_info then
    lines = {
      "Tell me about this village.",
      "The villagers are very friendly.  Oh, Ethel wants to see you.",
      "Ethel?  She's old, isn't she?",
      "Yes Dink, she is. <laugh>"
    }
  elseif choice_result == choice_angry then
    lines = {
      "I HATE YOU!",
      "You'll get over it."
    }
  elseif choice_result == choice_fed_pigs then
    lines = {
      "I'm finished with my chores, mother.",
      "Good boy.  Go visit our neighbors while I prepare dinner."
    }
  end
  
  if choice_result ~= choice_leave then
    dink.wait(200)
    local cs = prepare_cutscene(200)
    cs:scene({
      {"ass", {"d", "m"}, lines}
    })
  end
  
  player:unfreeze()
  current_sprite:unfreeze()
end

function hit()
  current_sprite:say_stop("`#Ouch!  Stop it!")
end

