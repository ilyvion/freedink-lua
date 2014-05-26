function main()
  if global.story == 3 then
    global.vision = 1
    
    local cs = cutscene.create_cutscene(250)
    cs:add_participant("d", player)
    cs:scene({
      {"w", 1000},
      {"fza"},
      {"w", 0},
      {"ss", "d", "Mother noooooo!"},
      {"msn", "d", 259},
      {"ih", false},
      {"msw", "d", 203},
      {"w", 200},
      {"msn", "d", 151},
      {"ss", "d", {
        "Mother, you can't die, nooo I  I ...",
        "never knew how much I really cared about you",
        "until now.",
        "Ahh, too much smoke .... gotta get out ..."
      }},
      {"fn", "d", function() global.story = 4 end},
      {"mss", "d", 259},
      {"mse", "d", 321},
      {"mss", "d", 398}
    })
    return
  elseif global.story > 3 then
    if global.letter == 1 then
      dink.debug("Why am I here...story is "..global.story.." and letter is "..global.letter)
      global.vision = 2
      local thing = dink.create_sprite(404, 220, brain.NONE, 422, 5)
      thing.script = "s1-ltr"
      player:move_stop(direction.NORTH, 370, true)
      player:say("There's that letter.")
      dink.wait(500)
      player:move_stop(direction.NORTH, 185, true)
      player:move_stop(direction.EAST, 373, true)
      return
    end
    global.vision = 2
    player:say("My old home ...")
  end
end

