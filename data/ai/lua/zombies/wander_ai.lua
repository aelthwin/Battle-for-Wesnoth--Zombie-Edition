local this = {}

--[[
Trevor's notes...

	wesnoth.* functions defined in src/scripting/lua.cpp
	ai.*      functions defined in src/ai/lua/core.cpp

	plenty of example lua code floating around in the codebase
	if you do a grep for "get_units"... found this while trying to discover
	where get_units was defined... turns out it's in lua.cpp (see above)

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



--[[ Variables set by init ... with some default values  :) ]]--
this.side      = 0
this.ai        = nil



--[[

Initialize the AI engine.  This will only be run once!

]]--
function this.init (ai)
	this.ai = ai
	this.side = this.ai.side -- for convenience
	math.randomseed (os.time())
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
		-- try recruiting first...
		-- TLB @TODO add gold check
		if unit.canrecruit then
			print ("Recruit capable unit at: " .. unit.x .. ", " .. unit.y);
			-- can only recruit from a Keep location...
			local keep_locs = wesnoth.get_locations ({
				terrain    = "K*"
			});
			print ("Keep locations:\n")
			local best_path = nil
			local best_cost = nil
			local best_idx = nil
			for i, loc in ipairs (keep_locs) do
				local x, y = unpack (loc)
				local path, cost = wesnoth.find_path (unit.x, unit.y, x, y, {ignore_units = true})
				local change_str = ''
				if best_cost == nil or cost < best_cost then
					best_cost = cost
					best_path = path
					best_idx = i
					change_str = '  --  best CSF!'
				end
				print ("\t(" .. x .. ", " .. y .. ") costs " .. cost .. change_str)
			end

			-- only do anything if we actually got a best choice for keep
			if best_idx ~= nil then
				local x, y = unpack (keep_locs[best_idx])
				print ("Chose to move to keep " .. best_idx .. " (" .. x .. ", " .. y .. ")")

				-- best_path is a table of single moves (not necessarily unit cost)
				-- first move in the table is ALWAYS our current position
				-- don't execute that move
				table.remove (best_path, 1)
				local ok = true  -- set to false first failure... maxed out move ability in other words
				for i, move in pairs (best_path) do
					if ok then
						local x, y = unpack (move)
						print ("\tbest_path: moving to (" .. x .. ", " .. y .. ")")
						local move_result = this.ai.move (unit, x, y)
						ok = move_result.ok
					end
				end


				-- did we make it to the location?
				if unit.x == x and unit.y == y then
					-- can recruit onto any Castle location, but should choose
					-- one that's close to provide believability
					print ("Got to the keep!")
					print ("Castle locations within 10 tiles of the current location..")
					-- it's strange, but I don't seem to be able to combine
					-- the terrain filter with the x,y,radius filter.
					-- so we'll get two sets and compute intersection
					local castle_locs = wesnoth.get_locations ({
						terrain = "C*"
					})
					local close_locs = wesnoth.get_locations ({
						x       = unit.x,
						y       = unit.y,
						radius  = 10
					})
					
					-- combine castle and keep locations
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
					print ("close recruitable locations:")
					for x, l in pairs (cr_locs) do
						print ("\t(" .. x .. ", " .. y .. ")")
					end

					-- don't have to be clever about this next part
					-- just try to recruit everywhere that it's possible
					-- right now, we don't really care so much whether
					-- there is an error... we can fix that later
					for x, l in pairs (cr_locs) do
						local lx, ly = unpack (l)
						this.ai.recruit ("Zombie", lx, ly)
					end

				end




				-- recruited!  done...
				print ("End WANDER do_moves (after recruit)")
				--return
			end
			
			-- didn't do anything ... fall through to next stage
		end

		-- Fall back on random movement
		this.move_randomly (unit)
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

function this.move_randomly (unit)
	local moves = wesnoth.find_reach (unit)

	-- pick a move at random
	local move_pick = math.random (1, table.getn (moves))
	x, y = unpack (moves[move_pick])
	print ("WANDER: Picked random move: " .. move_pick .. " (" .. x .. ", " .. y .. ") of " .. table.getn (moves) .. " possible moves")
	-- get an error on moves to our own tile... so only move if
	-- one of the coords is different than unit position
	if x ~= unit.x or y~= unit.y then
		this.ai.move (unit, x, y)
	end
end

return this
