-- bootstrap is the script that gets used by the engine to load the Lua State
print('hello, world')

--if Actor.new({}) then print("actor") end

Actor.new {
	alpha = 255,
	x = 0,
	y = 0,
	w = 32,
	h = 32,
}

Actor.new {
	alpha = 128,
	x = 32,
	y = 32,
	w = 64,
	h = 64,
}
