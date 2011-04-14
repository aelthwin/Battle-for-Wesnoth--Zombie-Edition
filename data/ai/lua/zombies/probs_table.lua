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
this.precalc_survival = {
	yes = {
		strength = {
			low = 7,   -- count of instances in table when survive? = yes and strength = low
			med = 5,   -- count of instances in table when survive? = yes and strength = med
			high = 2   -- count of instances in table when survive? = yes and strength = high
		},
		race = {
			human = 5,
			orc = 9
		},
		health = {
			low = 5,
			med = 3,
			high = 1
		},
		total = 14
	}.

	no = {
		strength = {
			low = 2,
			med = 3,
			high = 5
		},
		race = {
			human = 7,
			orc = 3
		},
		health = {
			low = 1,
			med = 3,
			high = 6
		},
		total = 10
	},

	total = 24,
}



--[[

Loads up the probability tables, does pre-calculations

]]--
this.init = function ()
	this.table_engagement_survival = table.load("engagementSurvival.tbl")
	this.table_engagement          = table.load("engagement.tbl")
	this.table_player_runs         = table.load("playerRuns.tbl")

	-- do precalculations here...
	-- @TODO

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
	wesnoth.message ('update is fake!')
	this.updatePlayerRunningProbabilityTable ();
	this.updateEngagementProbabilityTable ();
	this.updateEngagementSurvivalProbabilityTable ();
end



--[[

Store the current probability tables back to the filesystem

]]--
this.store = function ()
	table.save(this.table_player_runs, "playerRuns.tbl")
	table.save(this.table_engagement, "engagement.tbl")
	table.save(this.table_engagement_survival, "engagementSurvival.tbl")
end




--Kenny coded below


--requires 
--	z zombie units within pursuit radius
--	sp speed of player unit
-- 	d mean distance from close zombie center of gravity
--	str player strength
--	f number of fellow player units within pursuit radius
--	r race of player unit
this.updatePlayerRunningProbabilityTable = function ()
	print ("Stubbed out probability table in updatePlayerRunningProbabilityTable")
	this.table_player_runs = {
		{zombies = 1, speed = 4, distance = 9, strength = 2, fellows = 2, race = human},
		{zombies = 0, speed = 5, distance = 8, strength = 3, fellows = 3, race = elf},
	}
end

--requires
--	d distance from player unit
--	sp speed of player unit
--	r race of player unit
this.updateEngagementProbabilityTable = function ()
	print ("Stubbed out probability table in updateEngagementProbabilityTable")
	this.table_engagement = {
		{distance = 15, speed = 4, race = human},
		{distance = 10, speed = 7, race = elf}
	}
end

--requires
--	str player unit strength
--	r race of player unit
--	h health of player unit
this.updateEngagementSurvivalProbabilityTable = function ()
	print ("Stubbed out probability table in updateEngagementSurvivalProbabilityTable")
	this.table_engagement_survival = {
		{strength = 7, race = human, health = 100},
		{strength = 8, race = human, health = 99}
	}
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


return this
