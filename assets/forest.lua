-- require("assets.mobdebug").start() -- same as start("localhost", 8172)

---------------------------------- vec2 -------------------------------------
local vec2 = {}
vec2.__index = vec2

function vec2.new(x, y)
    local self = setmetatable({}, vec2)
    self.x = x
    self.y = y
    return self
end

function vec2.__add(self, other)
    return vec2.new(self.x + other.x, self.y + other.y)
end

function vec2.__sub(self, other)
    return vec2.new(self.x - other.x, self.y - other.y)
end

function vec2.__mul(self, value)
    return vec2.new(self.x * value, self.y * value)
end

function vec2.__div(self, other)
    return vec2.new(self.x / other.x, self.y / other.y)
end

function vec2.__eq(self, other)
    return self.x == other.x and self.y == other.y
end

function vec2.dot(self, other)
    return self.x * other.x + self.y * other.y
end

function vec2.length(self)
    return math.sqrt(self.x * self.x + self.y * self.y)
end

function vec2.distance(self, other)
    return (self - other):length()
end

function vec2.normalize(self)
    local len = self:length()
    if len > 0 then
        self.x = self.x / len
        self.y = self.y / len
    end
    return self
end

function vec2.__tostring(self)
    return "(" .. self.x .. ", " .. self.y .. ")"
end

-------------------------------- helpers ------------------------------------
function create_tile_sprite(image, index, dx, dy)
    local width = render.get_image_width(image)
    local height = render.get_image_height(image)
    local sx = width / dx
    local x = index % math.floor(sx)
    local y = math.floor(index / math.floor(sx))
    log("index="..index)
    log("x="..x)
    log("y="..y)
    return render.create_sprite(image,
            (dx * x) / width,
            (dy * y + dy) / height,
            (dx * x + dx) / width,
            (dy * y) / height)
end

function create_tile_sprite_range(image, from, to, tile_size)
    local sprites = {}
    local i = 0
    for index = from, to do
        sprites[i] = create_tile_sprite(image, index, tile_size, tile_size)
        i = i + 1
    end
    return sprites
end

function round(x)
    return x + 0.5 - (x + 0.5) % 1
end

function abs(x)
    if x < 0 then
        return -x
    end
    return x
end

function rand(x, y)
    local co = vec2.new(x, y)
    co:dot(vec2.new(12.9898, 78.233))
    local value = math.sin(co.x) * 43758.5453
    return value - math.floor(value)
end

function rand_range(rnd, from, to)
    return round(from + rnd * (to - from))
end

-------------------------------- animation ------------------------------------
local animation = {}
animation.__index = animation

function animation.new()
    local self = setmetatable({}, animation)
    self.sprites = {}
    self.current_frame = 0
    self.frame_time = 0.0
    self.frame_duration = 1.0 / 6.0
    return self
end

function animation.update(self, dt)
    self.frame_time = self.frame_time + dt
    if self.frame_time > self.frame_duration then
        self.frame_time = 0.0
        self.current_frame = (self.current_frame + 1) % (#self.sprites);
    end
end

function animation.get_current_sprite(self)
    return self.sprites[self.current_frame]
end

------------------------------- character ------------------------------------
local character = {}
character.__index = character
character.state = { idle = 0, walk = 1 }
character.direction = { left = 0, right = 1 }

function character.new()
    local self = setmetatable({}, character)
    local image = render.load_image("assets/playerSprites_/fPlayer_ [human].png")
    self.current_state = character.state.idle
    self.current_direction = character.direction.right
    self.idle_animation = animation.new()
    self.idle_animation.sprites = create_tile_sprite_range(image, 8, 11, 32)
    self.walk_animation = animation.new()
    self.walk_animation.sprites = create_tile_sprite_range(image, 16, 23, 32)
    self.position = vec2.new(0, 0)
    self.speed = 50.0
    return self
end

function character.update(self, dt)
    -- Get input
    local dx = input.get_axis(input.axis.stick_left_x)
    local dy = -input.get_axis(input.axis.stick_left_y)
    -- Account for deadzone
    if abs(dx) < 0.1 then
        dx = 0
    end
    if abs(dy) < 0.1 then
        dy = 0
    end
    -- Update state and position
    if dx ~= 0 or dy ~= 0 then
        self.current_state = character.state.walk
        self.position = self.position + vec2.new(dx, dy):normalize() * self.speed * dt
    else
        self.current_state = character.state.idle
    end
    -- Update direction
    if dx < 0 then
        self.current_direction = character.direction.left
    elseif dx > 0 then
        self.current_direction = character.direction.right
    end

    -- Update animation
    if self.current_state == character.state.idle then
        self.idle_animation:update(dt)
    elseif self.current_state == character.state.walk then
        self.walk_animation:update(dt)
    end
end

function character.draw(self)
    local flags = 0
    if self.current_direction == character.direction.left then
        flags = render.flip_x
    end
    if self.current_state == character.state.idle then
        render.render_sprite(self.idle_animation:get_current_sprite(),
                self.position.x, self.position.y, 2, 1.0, 0.0, 0xFFFFFFFF, flags)
    elseif self.current_state == character.state.walk then
        render.render_sprite(self.walk_animation:get_current_sprite(),
                self.position.x, self.position.y, 2, 1.0, 0.0, 0xFFFFFFFF, flags)
    end
end

-------------------------------- main ----------------------------------------
local player = nil
local ground_sprites = {}

-- A game script has a init() function that is called when the game is started.
-- The init() function is called only once.
function init()
    image = render.load_image("assets/forest_/forest_.png")

    local ids = {30, 31, 32, 33, 52, 53, 54, 55, 76, 77};

    --[[ Create sprites
    for i = 0, 15 do
        ground_sprites[i] = {}
        for j = 0, 15 do
            local index = math.random(1, #ids)
            index = ids[index]
            ground_sprites[i][j] = create_tile_sprite(image, index, 16, 16)
        end
    end
    ]]--

    -- Create sprites
    for i = 1, #ids do
        ground_sprites[i] = create_tile_sprite(image, ids[i], 16, 16)
    end

    player = character.new()
end

-- A game script has a update() function that is called every frame.
-- The update() function is called repeatedly until the game is stopped.
function update(dt)
    -- log("update "..dt)
    player:update(dt)
end

-- A game script has a render() function that is called every frame.
-- The render() function is called repeatedly until the game is stopped.
function draw()
    -- log("draw")

    local player_tile_pos = vec2.new(
            round(player.position.x / 16),
            round(player.position.y / 16))
    local from_x = player_tile_pos.x - 8
    local to_x = player_tile_pos.x + 8
    local from_y = player_tile_pos.y - 8
    local to_y = player_tile_pos.y + 8


    -- Render ground
    for i = from_x, to_x do
        for j = from_y, to_y do
            local x = i * 16
            local y = j * 16
            log("x="..x.." y="..y)
            local rnd = rand(x, y)
            log("rnd="..rnd)
            local idx = rand_range(rnd, 1, #ground_sprites)
            log("sprite idx="..idx)
            local sprite = ground_sprites[idx]
            render.render_sprite(sprite, x, y, 0, 1.0, 0.0, 0xFFFFFFFF, 0)
        end
    end

    player:draw()

    -- render.render_sprite(sprite, 0, 0, 1, 1.0, 0.0, 0xFFFFFFFF, 0)
    -- render.render_sprite(sprite2, 0, 0, 2, 1.0, 0.0, 0xFFFFFFFF, 0)
end

-- A game script has a shutdown() function that is called when the game is stopped.
-- The shutdown() function is called only once.
function shutdown()
    log("shutdown")
end
