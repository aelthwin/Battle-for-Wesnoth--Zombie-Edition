local this = {}

--[[
Trevor's notes...

	wesnoth.* functions defined in src/scripting/lua.cpp
	ai.*      functions defined in src/ai/lua/core.cpp

	plenty of example lua code floating around in the codebase
	if you do a grep for "get_units"... found this while trying to discover
	where get_units was defined... turns out it's in lua.cpp (see above)

	Another useful resource is scenario-lua-ai.cfg

	If you see a C++ implementation of a lua function, and its argument is
	a WML table... then look here: http://wiki.wesnoth.org/FilterWML
	An example of this is intf_get_locations.

	I use the [] notation a lot... because I'm comfortable with arrays, but in
	Lua, this notation makes no sense.  "Arrays" in the conventional sense are
	just tables, and most of the Lua websites I have found are very adamant
	about tables not being called arrays.

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
		string.format   (format, arg1, arg2, ... argN)  like printf in C

	Wesnoth object:
		Comes from:     built-in (lua.cpp)
		Properties:     unit_types[]   -- array of UnitType objects
		                sides[]        -- array of Side objects
		Functions:      << defined in src/scripting/lua.cpp as a callbacks array >>
		                << some useful functions: >>
		                get_units
				find_path      -- find a path... args and return on line 1804 of lua.cpp

	AI object:
		Comes from:     built-in (core.cpp)
		Properties:     side
		Functions:      move (unit, x, y) returns table -> {ok = bool, gamestate_changed = bool, status = int}

	Unit objects:
		Come from:      wesnoth.get_units, wesnoth.get_unit
		Properties:     <<defined in lua.cpp, impl_unit_get function >>
		                << some interesting properties: >>
		                x              -- x position
		                y              -- y position
		                type           -- type number
		                hitpoints      --
		                max_hitpoints  --
		                experience     --
		                max_experience --
				max_moves      -- 
				id             -- an identifier for the unit

	UnitType objects:
		Come from:      wesnoth.unit_types[]
		Properties:     << defined in lua.cpp, impl_unit_type_get function >>
		                << some interesting properties: >>
		                cost           -- $ gold to recruit
		                max_moves      -- ! Do not use, not accurate !
		                __cfg          -- variables from the race config file under the [unit_type] tag ??

	Side objects:
		Come from:      wesnoth.sides[]
		Properties:     << defined in lua.cpp, impl_side_get function >>
		                << some interesting properties: >>
		                gold           -- $ gold this side has

]]--




--[[ Probability table object, does all the hard bayes stuff  :) ]]--
this.probs = wesnoth.require('ai/lua/zombies/probs_table.lua')
this.helper = wesnoth.require('ai/lua/zombies/csc_helper.lua')


--[[ Configuration variables ]]--
this.attack_radius = 5 -- happens to be the move range of a Zombie



--[[ Variables set by init ... with some default values  :) ]]--
this.side      = 0
this.ai        = nil



--[[

Initialize the AI engine.  This will only be run once!

]]--
function this.init (ai)
	this.ai = ai
	this.side = this.ai.side -- for convenience
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
	
	print ("Start WANDER do_moves")

	-- make sure some class level vars are there
	this.units = wesnoth.get_units ({side=this.ai.side})
	this.leader_id = this.leader_id or this.units[1].id

	
	-- print out some state information...
	print ("Zombie gold:  " .. wesnoth.sides[this.side].gold)
	print ("Recruit cost: " .. wesnoth.unit_types[this.units[1].type].cost)
	print ("Leader id:    " .. this.leader_id)
	print ("Move rate:    " .. this.units[1].max_moves)


	-- Do moves for each unit
	for i, unit in ipairs (this.units) do

		print ("Unit " .. unit.x .. ", " .. unit.y .. "  (" .. unit.id .. ") doing moves")

		-- Action pipeline...
		local continue = true

		-- try recruiting first...
		if continue and this.helper.can_recruit ({unit = unit}) then
			this.helper.do_recruit ({
				unit = unit,
				ai   = this.ai
			})
			-- recruited!  done...  fall through in case there's anything else we can do
		end -- RECRUITING PHASE

		-- recklessly attack any enemies within smell distance
		if continue then
			local enemy_units = this.helper.enemy_units_in_range ({unit = unit, radius = this.attack_radius})
			for i, e_unit in pairs (enemy_units) do
				print ("WANDER: attacking enemy unit, " .. e_unit.x .. ", " .. e_unit.y)
				this.helper.move_and_attack ({unit = unit, enemy = e_unit, ai = this.ai})
				continue = false -- no continue if we've tried to attack at least one enemy
			end
		end

		-- Fall back on random movement		
		if continue then
			this.helper.move_randomly ({
				unit = unit,
				ai   = this.ai
			})
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

	print ("End WANDER do_moves")
	return
end

return this
