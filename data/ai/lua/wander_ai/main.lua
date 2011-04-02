local this = {}


function this.init (ai)
	this.ai = ai
end

function this.echo (msg)
	wesnoth.message (msg)
end

function this.do_moves ()
       --! Most of this comes from scenario-lua-ai.cfg
       --! plenty of example lua code floating around in the codebase
       --! if you do a grep for "get_units"... found this while trying to discover
       --! where get_units was defined... turns out it's in lua.cpp (see above)

       --! Used this next line to verify that the do_moves function was called
       --! by the stage configuration below... it worked  :)
       --! wesnoth.message ('doing moves!')
	this.echo ('echoing!')


       units = wesnoth.get_units({canrecruit = true, side = this.ai.side})
       --! move (partial move)
       --! ai.move(units[1],13, 22)
       --! full move. note that the my_leader still can be used altrough x and y are now different.
       --! ai.move_full(units[1], 11, 23)

       --! attack with auto weapon/aggression
       --! ai.attack(2, 12, 3, 12)
       --! attack with weapon selected
       --! ai.attack(3, 11, 3, 12, 1)
       --! attack with different aggression
       --! ai.attack(3, 13, 3, 12, -1, 0.9)
end

return this
