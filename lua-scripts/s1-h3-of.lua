local function prepare_cutscene(wait_time)
  local cs = cutscene.create_cutscene(wait_time)
  cs:add_participant("s", current_sprite, "7")
  cs:add_participant("d", player)
  return cs
end

function main()
  if dink.random(4, 1) == 4 then
    current_sprite:say_stop("`7I'm sorry my dear, I miss you...")
    current_sprite:say_stop("`7Oh Dink!!  Sorry, didn't see you there.")
  end
  if global.story == 5 then
    current_sprite:say_stop("`7Dink!  I hope you're doing okay.")
    dink.wait(250)
    current_sprite:say_stop("`7Tragic, what happened.")
  end
  if global.farmer_quest == 1 then
    current_sprite:say("`7Oh Dink, how goes the battle?")
  elseif global.farmer_quest == 2 then
    current_sprite:say("`7Ah Dink, savior of my farm!")
  end
end
  
local choice_menu = dink.create_choice_menu()
local choice_family       = choice_menu:add_choice("Ask how the family's doing")
local choice_farm_inquire = choice_menu:add_choice("Inquire about his farm", function() return global.farmer_quest == 0 end)
local choice_libby        = choice_menu:add_choice("Ask to see Libby")
local choice_farm_info    = choice_menu:add_choice("Get info about the farm", function() return global.farmer_quest == 1 end)
local choice_leave        = choice_menu:add_choice("Leave")

local farm_menu = dink.create_choice_menu()
local choice_take_care  = farm_menu:add_choice("Offer to take care of the problem")
local choice_what_to_do = farm_menu:add_choice("Ask what he plans to do")
    
function talk()
  local cs = prepare_cutscene(0)
  
  player:freeze()
  current_sprite:freeze()
  dink.wait(200)
  
  local choice_result = choice_menu:show()
  
  local lines
  if choice_result == choice_family then
    cs:command("ass", {"d", "s"}, {
        "So how is the family?",
        "We're fine.",
        "Ah."
    })
    if global.story == 5 then
      cs:command("ass", {"s", "d"}, {
        {
          "Dink, I'm so sorry about your mother.",
          "She was a good lady."
        },
        "Thanks."
      })
    end
  elseif choice_result == choice_farm_inquire then
    cs:command("ass", {"d", "s"}, {
      "So how is your farm?  Growing much?",
      {
        "The farm... oh the farm!!  Those damn monsters,",
        "they're really starting to bother me.",
        "They come in from the forest and tear up the fields!",
        "I gotta do something about them soon ..."
      }
    })
  
    local farm_result = farm_menu:show()
    if farm_result == choice_take_care then
      global.farmer_quest = 1
      cs:command("ass", {"d", "s"}, {
        "Don't worry Mr. SmileStein, I'll take care of the problem.",
        {
          "Really Dink??  Oh thank you, thank you.",
          "Just come back and tell me when you've defeated them."
        },
        "No problem sir."
      })
    elseif farm_result == choice_what_to_do then
      cs:command("ass", {"d", "s"}, {
        "What are you planning?",
        {
          "I'm thinking of hiring a hunter from PortTown to come",
          "and take care of them!  That'll help next year's harvest."
        }
      })
    end
  elseif choice_result == choice_libby then
    player:say_stop("Can I go up and see Libby?")
    dink.wait(250)
    if global.farmer_quest == 2 then
      current_sprite:say_stop("`7For you Dink, yes go right up.")
    else
      cs:command("ass", {"s", "d"}, {
        {
          "No, no you may not right now.",
          "She is not to be disturbed."
        },
        "Oh"
      })
    end
  elseif choice_result == choice_farm_info then
    cs:command("ss", "s", {
      "Well, my farm is off to the west a bit.",
      "The monsters seem to be trampling it constantly.",
      "Please see what you can do Dink."
    })
  end
  player:unfreeze()
  current_sprite:unfreeze()
end

function hit()
  current_sprite:say_stop("`7Please, I'm not in the mood.")
end

