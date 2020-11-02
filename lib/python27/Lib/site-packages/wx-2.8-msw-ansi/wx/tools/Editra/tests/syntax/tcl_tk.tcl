# Syntax Highlighting Test File for TCL/TK
# Comments are like this
# Hello World in tcl/tk

wm title . "Hello world!"

frame .h -borderwidth 2
frame .q -borderwidth 2
button .h.hello -text "Hello world" \
        -command "puts stdout \"Hello world!\"" -cursor gumby
button .q.quit -text "Quit" -command exit -cursor pirate

pack .h -side left
pack .q -side right
pack .h.hello
pack .q.quit

# Procedure Definition
proc printArguments args {
   foreach arg $args {
      puts $arg
   }
}

proc foo::xxxx {} {
    set a xxxxxxxxxxxx
}
proc foo_bar {} {
    set b xxxxxxx
}

proc foo::yyyy {} {
    set a bbbbbbb
}
