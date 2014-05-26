function talk()
  if global.wizard_see > 3 then return end
  
  player:freeze()
  local cs = cutscene.create_cutscene(200)
  
  local function whoosh(x, y)
    local mcrap = dink.create_sprite(x, y, brain.KILL_SEQ_DONE, 167, 1)
    mcrap.seq = 167
    dink.playsound(24, 22052, 0, nil, false)
  end
  
  cs:add_command("whoosh", whoosh)
  
  cs:add_participant("d", player)
  cs:add_participant("l", current_sprite, "9")
  cs:command("ass", {"d", "l"}, {
    {
      "Hey, it's a scroll from Phoenix!",
      "Who?!",
      "Let's see what he has to say..."
    },
    {
      "Dear player,",
      "Thank you for testing my Lua version of the original",
      "Dink Smallwood quest.",
      "I did not see it worth the while to re-code the whole quest",
      "so the Lua story ends here.",
      "If you have not done so already",
      "make sure you take a look at the Lua scripts that came with",
      "this special version of Dink Smallwood, and feel free to",
      "compare them with the original DinkC scripts, to see for",
      "yourself which script language you prefer the most.",
      "I have obviously chosen Lua, which is my reason for",
      "bothering to implement support for it in the first place.",
      "Thanks again for playing, feel free to leave me comments,",
      "suggestions and bugs in my Dink Network forum thread(s),",
      "or at the Lua Dink github page (should be linked to from",
      "all relevant discussion threads.)"
    },
    {
      "Huh. I've never read such gibberish. And who is Phoenix?",
      "Oh wait, there's a little more text",
      "on the other side of the letter."
    },
    {
      "P.S. Why don't you try to write some Lua-driven",
      "Dink adventure yourself? I've tried documenting the",
      "differences and new features as best I can on the",
      "github-associated Wiki.",
      "You will now be teleported to the Operating System."
    },
    {
      "What in the world is this Phoenix person talking about?",
      "I've never heard of this Lua or github. I do see my name",
      "in there, though... DinkC, Dink Network.",
      "This kinda creeps me out. And what is an Operating System?",
    }
  })
  player:say("Hey! What's going on?!")
  whoosh(player.x, player.y + 20)
  dink.wait(200)
  whoosh(player.x, player.y - 20)
  dink.wait(200)
  whoosh(player.x + 20, player.y)
  dink.wait(200)
  whoosh(player.x - 20, player.y)
  dink.wait(200)
  whoosh(player.x, player.y)
  dink.wait(200)
  
  dink.fade_down();
  dink.kill_game();
end

