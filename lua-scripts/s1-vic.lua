function talk()
  if global.safe == 0 then
    current_sprite:say("`3Help me please!")
  else
    local cs = cutscene.create_cutscene()
    cs:add_participant("d", player)
    cs:add_participant("g", current_sprite, "3")
    cs:scene({
      {"pmidi", "love.mid"},
      {"fza"},
      {"w", 200},
      {"ass", {"d", "g"}, {
        "You okay there pal?",
        "Thanks to you I am.  Those guys would've killed me!",
        {
          "Yea, I've never seen anything like that happen",
          "around here before."
        },
        {
          "Well, like I said I travel light and don't have",
          "much with me.",
          "Here, take this gold piece, it's all I can spare."
        }
      }},
      {"ufza"}
    })
    global.gold = global.gold + 1
    global.exp = global.exp + 15
    current_sprite.speed = 3
    current_sprite.timing = 0
    current_sprite:move_stop(direction.EAST, 680, true)
    current_sprite.active = false
  end
end

