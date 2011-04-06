local this = {}

--[[
Trevor's notes...

	wesnoth.* functions defined in src/scripting/lua.cpp
	ai.*      functions defined in src/ai/lua/core.cpp

	plenty of example lua code floating around in the codebase
	if you do a grep for "get_units"... found this while trying to discover
	where get_units was defined... turns out it's in lua.cpp (see above)


	Strings
		concatenate     var1 .. var2

	Variables
		assign with default value      var1 = var1 or default

	Arrays
		-- arrays seem to be 1-indexed in lua, weird right?

	Randoms
		math.randomseed                should call with os.time() for real randomness, except "os" not available here
		math.random (n,m)              random number on [n, m]

	Debugging
		print           (var1, var2, ...)   -- prints to console
		wesnoth.msg     (msg)               -- Message in Wesnoth

]]--




--[[ Probability table object, does all the hard bayes stuff  :) ]]--
this.probs = wesnoth.require('ai/lua/zombies/probs_table.lua')


--[[

Initialize the AI engine.  This will only be run once!

]]--
function this.init (ai)
	this.ai = ai
end


--[[

Simple test function to make sure that we can call this object

]]--
function this.echo (msg)
	wesnoth.message (msg)
end


--[[

Perform moves for each AI unit

]]--
function this.do_moves ()
	-- Uncomment the next line to verify that the do_moves function was called
	-- wesnoth.message ('doing moves!')

	-- Uncomment the next 3 lines to verify that the engine is only instantiated once
	--this.count = this.count or 1
	--this.echo ('bayes echoing!' .. this.count)
	--this.count = this.count + 1

	-- Uncomment the next line to verify that we can call functions on probs table obj.
	--this.probs.echo ('probs echo')

	ai_units = wesnoth.get_units ({side=this.ai.side})

	-- make sure some class level vars are there
	-- TLB @TODO optimize to prevent running this all the time
	this.first_unit = this.first_unit or ai_units[1]
	this.first_unit_x = this.first_unit_x or this.first_unit.x
	this.first_unit_y = this.first_unit_y or this.first_unit.y


	for i, unit in ipairs (ai_units) do
		if unit == this.first_unit then
			-- try recruiting first...
			-- TLB @TODO
			
			-- otherwise move
			this.moveUnit (unit)
		else
			this.moveUnit (unit)
		end
	end
	-- example code from scenario-lua-ai.cfg
	-- list of units on our side that can recruit?
	-- units = wesnoth.get_units({canrecruit = true, side = this.ai.side})
	-- move (partial move)
	-- ai.move(units[1],13, 22)
	-- full move. note that the my_leader still can be used altrough x and y are now different.
	-- ai.move_full(units[1], 11, 23)
	-- attack with auto weapon/aggression
	-- ai.attack(2, 12, 3, 12)
	-- attack with weapon selected
	-- ai.attack(3, 11, 3, 12, 1)
	-- attack with different aggression
	-- ai.attack(3, 13, 3, 12, -1, 0.9)
end

function this.moveUnit (unit)
	this.echo ("BAYES: moveUnit for BAYES not implemented")
end

return this
