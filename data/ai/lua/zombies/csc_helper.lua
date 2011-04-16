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

Move a unit to a random location within its reach

params should contain:
unit:            The unit to move (unit object from wesnoth.get_units)
ai:              The AI performing the move

]]--
function this.move_randomly (params)
	local moves = wesnoth.find_reach (params.unit)

	-- pick a move at random
	local move_pick = math.random (1, table.getn (moves))
	local x, y = unpack (moves[move_pick])
	print ("HELPER: Picked random move: " .. move_pick .. " (" .. x .. ", " .. y .. ") of " .. table.getn (moves) .. " possible moves")
	-- get an error on moves to our own tile... so only move if
	-- one of the coords is different than unit position
	if x ~= params.unit.x or y~= params.unit.y then
		params.ai.move (params.unit, x, y)
	end
end


--[[

Get a table of enemy units within some distance of a given unit

params should contain:
unit:            The unit looking for enemies (unit object from wesnoth.get_units)
radius:          The maximum move distance to check within

Returns: list of units, possibly empty

]]--
function this.enemy_units_in_range (params)
	local all_units = wesnoth.get_units()
	local enemy_units = {}
	for i, unit in pairs (all_units) do
		print ("enemy_units_in_range: Checking side of unit, " .. unit.x .. ", " .. unit.y)
		if unit.side ~= params.unit.side then
			print (string.format ("enemy_units_in_range: Side (%d) not ours (%d)", unit.side, params.unit.side))
			table.insert (enemy_units, unit)
		else
			print ("enemy_units_in_range: One of us -- no add")
		end
	end

	local close_enemy_units = {}
	for i, unit in pairs (enemy_units) do
		print ("enemy_units_in_range: Checking range of unit, " .. unit.x .. ", " .. unit.y)
		local attack_points = this.get_empty_adjacencies (unit.x, unit.y)
		local path, distance = this.find_closest_location ({unit = params.unit, list = attack_points})
		--local path, distance = wesnoth.find_path (params.unit.x, params.unit.y, unit.x, unit.y)
		print (string.format ("enemy_units_in_range: path length = %d and distance = %d ", table.getn(path), distance))
		if distance <= params.radius then
			print ("enemy_units_in_range: Adding unit to list, " .. unit.x .. ", " .. unit.y)
			table.insert (close_enemy_units, unit)
		end
	end

	return close_enemy_units
end


--[[

Is this unit able to recruit more of itself... in general, and is there enough gold?

params should contain:
unit           The unit in question (unit object from wesnoth.get_units)

Returns: boolean

]]--
function this.can_recruit (params)
	local unit = params.unit
	local result = true
	result = result and unit.canrecruit
	result = result and wesnoth.sides[unit.side].gold >= wesnoth.unit_types[unit.type].cost
	return result
end


--[[

Get the closest keep to unit in question

params should contain:
unit           The unit in question (unit object from wesnoth.get_units)

Returns: path, cost, location table {x, y} numeric, not by name

]]--
function this.get_closest_keep (params)
	local unit = params.unit

	print ("HELPER: Keep locations:")
	local keep_locs = wesnoth.get_locations ({
		terrain    = "K*"
	});

	return this.find_closest_location ({unit = unit, list = keep_locs})
end



--[[

Get a table of close recruitable locations

params should contain:
unit           The unit in question (unit object from wesnoth.get_units)
radius         How close to look

Returns: table of locations {x, y} maybe empty

]]--
function this.get_close_recruit_locs (params)
	local unit = params.unit

	-- it's strange, but I don't seem to be able to combine
	-- the terrain filter with the x,y,radius filter.
	-- so we'll get two sets and compute intersection
	local keep_locs   = wesnoth.get_locations ({
		terrain = "K*"
	})
	local castle_locs = wesnoth.get_locations ({
		terrain = "C*"
	})
	local close_locs = wesnoth.get_locations ({
		x       = unit.x,
		y       = unit.y,
		radius  = params.radius
	})
	
	-- combine castle and keep locations into recruitable locs
	local r_locs = {}
	for x, l in pairs (keep_locs) do
		table.insert (r_locs, l)
	end
	for x, l in pairs (castle_locs) do
		table.insert (r_locs, l)
	end
	
	-- compute intersection of recruitable locs and close locs
	local cr_locs = {}
	for x, l in pairs (r_locs) do
		for y, m in pairs (close_locs) do
			local lx, ly = unpack (l)
			local mx, my = unpack (m)
			if lx == mx and ly == my then table.insert (cr_locs, l) end
		end
	end

	return cr_locs
end

