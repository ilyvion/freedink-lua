function main()
  --This is the rest of the robbery
  local bad1 = dink.get_editor_sprite(4).sprite
  local bad2 = dink.get_editor_sprite(3).sprite
  local guy = dink.get_editor_sprite(5).sprite
  
  bad1.strength = 3
  bad2.strength = 2
  bad1.distance = 50
  bad2.distance = 50
  bad1.exp = 30
  bad2.exp = 35
  
  --Ok, now start it!
  local cs = cutscene.create_cutscene(1000)
  cs:add_participant("d", player)
  cs:add_participant("g", guy, "3")
  cs:add_participant("b1", bad1, "4")
  cs:add_participant("b2", bad2, "4")
  
  cs:scene({
    {"fza"},
    {"w", 0},
    {"pmidi", "battle.mid"},
    {"w", 200},
    {"ss", "g", "Please, I've no money, I'm just traveling light."},
    {"ss", "b2", "We'll be the judge of that, where are you headed?"},
    {"mve", "g", 455},
    {"ss", "g", "I'm just passing through to Windemere sir."},
    {"ss", "d", "Those are Royal Guards, what are they doing out here ..."},
    {"ss", "b1", "Hah, you still owe us 5 gold pieces for passing through the land."},
    {"mvw", "g", 400},
    {"ss", "g", "I don't have that much sir."},
    {"say", "b1", "Ha ha ha ha"},
    {"ss", "b2", "Ha ha ha ha"},
    {"say", "g", "Someone, help!"},
    {"fn", "g", function()
      bad1.target = guy
      bad2.target = guy
    end},
    {"ufza"}
  })
  
end

function die()
  global.robbed = 1
  global.safe = 1
  
  current_sprite:freeze()
  current_sprite:say("`1Ahhhhhhh")
  dink.wait(400)
  current_sprite.active = false
end

function attack()
  dink.playsound(36, 22050, 0, current_sprite, false)
  current_sprite.attack_wait = dink.random(4000, 0)
end

