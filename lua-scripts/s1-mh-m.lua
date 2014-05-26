function main()
  if global.wizard_see == 2 then
    current_sprite:say("`0How goes the hunt Dink?")
    return
  elseif global.wizard_see == 3 then
    current_sprite.active = false
    player:say("He's gone!!")
    return
  elseif global.wizard_see == 4 then
    curent_sprite.active = false
    return
  end
  current_sprite:say("`0Ahh Dink, I've been expecting you.")
end
  
local choice_menu = dink.create_choice_menu()
local choice_worth     = choice_menu:add_choice("Tell him you're worthy", function() return global.wizard_see == 1 end)
local choice_magic     = choice_menu:add_choice("Ask about Magic")
local choice_nevermind = choice_menu:add_choice("Nevermind")

function talk()
  player:freeze()
  current_sprite:freeze()

  local cs = cutscene.create_cutscene(200)
  cs:add_participant("d", player)
  cs:add_participant("w", current_sprite, "0")
  
  if global.wizard_see == 0 then
    cs:command("ass", {"d", "w"}, {
      "Hi, it's me again.  Now who are you really?",
      "I am the Wizard Martridge.  A teacher of magic.",
      "Wow, I don't think I've ever seen your place around here before.",
      {
        "I like being closer to nature.",
        "I've been watching you for a while Dink."
      },
      "Like, peeking in through my windows and such?",
      {
        "No Dink, I have magic.",
        "You too may have the power, an ability for the supernatural."
      },
      {
        "Really?  You think so?  Cool, what can I do?",
        "Can I like create chicks right in front of me and stuff?"
      },
      "Uh, no.",
      "Well can I like float around and fly up into trees?",
      "Uh maybe, it depends...",
      "Can I throw death from my hands??",
      {
        "Maybe I was wrong Dink.",
        "But these things can be hard to tell."
      }
    })
    global.wizard_see = 1
  end
  
  local choice_result = choice_menu:show()
  dink.wait(200)
  
  if choice_result == choice_worth then
    cs:command("ass", {"d", "w"}, {
      {
        "Martridge, I .. I can handle it.  I'm prepared.",
        "I promise I'll be honorable and learn to use it right."
      },
      "Well Dink, you must prove yourself.",
      {
        "Well in a cave on these hills, there lies a beast.",
        "It's called a Bonca.  Slay it and return."
      },
      "I'll do it, I can fight the beast.",
      "Excellent, excellent.",
      "Where does it dwell?",
      "In a cave to the west Dink.",
      "I won't fail you.",
      "Here Dink, take this.  This red potion will strengthen you."
    })
    
    global.wizard_see = 2
    local boom = dink.create_sprite(187, 157, brain.KILL_SEQ_DONE, 167, 1)
    boom.seq = 167
    local bottle = dink.create_sprite(187, 157, brain.NONE, 56, 1)
    dink.playsound(24, 22052, 0, nil, false)
    bottle.script = "rpotion"
  elseif choice_result == choice_magic then
    cs:command("ass", {"d", "w"}, {
      "Martridge, tell me about magic.",
      {
        "Well magic is an ancient art.  Those who've known it",
        "have been great leaders, entertainers, and warriors.",
        "It's a great power and gift to those who can use it."
      }
    })
  end
  player:unfreeze()
  current_sprite:unfreeze()
end

function hit()
  current_sprite:say("`0Why are you trying to hurt me Dink?")
  -- Warp the Wizard elsewhere
end

