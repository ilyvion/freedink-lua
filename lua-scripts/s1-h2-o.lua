-- old woman who has a pet duck

function main()
  if global.old_womans_duck == 5 then
    current_sprite:say("`3Hello murderer!!")
  elseif global.old_womans_duck == 0 then
    current_sprite:say("`3Why hello, Dink.")
  elseif global.old_womans_duck == 1 then
    current_sprite:say("`3Oh Dink, I'm so worried - have you found him?")
  elseif global.old_womans_duck == 2 then
    global.old_womans_duck = 4
    current_sprite:say_stop("`3Oh Dink, look who is here!")
    dink.wait(200)
    current_sprite:say_stop("`3Also Dink, your mother was looking for you.")
  end
end

function talk()
  if global.old_womans_duck == 5 then
    current_sprite:say("`3Get away from me!")
    return
  end
  
  player:freeze()
  current_sprite:freeze()
  
  if global.story > 3 then
    current_sprite:say_stop("`3I'm so sorry about your mother Dink,")
    dink.wait(200)
    current_sprite:say_stop("`3she was a good woman.")
    if global.old_womans_duck == 3 then
      current_sprite:say_stop("`3Kinda funny, my duck missing and your Mom dead.")
    elseif global.old_womans_duck == 5 then
      current_sprite:say_stop("`3Kinda funny, my duck and your Mom dead.")
    end
    current_sprite:unfreeze()
    player:unfreeze()
    return
  end
  
  if global.old_womans_duck > 0 and global.old_womans_duck < 5 then
    if global.old_womans_duck == 2 then
      current_sprite:say_stop("`3I'm so grateful to you, Dink!  You are such a dear!")
      return
    elseif global.old_womans_duck == 4 then
      current_sprite:say_stop("`3I'm so grateful to you, Dink!  You are wonderful!")
    elseif global.old_womans_duck == 3 then
      current_sprite:say_stop("`3I wonder where my duck is?")
      player:say("<chortles>")
    elseif global.old_womans_duck == 1 then
      current_sprite:say_stop("`3You must keep searching for Quackers!  I loved him!")
    end
    
    current_sprite:unfreeze()
    player:unfreeze()
    return
  end
  
  local choice_menu = dink.create_choice_menu()
  local choice_well_being = choice_menu:add_choice("Ask about her well being")
  local choice_duck = choice_menu:add_choice("Ask after her pet")
  choice_duck.condition = global.old_womans_duck == 0
  local choice_bottles = choice_menu:add_choice("Inquire about all her bottles")
  local choice_leave = choice_menu:add_choice("Leave")
  
  local choice_result = choice_menu:show()
  
  if choice_result == choice_leave then
    player:unfreeze()
    current_sprite:unfreeze()
  elseif choice_result == choice_well_being then
    player:say_stop("How are you today, Ethel?")
    dink.wait(500)
    if global.old_womans_duck == 0 then
      current_sprite:say_stop("`3Not so good, Dink.  Little Quackers is missing!")
    else
      current_sprite:say_stop("`3I'm fine Dink, thank you for asking.")
    end
  elseif choice_result == choice_duck then
    player:say_stop("Where is the little one today?")
    dink.wait(500)
    current_sprite:say_stop("`3Quackers is gone!  Will you help me find him?")
    dink.wait(500)
    
    local choice_menu = dink.create_choice_menu()
    local choice_wholeheartedly = choice_menu:add_choice("Agree whole heartedly")
    local choice_barely = choice_menu:add_choice("Barely agree")
    local choice_stick = choice_menu:add_choice("Tell her where she can stick 'Quackers'")
    
    local choice_result = choice_menu:show()
    
    if choice_result == choice_wholeheartedly then
      player:say_stop("I will find him at once dear Ethel, do not doubt this!")
      dink.wait(500)
      current_sprite:say_stop("`3Thank you Dink!")
      dink.wait(500)
      global.old_womans_duck = 1
    elseif choice_result == choice_barely then
      player:say_stop("Yeah, I guess if I see 'em I'll send him home.  Maybe.")
      dink.wait(500)
      current_sprite:say_stop("`3I see...thank.. you .. I guess.")
      dink.wait(500)
      global.old_womans_duck = 1
    elseif choice_result == choice_stick then
      player:say_stop("You're pathetic.  Find your own duck.")
      dink.wait(500)
      current_sprite:say_stop("`3I.. I... didn't know... <begins to tear up>")
      dink.wait(500)
    end
  elseif choice_result == choice_bottles then
    player:say_stop("Hey Ethel, what's with all the spirits on the wall there?")
    player:say_stop("You really like to party don't you?")
    dink.wait(200)
    current_sprite:say_stop("`3What Dink?")
    dink.wait(200)
    player:say_stop("You know, all drowning away your problems on the weekend")
    player:say_stop("waking up with guys you don't even know.")
    player:say_stop("Ahh the regrets, right Ethel?")
    dink.wait(200)
    current_sprite:say_stop("`3Dink ..")
    current_sprite:say_stop("`3You've got problems.")
  end
    
  current_sprite:unfreeze()
  player:unfreeze()
  return
end

