-- bootstrap is the script that gets used by the engine to load the Lua State
print('hello, world')

--if Actor.new({}) then print("actor") end

a = Actor.new {
	alpha = 255,
	x = 0,
	y = 0,
	w = 128,
	h = 128,
}

function a:up()
	self.alpha = self.alpha + 128
	if (Joysticks) then
		for k, v in pairs(Joysticks) do
			--print(k)
			--if Joysticks[1]['buttons'][k] then print(k .. ": " .. tostring(v)) end
		end
	end
	return a.down
end

function a:down()
	self.alpha = self.alpha - 128
	return a.up
end

a.update = a.up

Actor.new {
	alpha = 128,
	x = 128,
	y = 128,
	w = 64,
	h = 64,
}
