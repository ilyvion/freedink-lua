local function story()
  local maybe = dink.random(3, 1)
  if maybe == 1 then
    current_sprite:say_stop("`6Be careful in the West.  The Goblin castle has extreme")
    current_sprite:say_stop("`6defenses to say the least.")
    dink.wait(250)
    current_sprite:say_stop("`6I've seen many a traveler lose his head if you know what I mean.")
  elseif maybe == 2 then
    current_sprite:say_stop("`6I've heard of an old man who lives just north of SunCool pond.")
    current_sprite:say_stop("`6Although I've never seen him I have heard noises when I've")
    current_sprite:say_stop("`6gone by there.")
  elseif maybe == 3 then
    current_sprite:say_stop("`6Travel by sea is quite spendy these days. I suggest if")
    current_sprite:say_stop("`6you aren't a person of wealth you look towards perhaps")
    current_sprite:say_stop("`6working for your voyage or find a friend who will pay")
    current_sprite:say_stop("`6for you.  Otherwise you could be saving for a long time.")
  end
end

local function makefun()
  if global.story == 6 and dink.random(3, 1) == 1 then
    current_sprite:say_stop("`6I heard there was a bad fire in a nearby village,")
    current_sprite:say_stop("`6hope no one was hurt too bad.")
    dink.wait(500)
    player:say_stop("Yes.. I'm sure no one was ... hurt ...")
  elseif global.old_womans_duck == 3 or global.old_womans_duck == 5 then
    current_sprite:say_stop("`6In the small village nearby, I've heard there's")
    current_sprite:say_stop("`6a madman.  A murderer who stalks and kills the pets of the people!")
    dink.wait(250)
    current_sprite:say_stop("`6Horrible isn't it?")
    dink.wait(250)
    player:say_stop("Yes.. very uh .. horrible ..")
  else
    current_sprite:say_stop("`6So young man, what's your name?")
    player:say_stop("Smallwood, Dink Smallwood.")
    dink.wait(250)
    current_sprite:say_stop("`6Greetings Mr. Smallwood.")
  end
end

local function news()
  local maybe = dink.random(3, 1)
  if maybe == 1 then
    makefun()
    return
  end
  
  maybe = dink.random(3, 1)
  if maybe == 1 then
    current_sprite:say_stop("`6There's some talk of an old evil awakening to")
    current_sprite:say_stop("`6the west.  I don't know though, I think it's all")
    current_sprite:say_stop("`6just silly superstition.")
    dink.wait(500)
    player:say_stop("Yes.. I'm sure it's super ... stition ...")
  elseif maybe == 2 then
    current_sprite:say_stop("`6In the city of PortTown, the populous can seem quite")
    current_sprite:say_stop("`6rowdy at times.  Some say it's because of the frequenting")
    current_sprite:say_stop("`6of pirates in the town.  Either way, be careful if you")
    current_sprite:say_stop("`6ever travel there.")
  elseif maybe == 3 then
    current_sprite:say_stop("`6If you ever run into goblins, be careful.")
    current_sprite:say_stop("`6They're pretty prone to violence, so just don't")
    current_sprite:say_stop("`6upset them.  However they're quite stupid.")
    current_sprite:say_stop("`6You could probably tell them just about anything,")
    current_sprite:say_stop("`6they'll believe it.")
  end
end

function main()
  if dink.random(3, 1) == 1 then
    current_sprite:say_stop("`6Hello fellow traveller.")
  end
end

function talk()
  player:freeze()
  current_sprite:freeze()
  
  local choice_menu = dink.create_choice_menu()
  local choice_travels = choice_menu:add_choice("Inquire about his travels")
  local choice_news = choice_menu:add_choice("Ask about news")
  local choice_alktree = choice_menu:add_choice("Ask about AlkTree nuts")
  choice_alktree.condition = global.story == 2
  local choice_leave = choice_menu:add_choice("Leave")
  
  local choice_result = choice_menu:show()
  
  dink.wait(300)
  if choice_result == choice_travels then
    story()
  elseif choice_result == choice_news then
    news()
  elseif choice_result == choice_alktree then
    current_sprite:say_stop("`6AlkTree nuts??  Those make for a hearty meal.  Let's see..")
    current_sprite:say_stop("`6I think there's a tree to the south east of the small village")
    current_sprite:say_stop("`6near here.")
  end
  
  player:unfreeze()
  current_sprite:unfreeze()
end

function hit()
  current_sprite:say("`6Ow, crazy bastard!!")
  current_sprite.speed = 4
  current_sprite.timing = 0
  
  -- Disappear off screen
  local dir = dink.random(4, 1)
  if dir == 1 then
    current_sprite:move(direction.SOUTH_WEST, -100, true)
  elseif dir == 2 then
    current_sprite:move(direction.SOUTH_EAST, 700, true)
  elseif dir == 3 then
    current_sprite:move(direction.NORTH_WEST, -100, true)
  elseif dir == 4 then
    current_sprite:move(direction.NORTH_EAST, 700, true)
  end
  current_sprite.brain = brain.NONE
  current_sprite.kill = 2000
  dink.script_attach(0)
end

function die()
  player:say("I'm afraid he won't be making it to his destination.")
end

