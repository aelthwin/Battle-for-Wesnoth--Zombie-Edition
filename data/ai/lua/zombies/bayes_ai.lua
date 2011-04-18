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


--[[ Variables set by init ... with some default values  :) ]]--
this.side      = 0
this.ai        = nil
this.modes     = {}   -- will map unit id to mode 
this.targets   = {}   -- will map unit id to target id


this.chase_threshold = 0.25 -- total guess for now -- will require tuning

--[[ Modes ]]--
local PURSUIT  = 1
local WANDER   = 0


--[[

Initialize the AI engine.  This will only be run once!

]]--
function this.init (ai)
	this.ai = ai
	this.side = this.ai.side -- for convenience
	this.probs.init ()
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
	-- make sure some class level vars are there
	this.units = wesnoth.get_units ({side=this.ai.side})
	this.leader_id = this.leader_id or this.units[1].id -- only set once

	
	-- print out some state information...
	print ("")
	print ("")
	print ("")
	print ("")
	print ("TEAM:         " .. wesnoth.sides[this.side].team_name)
	print ("Zombie gold:  " .. wesnoth.sides[this.side].gold)
	print ("Recruit cost: " .. wesnoth.unit_types[this.units[1].type].cost)
	print ("Race:         " .. wesnoth.unit_types[this.units[1].type].race)
	print ("Move rate:    " .. this.units[1].max_moves)
	print ("------------------------------------------");
	print ("Start BAYES do_moves")


	for i, unit in ipairs (this.units) do

		-- Initialization does:
		-- 1. print status
		-- 2. make sure unit indexes are present in this.modes and this.targets
		-- 3. puts a unit back into WANDER if its target is gone
		print (string.format ("\nUnit %d, %d (%s) (%s type) doing moves", unit.x, unit.y, unit.id, unit.type))
		this.init_unit (unit)


		-- Action pipeline...
		local continue = true

		-- try recruiting?
		print ("BAYES: recruiting stage ---")
		if continue and this.helper.can_recruit ({unit = unit}) then
			local recruited = this.helper.do_recruit ({
				unit = unit,
				ai   = this.ai
			})
			-- recruited?
			-- if so, don't do anything apart from recruiting
			continue = not recruited
		end -- RECRUITING PHASE



		-- try to find a good target for WANDERers?
		print ("BAYES: candidate finding stage ---")
		if continue and this.modes[unit.id] == WANDER then
			local best_unit_id = nil
			local best_prob    = 0.0
			local close_units  = this.helper.enemy_units_in_range ({unit = unit, radius = 10})

			-- filter out non-candidates
			print ("BAYES: filtering out non-candidates")
			local temp_close_units = {}
			for i, e_unit in pairs (close_units) do
				if
				not e_unit.petrified -- can't smell petrifieds... they smell like rocks |-(
				then
					table.insert (temp_close_units, e_unit)
				end
			end
			close_units = temp_close_units

			-- find best close candidate, if there are any at all that is
			print ("BAYES: finding best of " .. table.getn(close_units) .. " candidates")
			if table.getn(close_units) then
				for i, e_unit in pairs (close_units) do

					-- calculate params for probs
					local zombies, distances = this.helper.enemies_that_can_attack ({unit = e_unit, distance = 10})

					-- get proportional probs
					-- in this circumstance, player unit == e_unit
					local params = {
						unit     = unit,
						target   = e_unit,
						-- probabilities inputs
						z   = table.getn (zombies),
						sp  = e_unit.max_moves,
						d   = this.helper.distance_from (unit, e_unit),
						str = wesnoth.unit_types[e_unit.type].max_attack,
						f   = table.getn (close_units),
						r   = wesnoth.unit_types[e_unit.type].race,
						h   = e_unit.hitpoints
					}
					print ("PARAMS TO PROBS")
					table.foreach (params, print)

					local ppr, pce, pcc = this.probs.get_all_probs ({
						unit     = unit,
						target   = e_unit,
						-- probabilities inputs
						z   = table.getn (zombies),
						sp  = e_unit.max_moves,
						d   = this.helper.distance_from (unit, e_unit),
						str = wesnoth.unit_types[e_unit.type].max_attack,
						f   = table.getn (close_units),
						r   = wesnoth.unit_types[e_unit.type].race,
						h   = e_unit.hitpoints
					})
			
					-- combine
					local combined_probs = ppr * pce * pcc
			
					-- compare
					if combined_probs > best_prob then
						best_prob    = combined_probs
						best_unit_id = e_unit.id
					end
				end

				-- should we pursue best candidate?
				if best_prob > this.chase_threshold then
					this.to_pursuit_mode ({
						unit      = unit,
						target_id = best_unit_id
					})
				end
			end
		end


		-- if in pursuit mode, try chase and attack?
		-- don't need to check for this.targets[unit.id] ~= nil
		-- because we can guarantee that if we take the branch
		-- that it's something real, an enemy actually on the map
		print ("BAYES: pursuit stage ---")
		if continue and this.modes[unit.id] == PURSUIT then
			-- do the attack
			-- this.helper.move_and_attack ({
			this.helper.move_and_attack2 ({
				unit  = unit,
				enemy = this.helper.unit_for_id (this.targets[unit.id]),
				ai    = this.ai
			})

			-- is the unit converted?
			local enemy = this.helper.unit_for_id (this.targets[unit.id])
			if enemy ~= nil and enemy.side == unit.side then
				this.to_wander_mode ({
					unit = unit
				})
			-- no?  might be pursuing and not caught him yet or attacked but didn't convert
			-- prevent any random movement
			else
				continue = false
			end
		end


		-- Fall back on random movement
		print ("BAYES: wander stage ---")
		if continue then
			this.helper.move_randomly ({
				unit = unit,
				ai   = this.ai
			})
		end
		
		print ("\nUnit " .. unit.x .. ", " .. unit.y .. "  (" .. unit.id .. ") DONE")
	end

	print ("doing results...")
	this.do_results ()
	print ("DONE with results...")

	print ("End BAYES do_moves")
	print ("------------------------------------------");
	return
end



function this.init_unit (unit)
	this.modes[unit.id]   = this.modes[unit.id] or WANDER
	this.targets[unit.id] = this.targets[unit.id] or nil

	if  this.modes[unit.id] == PURSUIT and this.helper.unit_for_id (this.targets[unit.id]) == nil then
		this.to_wander_mode ({
			unit = unit
		})
	end
end


function this.to_wander_mode (params)
	this.modes[params.unit.id]   = WANDER
	this.targets[params.unit.id] = nil
end


function this.to_pursuit_mode (params)
	this.modes[params.unit.id]   = PURSUIT
	this.targets[params.unit.id] = params.target_id
end



--[[

Figure out what results, if any, need to be written back to the probs table

]]--
function this.do_results ()
	-- figure results
	local results = {}

	-- then store
	for i, result in pairs (results) do
		this.probs.update (result)
	end
	this.probs.store ()
end

return this
