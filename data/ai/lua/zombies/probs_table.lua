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


this.table_engagement_survival = {}
this.table_engagement = {}
this.table_player_runs = {}


this.schema = {
	questions = {
		will_run = {
			answers    = {"yes", "no"},
			attributes = {
				"zombies",
				"speed",
				"distance",
				"strength",
				"fellows",
				"race"
			}
		},
		can_engage = {
			answers    = {"yes", "no"},
			attributes = {
				"distance",
				"speed",
				"race"
			}
		},
		will_survive = {
			answers    = {"yes", "no"},
			attributes = {
				"strength",
				"race",
				"health"
			}
		}
	},
	attributes = {
		strength = {
			values  = {"low", "med", "high"},
			to_attr = function (value)
				if     value < 5  then return "low"
				elseif value < 10 then return "med"
				else                   return "high"
				end
			end
		},
		race = {
			values  = {}, --[[ must be preloaded using wesnoth.get_units ]]
			to_attr = function (value)
				return value
			end
		},
		health = {
			values  = {"low", "med", "high"},
			to_attr = function (value) -- value should be in hit points
				if     value < 25 then return "low"
				elseif value < 50 then return "med"
				else                   return "high"
				end
			end
		},
		distance = {
			values = {"low", "med", "high"},
			to_attr = function (value) -- value in # of moves
				if     value < 6  then return "low"
				elseif value < 10 then return "med"
				else                   return "high"
				end
			end
		},
		speed = {
			values = {"low", "med", "high"},
			to_attr = function (value) -- value in # of moves per turn
				if     value < 3  then return "low"
				elseif value < 6  then return "med"
				else                   return "high"
				end
			end
		},
		zombies = {
			values = {"low", "med", "high"},
			to_attr = function (value) -- # of zombies for whom unit is in pursuit range
				if value == nil then value = 0 end
				if     value < 2  then return "low"
				elseif value < 4  then return "med"
				else                   return "high"
				end
			end
		},
		fellows = {
			values = {"low", "med", "high"},
			to_attr = function (value) -- # of fellow units within zombie pursuit range
				if     value < 2  then return "low"
				elseif value < 4  then return "med"
				else                   return "high"
				end
			end
		},
	}
}
--[[
Example of this.precalc_survival table in action...

supppose looking at enemy and we discover the tuple:
(s = low, r = orc, h = med)

I think the bayes rule calc for a given term should end up being like:
p_s_low_given_yes = this.precalc_survival.yes.strength.low / this.precalc_survival.yes.total
p_r_orc_given_yes = this.precalc_survival.yes.race.orc / this.precalc_survival.yes.total
p_h_med_given_yes = this.precalc_survival.yes.health.med / this.precalc_survival.yes.total
p_yes = this.precalc_survival.yes.total / this.precalc_survival.total

p_survive = this.p_s_low_given_yes * p_r_orc_given_yes * p_h_med_given_yes * p_yes

p_s_low_given_no = this.precalc_survival.no.strength.low / this.precalc_survival.no.total
p_r_orc_given_no = this.precalc_survival.no.race.orc / this.precalc_survival.no.total
p_h_med_given_no = this.precalc_survival.no.health.med / this.precalc_survival.no.total
p_no = this.precalc_survival.no.total / this.precalc_survival.total

p_not_survive = this.p_s_low_given_no * p_r_orc_given_no * p_h_med_given_no * p_no


At which point a normal bayes classifier would compare p_survive with p_not_survive to classify
this as either a surviving scenario or a not surviving.

We'll do something a little different... 

getProbability_CanConvert will return     p_survive / p_not_survive
this.getProbability_CanEngage will return p_engage / p_not_engage
getProbability_PlayerRunning will return  p_not_run / p_run  -- backward because we are greedy and would rather engage non-running enemies

then we'll weight each return value, sum it, and compare to a threshold
if the threshold passed, then we attack.


Why keep totals and recalculate the quotient every time?

We would ultimately like to be able to learn not just game to game, but in real time.  If we do it this way, at first we would
read in and calculate once... and never touch these again until we read in the tables the next game.  Those second-game tables
would or course be our first-game tables, but changed throughout the game... so game-to-game learning occurs.

Leaving the precalc's this way would make it relatively straightforward to update not only the raw tables, but also the counts
as new data comes in... think about it.  Let's say a new data point is passed to update like
(survive = yes, s = low, r = orc, h = med)
then we can say:
this.precalc_survival.total ++  -- not valid lua syntax
this.precalc_survival.strength.low ++
this.precalc_survival.race.orc ++
this.precalc_survival.health.med ++

and if that update removed an old data point that looked like
(survive = yes, s = high, r = human, h = low)
then we can say:
this.precalc_survival.total --  -- not valid lua syntax
this.precalc_survival.strength.high --
this.precalc_survival.race.human --
this.precalc_survival.health.low --

And then our probabilities have been updated in real-time.

We shouldn't start by doing this right away though... because it opens us up for bugs.  Don't want that when we're cutting it so close.
]]--