function this.do_recruit (params)
	local unit = params.unit
	local ai   = params.ai

	print ("Recruit capable unit do_recruit'ing at: " .. unit.x .. ", " .. unit.y)

	-- can only recruit from a Keep location...
	local k_path, k_cost, k_keep = this.get_closest_keep ({unit = unit})

	-- only do anything if we actually got a best choice for keep
	if k_keep ~= nil then
		local x, y = unpack (k_keep)
		print ("Chose to move to keep  (" .. x .. ", " .. y .. ")")

		-- k_path is a table of single moves (not necessarily unit cost)
		-- first move in the table is ALWAYS our current position
		-- don't execute that move
		table.remove (k_path, 1)
		local ok = true  -- set to false first failure... maxed out move ability in other words
		for i, move in pairs (k_path) do
			if ok then
				local x, y = unpack (move)
				print ("\tk_path: moving to (" .. x .. ", " .. y .. ")")
				local move_result = ai.move (unit, x, y)
				ok = move_result.ok
			end
		end

		-- did we make it to the location?
		if unit.x == x and unit.y == y then
			-- can recruit onto any Castle location, but should choose
			-- one that's close to provide believability
			print ("Got to the keep!")
			print ("Recruitable locations within 10 tiles of the current location..")
			local cr_locs = this.get_close_recruit_locs ({unit = unit, radius = 10})

			-- don't have to be clever about this next part
			-- just try to recruit everywhere that it's possible
			-- right now, we don't really care so much whether
			-- there is an error... we can fix that later
			for x, l in pairs (cr_locs) do
				local lx, ly = unpack (l)
				print ("\trecruiting at: (" .. lx .. ", " .. ly .. ")")
				ai.recruit ("Zombie", lx, ly)
			end
		end
	end
end



function this.get_empty_adjacencies (x, y)
	local adjacencies = wesnoth.get_locations ({
		x      = x,
		y      = y,
		radius = 1
	})
	local empty_adj = {}
	for i, loc in pairs (adjacencies) do
		local x, y = unpack (loc)
		if this.is_tile_vacant (x, y) then
			table.insert (empty_adj, {x, y})
		end
	end
	return empty_adj
end


function this.is_tile_vacant (x, y)
	-- find_vacant_tile gets the nearest empty tile
	-- only add to the list if that nearest empty tile is the
	-- one we passed into find_vacant_tile
	local vx, vy = wesnoth.find_vacant_tile (x, y)
	return vx ~= nil and vy ~= nil  and vx == x and vy == y
end


--[[

Moves to an adjacent tile, then attacks

params should contain:
unit           The unit doing the attacking (unit object from wesnoth.get_units)
enemy          The enemy unit being attacked
ai:            The AI performing the move

]]--
function this.move_and_attack (params)
	local unit  = params.unit
	local enemy = params.enemy

	print ("move_and_attack: anfangen")

	-- Find vacant tiles near the enemy
	local empty_adj = this.get_empty_adjacencies (enemy.x, enemy.y)

	-- find the "closest" location
	local path, cost, loc = this.find_closest_location ({unit = unit, list = empty_adj})
	
	if loc~= nil and path ~= nil and table.getn (path) > 0 then

		--print ("\tmove_and_attack: find path from " .. unit.x .. ", " .. unit.y .. "   to   " .. enemy.x .. ", " .. enemy.y)
		--local path = wesnoth.find_path (unit.x, unit.y, enemy.x, enemy.y)
		print ("\tmove_and_attack: path length = " .. table.getn (path))

		--local eloc = table.remove (path, table.getn (path)) -- remove enemy location
		local uloc = table.remove (path, 1) -- remove unit location

		--print ("\tmove_and_attack: eloc = " .. eloc[1] .. ", " .. eloc[2])
		print ("\tmove_and_attack: uloc = " .. uloc[1] .. ", " .. uloc[2])

		local ok = true
		while table.getn (path) > 0 and ok == true do
			local x, y = unpack (path[1])
			print ("\tmove_and_attack: move = " .. x .. ", " .. y)
			local result = params.ai.move (unit, x, y)
			ok = result.ok
			print (string.format ("\tmove_and_attack: ok = %s", this.to_str (ok)))
			if ok then table.remove (path, 1) end
		end

		if table.getn (path) == 0 then
			params.ai.attack (unit.x, unit.y, enemy.x, enemy.y)
		end
	end

	print ("move_and_attack: beenden")
end


function this.find_closest_location (params)
	local list         = params.list
	local unit         = params.unit
	
	print (string.format ("find_closest_location: list size: %d unit: (%d, %d)", table.getn (list), unit.x, unit.y))

	-- variables to return
	local best_path = nil  -- returned
	local best_cost = nil  -- returned
	local best_loc  = nil  -- returned

	for i, loc in ipairs (list) do
		local x, y = unpack (loc)
		local path, cost = wesnoth.find_path (unit.x, unit.y, x, y)
		local change_str = ''
		if best_cost == nil or cost < best_cost then
			best_cost  = cost
			best_path  = path
			best_loc   = loc
			change_str = '  --  best CSF!'
		end
		print ("\t(" .. x .. ", " .. y .. ") costs " .. cost .. change_str)
	end
	return best_path, best_cost, best_loc
end


function this.to_str (bool)
	if bool then return "true" else return "false" end
end

return this
