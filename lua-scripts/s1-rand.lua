local misc = include("_misc")

function main()
  if dink.random(8,1) == 1 then
    if dink.random(2,1) == 1 then
      misc.preload_diagonal_seq(340)
      maketraveler(340, 5, "s1-wand")
    else
      misc.preload_diagonal_seq(380)
      maketraveler(380, 2, "s1-wand2")
    end
  end
end

function maketraveler(base_seq, initial_frame, script)
  local spawnx, spawny = dink.random(200,200), dink.random(100, 150)
  local crap = dink.create_sprite(spawnx, spawny, brain.MONSTER_DIAGONAL, base_seq + 1, initial_frame)
  crap.speed = 1
  crap.timing = 33
  crap.brain = brain.SMART_PEOPLE
  crap.base_walk = base_seq
  crap.script = script
end

