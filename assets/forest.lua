print("loading lua script: assets/forest.lua")


local image
local sprite

-- A game script has a init() function that is called when the game is started.
-- The init() function is called only once.
function init()
    image = render.load_image("assets/forest_/forest_.png")
    sprite = render.create_sprite(image, 0, 0, 0.5, 0.5);
end

-- A game script has a update() function that is called every frame.
-- The update() function is called repeatedly until the game is stopped.
function update(dt)
    log("update "..dt)
end

-- A game script has a render() function that is called every frame.
-- The render() function is called repeatedly until the game is stopped.
function draw()
    log("draw")
    render.render_sprite(sprite, 0, 0, 2, 1.0, 0.0, 0xFFFFFFFF, 0)
end

-- A game script has a shutdown() function that is called when the game is stopped.
-- The shutdown() function is called only once.
function shutdown()
    log("shutdown")
end
