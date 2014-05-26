function main()
  if dink.random(4, 1) == 1 then
    current_sprite:say_stop("`3Oh Dink, it's you..")
    dink.wait(250)
  end
end

local choice_menu = dink.create_choice_menu()
local choice_coax      = choice_menu:add_choice("Coax her into telling you why she's upset", function() return global.mlibby == 0 end)
local choice_comfort   = choice_menu:add_choice("Try to comfort her")
local choice_brag      = choice_menu:add_choice("Brag about how you cleared their farm", function() return global.farmer_quest == 2 end)
local choice_nevermind = choice_menu:add_choice("Never mind")
      
local comfort_menu = dink.create_choice_menu()
local comfort_family    = comfort_menu:add_choice("Try to comfort her about her family")
local comfort_apologize = comfort_menu:add_choice("Apologize for being an ass")

function talk()
  local cs = cutscene:create_cutscene()
  cs:add_participant("d", player)
  cs:add_participant("l", current_sprite, "3")

  player:freeze()
  current_sprite:freeze()
  
  local choice_result = choice_menu:show()
  dink.wait(300)
  
  if choice_result == choice_coax then
    cs:command("ass", {"d", "l"}, {
      "Libby, what's wrong?  Why are you so upset?",
      "Nothing Dink, you wouldn't understand."
    })
    
    if global.gossip ~= 0 then
      global.mlibby = 1
      cs:command("ass", {"d", "l"}, {
        "It's your father isn't it?  Does he do anything to you?",
        "What?  What are you talking about?",
        "I've heard .. rumors Libby, it's okay to admit it you know.",
        "Admit?  Admit what?",
        "That your father hits you.",
        {
          "You fool, of course not!!  He doesn't do that!!",
          "I'm crying because my father's so upset."
        },
        "Oh ... I .... see.",
        "<sniff>",
        "Is your father upset because you won't satisfy him?",
        {
          "DINK!!  What the hell's wrong with you??",
          "It's been exactly one year since Mother died, that's why."
        },
        {
          "Uhhh.  I ... uhm ...",
          "Oh"
        }
      })
      local comfort_result = comfort_menu:show()
      dink.wait(300)
      if comfort_result == comfort_family then
        cs:command("ass", {"d", "l"}, {
          {
            "I'm sorry about your mother.  But I know how you feel",
            "I'm pretty upset myself you know."
          },
          "Thank you Dink, you've made me happy."
        })
      elseif comfort_result == comfort_apologize then
        cs:command("ss", "d", {
          "Uhh, hehe, sorry for being so insensitive and saying those things.",
          "Guess I shouldn't listen to rumors so much.",
          "Uhh .. hehe ..he ..",
          "Uhhhhh...",
          "Oh boy."
        })
      end
    end
  elseif choice_result == choice_comfort then
    cs:command("ass", {"d", "l"}, {
      {
        "Don't cry Libby.  It's okay whatever it is.",
        "Sometimes bad things just happen to us, we just",
        "have to learn to persevere through them."
      },
      "Thanks Dink, that's kind of you."
    })
  elseif choice_result == choice_brag then
    cs:command("ass", {"d", "l"}, {
      {
        "So I guess you noticed that your farm is doing better now huh?",
        "You know that was my handiwork out there."
      },
      {
        "Pretty good...",
        "...compared to your usual."
      },
      "Dohh!"
    })
  elseif choice_result == choice_nevermind then
    player:say_stop("Uh, never mind.  I have to get going now.")
  end
  
  player:unfreeze()
  current_sprite:unfreeze()
end

function hit()
  if dink.random(6, 1) == 1 then
    player:say_stop("You know I hate to do this to you baby.")
    return
  end
  current_sprite:say_stop("`3Noo Dink!!  What's your problem...")
end

