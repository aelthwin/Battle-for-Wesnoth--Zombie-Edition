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



--[[

Loads up the probability tables, does pre-calculations

]]--
this.init = function ()
	wesnoth.message ('init is undefined!')
end



--[[

Simple test function to make sure that we can call this object

]]--
this.echo = function (msg)
	wesnoth.message (msg)
end



--[[

What is the probability that the player unit will run away from the AI unit

params should contain:
var1:            describe var1
var2:            describe var2
...
varN:            describe varN

]]--
this.getProbability_PlayerRunning = function (params)
	wesnoth.message ('getProbability_PlayerRunning is undefined!')
end



--[[

What is the probability that AI unit can engage the player unit
over (possibly) more than one turn?

params should contain:
var1:            describe var1
var2:            describe var2
...
varN:            describe varN

]]--
this.getProbability_CanEngage = function (params)
	wesnoth.message ('getProbability_CanEngage is undefined!')
end



--[[

What is the probability of a successful conversion event

params should contain:
var1:            describe var1
var2:            describe var2
...
varN:            describe varN

]]--
this.getProbability_CanConvert = function (params)
	wesnoth.message ('getProbability_CanConvert is undefined!')
end



--[[

Update the probability tables with a new datapoint

params should contain:
var1:            describe var1
var2:            describe var2
...
varN:            describe varN

]]--
this.update = function (params)
	wesnoth.message ('update is undefined!')
end



--[[

Store the current probability tables back to the filesystem

]]--
this.store = function ()
	wesnoth.message ('store is undefined!')
end


return this