this.precalc_p_runs     = {}
this.precalc_engage     = {}
this.precalc_survival   = {}

--[[

Loads up the probability tables, does pre-calculations

]]--
this.init = function ()
	this.table_engagement_survival = table.load("engagementSurvival.tbl")
	this.table_engagement          = table.load("engagement.tbl")
	this.table_player_runs         = table.load("playerRuns.tbl")

	-- init race values in schema
	this.debug ("Unit Types:")
	local temp = {}
	for t, ut in pairs (wesnoth.unit_types) do
		local found = false
		for _, r in pairs (temp) do
			if r == ut.race then found = true end
		end
		if not found then table.insert (temp, ut.race) end
	end
	this.debug (table.tostring (temp))
	this.schema.attributes.race.values = temp
	this.debug (table.tostring (this.schema))

	-- init precalcs
	this.init_precalc ("p_runs",     "will_run")
	this.init_precalc ("engage",     "can_engage")
	this.init_precalc ("survival",   "will_survive")

	-- do precalculations
	this.do_precalc ("p_runs",     "will_run",     this.table_player_runs)
	this.do_precalc ("engage",     "can_engage",   this.table_engagement)
	this.do_precalc ("survival",   "will_survive", this.table_engagement_survival)

	this.debug (table.tostring (this.precalc_engage))
	this.debug (table.tostring (this.precalc_engage))
	this.debug (table.tostring (this.precalc_survival))
end


this.debug = function (str)
	-- change this to turn off DEBUG
	if true then print (str) end
end


this.init_precalc = function (table, question)
	table = "precalc_" .. table
	this[table] = {}
	this.debug (table .. " initializing")
	for i, answer in pairs (this.schema.questions[question].answers) do
		this.debug ("\tAdd answer " .. answer)
		this[table][answer] = {}
		for j, attr in pairs (this.schema.questions[question].attributes) do
			this.debug ("\t\tAdd attribute " .. attr)
			this[table][answer][attr] = {}
			for k, av in pairs (this.schema.attributes[attr].values) do
				this.debug ("\t\t\tAdd value " .. av)
				this[table][answer][attr][av] = 0
			end
		end
		this.debug ("\t\tAdd total")
		this[table][answer]["total"] = 0
	end
	this.debug ("\tAdd total")
	this[table]["total"] = 0
end


this.do_precalc = function (table, question, records)
	table = "precalc_" .. table
	this.debug (table .. " pre-calculating")
	for i, record in pairs (records) do
		this.debug ("\tAdd record " .. i)
		if record[question] ~= nil and this[table][record[question]] ~= nil then
			this.debug ("\t\tAttributes:")
			local answer = record[question] -- value in the "question" index of the record
			for j, attr in pairs (this.schema.questions[question].attributes) do
				local value = record[attr]
				local bumped = false
				if this[table][answer][attr] ~= nil and this[table][answer][attr][value] ~= nil then
					this[table][answer][attr][value] = this[table][answer][attr][value] + 1
					bumped = true
				end
				local b_str = "X"
				if bumped then b_str = "+" end
				this.debug (string.format ("\t\t\t%s = %s (%s)", attr, value, b_str))
			end
			this[table][answer].total = this[table][answer].total + 1
		end
		this[table].total = this[table].total + 1
	end
end


--[[

Simple test function to make sure that we can call this object

]]--
this.echo = function (msg)
	wesnoth.message (msg)
end


this.get_all_probs = function (params)
	-- discretize values in params
	params.z   = this.schema.attributes.zombies.to_attr  (params.z)
	params.sp  = this.schema.attributes.speed.to_attr    (params.sp)
	params.d   = this.schema.attributes.distance.to_attr (params.d)
	params.str = this.schema.attributes.strength.to_attr (params.str)
	params.f   = this.schema.attributes.fellows.to_attr  (params.f)
	params.r   = this.schema.attributes.race.to_attr     (params.r)
	params.h   = this.schema.attributes.health.to_attr   (params.h)

	return
		this.getProbability_PlayerRunning (params),
		this.getProbability_CanEngage     (params),
		this.getProbability_CanConvert    (params)
end


this.get_ind_prob = function (table, answer, attr, value)
	table = "precalc_"..table
	if attr ~= nil and value ~= nil then
		return this[table][answer][attr][value] / this[table][answer].total
	end
	return this[table][answer] / this[table].total
end




