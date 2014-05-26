local function story()
  local maybe = dink.random(3, 1)
  if maybe == 1 then
    current_sprite:say_stop("`9I just escaped from the Goblin Sanctuary.")
    current_sprite:say_stop("`9It's quite a horrible place and I wouldn't recommend going there.")
    dink.wait(250)
    player:say_stop("That place is far to the west, how'd you get out here?")
    current_sprite:say_stop("`9Well I've just been headed away from it ever since I got out.")
    current_sprite:say_stop("`9I hope I can make it to PortTown and get away for a while.")
  elseif maybe == 2 then
    current_sprite:say_stop("`9I've always wanted to go out into the world and find adventure.")
    current_sprite:say_stop("`9Whether it be working with the Royal Guard or signing up")
    current_sprite:say_stop("`9with a band of mercenaries.")
    dink.wait(250)
    current_sprite:say_stop("`9Unfortunately for me one of my first jobs led me right into")
    current_sprite:say_stop("`9being captured by the goblins.  Last time I go to the WestLands.")
  elseif maybe == 3 then
    current_sprite:say_stop("`9After being around those goblins for so long I'm glad to be out.")
    current_sprite:say_stop("`9Those guy are dumb as bricks man!")
    current_sprite:say_stop("`9I'm embarrassed that I got captured in the first place.")
  end
end

local function makefun()
  if global.story == 5 then
    current_sprite:say_stop("`9Some other guy I meet wandering told me there was a bad fire")
    current_sprite:say_stop("`9around these parts.  Hope no one was injured.")
    dink.wait(200)
    player:say_stop("Yes.. I'm sure no one was ... hurt ...")
    return false
  end
  player:say_stop("Greetings friend, any news?")
  dink.wait(200)
  current_sprite:say_stop("`9Not really.  What's your name anyway?")
  dink.wait(200)
  player:say_stop("Smallwood,")
  dink.wait(250)
  player:say_stop("Dink Smallwood.")
  dink.wait(200)
  current_sprite:say_stop("`9Hello, I'm Chance `Zands")
  dink.wait(200)
  current_sprite:say_stop("`9You have an interesting name there.")
  dink.wait(200)
  player:say_stop("Thank you.")
  return true
end

local function news()
  local maybe = dink.random(5, 1)
  if maybe == 1 then
    if not makefun() then
      return
    end
  end
  
  maybe = dink.random(3, 1)
  if maybe == 1 then
    --dink.playmidi("creepy.mid")
    dink.wait(1000)
    current_sprite:say_stop("`9When I was captured at the Goblin Sanctuary I did notice something.")
    current_sprite:say_stop("`9All their patrols that came back from the north seemed different.")
    current_sprite:say_stop("`9Soon they started behaving really weird, not much later a few were")
    current_sprite:say_stop("`9locked up right next to me and going crazy!")
    dink.wait(200)
    player:say_stop("Man, sounds pretty creepy.")
    dink.wait(200)
    current_sprite:say_stop("`9Just another reason I'm headed away for a while.")
    dink.wait(200)
  elseif maybe == 2 then
    current_sprite:say_stop("`9While imprisoned, the goblins said that by the Crag cliffs")
    current_sprite:say_stop("`9There was a wizard that they were fighting, they thought")
    current_sprite:say_stop("`9they could actually get his magic if they defeated him.")
    current_sprite:say_stop("`9Anyway they said this guy could actually turn people to ice!!")
    current_sprite:say_stop("`9I'd guess they lost about 30 troops before they gave up.")
  elseif maybe == 3 then
    current_sprite:say_stop("`9I hear your Mom's a slut.  But that's just the word on")
    current_sprite:say_stop("`9the street.  Ha Ha Ha")
    current_sprite:say_stop("`9Ha Ha .. Ha ..")
    current_sprite:say_stop("`9 ... Ha ... ")
    player:say_stop("Ha Ha Ha ...")
    current_sprite:say_stop("`9 .. Ha Ha ..")
    
    if global.story > 4 then
      player:say_stop("Ha Ha ... wait!!!!")
      player:say_stop("My Mom's dead!!!!!")
    end
  end
end

function main()
  if dink.random(3, 1) == 1 then
    if dink.random(3, 1) == 1 then
      current_sprite.speed = 7
      current_sprite.timing = 0
      current_sprite:move(direction.EAST, 510, true)
      current_sprite.speed = 1
      current_sprite.timing = 33
      current_sprite:say_stop("`9I hope they didn't follow me ..")
    else
      current_sprite:say_stop("`9Hello friend..")
    end
  end
end

function talk()
  player:freeze()
  current_sprite:freeze()
  
  local choice_menu = dink.create_choice_menu()
  local choice_travels = choice_menu:add_choice("Ask about his travels")
  local choice_news = choice_menu:add_choice("Ask for news")
  local choice_alktree = choice_menu:add_choice("Ask if he has any AlkTree nuts")
  choice_alktree.condition = global.story == 2
  local choice_leave = choice_menu:add_choice("Leave")
  
  local choice_result = choice_menu:show()
  
  if choice_result == choice_travels then
    story()
  elseif choice_result == choice_news then
    news()
  elseif choice_result == choice_alktree then
    current_sprite:say_stop("`9AlkTree nuts??  I haven't had those in a long long time.")
    current_sprite:say_stop("`9In my old town I might have been able to tell you,")
    current_sprite:say_stop("`9but I don't know of any around here.")
  end
  
  player:unfreeze()
  current_sprite:unfreeze()
end

function hit()
  current_sprite:say("`9Ow, the hell's your problem?!")
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
  player:say("He won't bother anyone anymore...")
end

