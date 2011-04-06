local this = {}

-- wesnoth.* functions defined in src/scripting/lua.cpp
-- ai.*      functions defined in src/ai/lua/core.cpp



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




return this