--[[

What is the probability that the player unit will run away from the AI unit

params should contain:
z:   # of zombies who have the unit in pursuit radius
sp:  max_moves of the unit per turn
d:   distance unit is from zombie
str: max attack strength of the unit
f:   number of fellow units in the zombies pursuit radius
r:   unit race

]]--
this.getProbability_PlayerRunning = function (params)
	local answer = "no"
	return
		this.get_ind_prob ("p_runs", answer, "zombies",  params.z) *
		this.get_ind_prob ("p_runs", answer, "speed",    params.sp) *
		this.get_ind_prob ("p_runs", answer, "distance", params.d) *
		this.get_ind_prob ("p_runs", answer, "strength", params.str) *
		this.get_ind_prob ("p_runs", answer, "fellows",  params.f) *
		this.get_ind_prob ("p_runs", answer, "race",     params.r) *
		this.get_ind_prob ("p_runs", answer)
end



--[[

What is the probability that AI unit can engage the player unit
over (possibly) more than one turn?

params should contain:
sp:  max_moves of the unit per turn
d:   distance unit is from zombie
r:   unit race

]]--
this.getProbability_CanEngage = function (params)
	local answer = "yes"
	return
		this.get_ind_prob ("engage", answer, "speed",    params.sp) *
		this.get_ind_prob ("engage", answer, "distance", params.d) *
		this.get_ind_prob ("engage", answer, "race",     params.r) *
		this.get_ind_prob ("engage", answer)
end



--[[

What is the probability of a successful conversion event

params should contain:
str: unit max attack
r:   unit race
h:   unit health

]]--
this.getProbability_CanConvert = function (params)
	local answer = "yes"
	return
		this.get_ind_prob ("survival", answer, "strength", params.str) *
		this.get_ind_prob ("survival", answer, "race",     params.r) *
		this.get_ind_prob ("survival", answer, "health",   params.h) *
		this.get_ind_prob ("survival", answer)
end



--[[

Update the probability tables with a new datapoint

params is a datapoint, and should contain:
var1:            describe var1
var2:            describe var2
...
varN:            describe varN

]]--
this.update = function (params)
	this.updatePlayerRunningProbabilityTable (params);
	this.updateEngagementProbabilityTable (params);
	this.updateEngagementSurvivalProbabilityTable (params);
end



--[[

Store the current probability tables back to the filesystem

]]--
this.store = function ()
	table.save(this.table_player_runs, "playerRuns.tbl")
	table.save(this.table_engagement, "engagement.tbl")
	table.save(this.table_engagement_survival, "engagementSurvival.tbl")
end




--requires 
--	z zombie units within pursuit radius
--	sp speed of player unit
-- 	d distance of zombie from player unit
--	str player strength
--	f number of fellow player units within pursuit radius
--	r race of player unit
this.updatePlayerRunningProbabilityTable = function (params)
	table.insert(this.table_player_runs, {
		will_run = params.will_run,
		zombies  = params.z,
		speed    = params.sp,
		distance = params.d,
		strength = params.str,
		fellows  = params.f,
		race     = params.r
	})
end

--requires
--	d distance of zombie from player unit
--	sp speed of player unit
--	r race of player unit
this.updateEngagementProbabilityTable = function ()
	table.insert(this.table_engagement, {
		can_engage = params.can_engage,
		distance   = params.d,
		speed      = params.sp,
		race       = params.r
	})
end

--requires
--	str player unit strength
--	r race of player unit
--	h health of player unit
this.updateEngagementSurvivalProbabilityTable = function ()
	table.insert(this.table_engagement_survival, {
		will_survive = params.will_survive,
		strength     = params.str,
		race         = params.r,
		health       = params.h
	})
end



-- The following code is from http://lua-users.org/wiki/SaveTableToFile and is freely available.  Its purpose is reading and writing tables to/from a file.  
   -- declare local variables
   --// exportstring( string )
   --// returns a "Lua" portable version of the string
function exportstring( s )
      s = string.format( "%q",s )
      -- to replace
      s = string.gsub( s,"\\\n","\\n" )
      s = string.gsub( s,"\r","\\r" )
      s = string.gsub( s,string.char(26),"\"..string.char(26)..\"" )
      return s
   end
