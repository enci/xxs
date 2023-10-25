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
local props_sprites = {}
local large_props_sprites = {}
local camera_position = vec2.new(0, 0)


-- A game script has a init() function that is called when the game is started.
-- The init() function is called only once.
function init()

    -- Ground
    local ground_image = render.load_image("assets/forest_/forest_.png")
    local ground_ids = {30, 31, 32, 33, 52, 53, 54, 55, 76, 77};
    for i = 1, #ground_ids do
        ground_sprites[i] = create_tile_sprite(ground_image, ground_ids[i], 16, 16)
    end

    -- Props
    local props_ids = {6, 7, 8, 9, 20, 30, 23, 33, 24, 34, 25, 35, 26, 36, 27, 37, 28, 38, 29, 39};
    local props_image = render.load_image("assets/forest_/forest_ [resources].png")
    for i = 1, #props_ids do
        props_sprites[i] = create_tile_sprite(props_image, props_ids[i], 16, 16)
    end

    -- Large props
    local large_props_ids = {0, 1, 5};
    local large_props_image = render.load_image("assets/forest_/forest_ [resources].png")
    for i = 1, #large_props_ids do
        large_props_sprites[i] = create_tile_sprite(large_props_image, large_props_ids[i], 16, 32)
    end

    player = character.new()
end

-- A game script has a update() function that is called every frame.
-- The update() function is called repeatedly until the game is stopped.
function update(dt)
    player:update(dt)

    -- Update camera
    camera_position = player.position
end

-- A game script has a render() function that is called every frame.
-- The render() function is called repeatedly until the game is stopped.
function draw()
    local camera_tile_pos = vec2.new(
            round(camera_position.x / 16),
            round(camera_position.y / 16))
    local from_x = camera_tile_pos.x - 9
    local to_x = camera_tile_pos.x + 8
    local from_y = camera_tile_pos.y - 9
    local to_y = camera_tile_pos.y + 8
    render.set_offset(-camera_position.x, -camera_position.y)

    -- Render ground
    for i = from_x, to_x do
        for j = from_y, to_y do
            local x = i * 16
            local y = j * 16
            math.randomseed(x, y)
            local grd = math.random(1, #ground_sprites)
            local sprite = ground_sprites[grd]
            render.render_sprite(sprite, x, y, 0, 1.0, 0.0, 0xFFFFFFFF, 0)

            if math.random() < 0.20 then
                local prp = math.random(1, #large_props_sprites)
                local sprite = large_props_sprites[prp]
                render.render_sprite(sprite, x, y, 2, 1.0, 0.0, 0xFFFFFFFF, 0)
            elseif math.random() < 0.35 then
                local prp = math.random(1, #props_sprites)
                local sprite = props_sprites[prp]
                render.render_sprite(sprite, x, y, 2, 1.0, 0.0, 0xFFFFFFFF, 0)
            end

        end
    end

    player:draw()
end

-- A game script has a shutdown() function that is called when the game is stopped.
-- The shutdown() function is called only once.
function shutdown()
    log("shutdown")
end