# fetching symbol files takes forever
set remotetimeout 300

# Allow pending breakpoints by default
set breakpoint pending on

# Turn off acks for a slight performance improvement (the underlying
# protocol is reliable so we don't need them).
set remote noack-packet on

# also, we support "monitor eval [expr]" but it's not
# convenient to wrap in a user-defined function 

define as3bt
        avm2-select-thread-for-bt
	monitor bt
end

define as3sync
        avm2-select-thread-for-continue
        set confirm no
        handle SIGTRAP stopnoprint
	# cause 'continue' to be a no-op
	monitor setContinueActionNop
	# but gdb thinking state may have changed causes it to reread regs, etc.
	continue
        handle SIGTRAP print
        set confirm yes
end

define as3up
        avm2-select-thread-for-bt
	monitor up
	as3sync
	as3bt
end

define as3down
        avm2-select-thread-for-bt
	monitor down
	as3sync
	as3bt
end

define as3finish
        avm2-select-thread-for-continue
        set confirm no
        handle SIGTRAP stopnoprint
	monitor setContinueActionOut
	continue
        handle SIGTRAP print
        set confirm yes
	as3bt
end

define as3step
        avm2-select-thread-for-continue
        set confirm no
        handle SIGTRAP stopnoprint
	monitor setContinueActionInto
	continue
        handle SIGTRAP print
        set confirm yes
	as3bt
end

define as3next
        avm2-select-thread-for-continue
        set confirm no
        handle SIGTRAP stopnoprint
	monitor setContinueActionOver
	continue
        handle SIGTRAP print
        set confirm yes
	as3bt
end

define as3locals
        avm2-select-thread-for-bt
	monitor locals
end

define as3args
        avm2-select-thread-for-bt
	monitor args
end

