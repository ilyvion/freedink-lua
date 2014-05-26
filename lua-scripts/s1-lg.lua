local s1, s2, s3, s4

function main()
  current_sprite.hitpoints = 30
  
  local tip = dink.random(3, 1)
  if tip == 1 then
    current_sprite:say("`5La la laaa La La")
  end
end

function talk()
  player:freeze()
  current_sprite:freeze()
  current_sprite:say_stop("`5Hey Dink, what's up?")
  player:say_stop("Hello Chealse ...")
  dink.wait(250)
  
  local choice_menu = dink.create_choice_menu()
  local choice_flirt = choice_menu:add_choice("Flirt with her")
  local choice_gossip = choice_menu:add_choice("Ask about gossip")
  local choice_milder = choice_menu:add_choice("Ask about Milder FlatStomp")
  local choice_duck = choice_menu:add_choice("Tell her you killed Ethel's duck")
  choice_duck.condition = global.old_womans_duck == 3
  choice_menu:add_choice("Nevermind")
  
  local choice_result = choice_menu:show()
  
  dink.wait(300)
  
  if choice_result == choice_flirt then
    player:say_stop("So baby, you still single and bothered?")
    player:say_stop("Cause you know I'm a real man for you.")
    dink.wait(250)
    current_sprite:say_stop("`5Whatever, Smallwood!!")
  elseif choice_result == choice_gossip then
    player:say_stop("So what's the latest word in town Chealse?")
    dink.wait(250)
    
    if s1 and s2 and s3 and s4 then
      current_sprite:unfreeze()
      player:unfreeze()
      current_sprite:say_stop("`5I can't think of anymore gossip, sorry Dink.")
      return
    end
    
    if global.old_womans_duck == 3 or global.old_womans_duck == 5 then
      if dink.random(3,1) == 1 then
        current_sprite:say_stop("`5Well I hear there's been a horrible murder of Ethel's duck!")
        current_sprite:say_stop("`5I don't know much now, but I'll tell you when I do.")
        player:say_stop("Oh, how tragic .... uh ...")
        dink.wait(250)
        player:say_stop("... gotta go")
        current_sprite:unfreeze()
        player:unfreeze()
        return
      end
    end
    
    while true do
      local topic = dink.random(4, 1)
      dink.debug("Ok, debug is "..topic)
      
      if topic == 1 and not s1 then
        current_sprite:say_stop("`5Well, I heard that the SmileSteins have quite an abusive family now.")
        current_sprite:say_stop("`5Despite their father being a model farmer.")
        dink.wait(250)
        player:say_stop("Are you sure, that sounds pretty far fetched.")
        dink.wait(250)
        current_sprite:say_stop("`5Yeah, oh yeah, just the other night when I was coming back")
        current_sprite:say_stop("`5from picking flowers, I heard Libby up in her room crying.")
        dink.wait(250)
        player:say_stop("No ... Libby?  Oh my.")
        
        if global.gossip == 0 then global.gossip = 1 end
        s1 = true
        break
      elseif topic == 2 and not s2 then
       current_sprite:say_stop("`5Libby and her new boyfriend seem really happy together.")
       dink.wait(250)
       player:say_stop("That's the guy from PortTown isn't it?")
       player:say_stop("He seems kinda bossy.")
       dink.wait(250)
       current_sprite:say_stop("`5Yeah, well she says she really likes him and that once")
       current_sprite:say_stop("`5you get to know him he's a great guy.")
       s2 = true
       break
      elseif topic == 3 and not s3 then
        current_sprite:say_stop("`5I hear that the monsters over in farmer SmileStein's field")
        current_sprite:say_stop("`5are really driving him crazy.  I've even heard him say he's")
        current_sprite:say_stop("`5thinking about hiring a hunter to clear them out.")
        dink.wait(250)
        player:say_stop("Wow")
        dink.wait(250)
        current_sprite:say_stop("`5Yeah, big stuff..")
        s3 = true
        break
      elseif topic == 4 and not s4 then
        current_sprite:say_stop("`5Well when I went to the coast the other day I heard the Pirates")
        current_sprite:say_stop("`5in PortTown are looking for some more help and that they may be")
        current_sprite:say_stop("`5waging a war with the Northern Trading Company!")
        player:say_stop("Wow Chealse, that's big.")
        dink.wait(250)
        player:say_stop("How did you hear all THAT stuff??")
        dink.wait(250)
        current_sprite:say_stop("`5Ok ok, my big sister Aby who lives in PortTown told me.")
        s4 = true
        break
      end
    end
  elseif choice_result == choice_milder then
    player:say_stop("What's with that big dummy Milder Flatstomp lately?")
    dink.wait(250)
    current_sprite:say_stop("`5Well I hear he applied to the Royal Guard.")
    current_sprite:say_stop("`5But besides that he's just a big oaf, but I")
    current_sprite:say_stop("`5also saw him flirting with Libby AND Lyna!")
    dink.wait(250)
    player:say_stop("Damn.. they must dig his uniform.  I HAVE to join!")
  elseif choice_result == choice_duck then
    player:say_stop("Guess what, I got a secret for ya Chealse.")
    dink.wait(250)
    current_sprite:say_stop("`5Really?  What is it Dink?")
    player:say_stop("Well....")
    dink.wait(500)
    player:say_stop("I WAS THE ONE WHO KILLED ETHEL'S DUCK!!!!!!!!!")
    current_sprite:unfreeze()
    current_sprite:say("`5Noooooooooooo")
    current_sprite.timing = 0
    current_sprite.speed = 10
  end
  player:unfreeze()
  current_sprite:unfreeze()
end

function hit()
  dink.playsound(12, 22050, 0, nil, false)
  current_sprite.timing = 0
  current_sprite.speed = 4
  current_sprite:say("`5Hey, watch it pig farmer!!")
  dink.wait(3000)
  current_sprite.timing = 33
  current_sprite.speed = 1
end

function die()
  global.little_girl = 2
  player:say("She won't be bothering people any more.")
end