--// The Save Function
function table.save(  tbl,filename )
   local charS,charE = "   ","\n"
   local file,err
   -- create a pseudo file that writes to a string and return the string
   if not filename then
      file =  { write = function( self,newstr ) self.str = self.str..newstr end, str = "" }
      charS,charE = "",""
   -- write table to tmpfile
   elseif filename == true or filename == 1 then
      charS,charE,file = "","",io.tmpfile()
   -- write table to file
   -- use io.open here rather than io.output, since in windows when clicking on a file opened with io.output will create an error
   else
      file,err = io.open( filename, "w" )
      if err then return _,err end
   end
   -- initiate variables for save procedure
   local tables,lookup = { tbl },{ [tbl] = 1 }
   file:write( "return {"..charE )
   for idx,t in ipairs( tables ) do
      if filename and filename ~= true and filename ~= 1 then
         file:write( "-- Table: {"..idx.."}"..charE )
      end
      file:write( "{"..charE )
      local thandled = {}
      for i,v in ipairs( t ) do
         thandled[i] = true
         -- escape functions and userdata
         if type( v ) ~= "userdata" then
            -- only handle value
            if type( v ) == "table" then
               if not lookup[v] then
                  table.insert( tables, v )
                  lookup[v] = #tables
               end
               file:write( charS.."{"..lookup[v].."},"..charE )
            elseif type( v ) == "function" then
               file:write( charS.."loadstring("..exportstring(string.dump( v )).."),"..charE )
            else
               local value =  ( type( v ) == "string" and exportstring( v ) ) or tostring( v )
               file:write(  charS..value..","..charE )
            end
         end
      end
      for i,v in pairs( t ) do
         -- escape functions and userdata
         if (not thandled[i]) and type( v ) ~= "userdata" then
            -- handle index
            if type( i ) == "table" then
               if not lookup[i] then
                  table.insert( tables,i )
                  lookup[i] = #tables
               end
               file:write( charS.."[{"..lookup[i].."}]=" )
            else
               local index = ( type( i ) == "string" and "["..exportstring( i ).."]" ) or string.format( "[%d]",i )
               file:write( charS..index.."=" )
            end
            -- handle value
            if type( v ) == "table" then
               if not lookup[v] then
                  table.insert( tables,v )
                  lookup[v] = #tables
               end
               file:write( "{"..lookup[v].."},"..charE )
            elseif type( v ) == "function" then
               file:write( "loadstring("..exportstring(string.dump( v )).."),"..charE )
            else
               local value =  ( type( v ) == "string" and exportstring( v ) ) or tostring( v )
               file:write( value..","..charE )
            end
         end
      end
      file:write( "},"..charE )
   end
   file:write( "}" )
   -- Return Values
   -- return stringtable from string
   if not filename then
      -- set marker for stringtable
      return file.str.."--|"
   -- return stringttable from file
   elseif filename == true or filename == 1 then
      file:seek ( "set" )
      -- no need to close file, it gets closed and removed automatically
      -- set marker for stringtable
      return file:read( "*a" ).."--|"
   -- close file and return 1
   else
      file:close()
      return 1
   end
end

--// The Load Function
function table.load( sfile )
   -- catch marker for stringtable
   if string.sub( sfile,-3,-1 ) == "--|" then
      tables,err = loadstring( sfile )
   else
      tables,err = loadfile( sfile )
   end
   if err then return _,err
   end
   tables = tables()
   for idx = 1,#tables do
      local tolinkv,tolinki = {},{}
      for i,v in pairs( tables[idx] ) do
         if type( v ) == "table" and tables[v[1]] then
            table.insert( tolinkv,{ i,tables[v[1]] } )
         end
         if type( i ) == "table" and tables[i[1]] then
            table.insert( tolinki,{ i,tables[i[1]] } )
         end
      end
      -- link values, first due to possible changes of indices
      for _,v in ipairs( tolinkv ) do
         tables[idx][v[1]] = v[2]
      end
      -- link indices
      for _,v in ipairs( tolinki ) do
         tables[idx][v[2]],tables[idx][v[1]] =  tables[idx][v[1]],nil
      end
   end
   return tables[1]
end
-- This is the end of the code from http://lua-users.org/wiki/SaveTableToFile


-- START code from http://lua-users.org/wiki/TableUtils
function table.val_to_str ( v )
  if "string" == type( v ) then
    v = string.gsub( v, "\n", "\\n" )
    if string.match( string.gsub(v,"[^'\"]",""), '^"+$' ) then
      return "'" .. v .. "'"
    end
    return '"' .. string.gsub(v,'"', '\\"' ) .. '"'
  else
    return "table" == type( v ) and table.tostring( v ) or
      tostring( v )
  end
end

function table.key_to_str ( k )
  if "string" == type( k ) and string.match( k, "^[_%a][_%a%d]*$" ) then
    return k
  else
    return "[" .. table.val_to_str( k ) .. "]"
  end
end

function table.tostring( tbl )
  local result, done = {}, {}
  for k, v in ipairs( tbl ) do
    table.insert( result, table.val_to_str( v ) )
    done[ k ] = true
  end
  for k, v in pairs( tbl ) do
    if not done[ k ] then
      table.insert( result,
        table.key_to_str( k ) .. "=" .. table.val_to_str( v ) )
    end
  end
  return "{" .. table.concat( result, "," ) .. "}"
end
-- END code from http://lua-users.org/wiki/TableUtils

return this
