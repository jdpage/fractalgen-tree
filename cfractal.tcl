#!/usr/bin/env wish8.5

package require Tk
package require tkpng

load [file join [file dirname [info script]] cfractal.so]

set img [image create photo -palette grayscale]
set size 500
set branch 4
set level 7
set seed "time"

proc buildui {} {
	global img size branch level seed
	label .view -image $img
	
	ttk::label .lsize -text "Size:"
	ttk::entry .esize -textvariable size
	
	ttk::label .lbranch -text "Branches:"
	ttk::entry .ebranch -textvariable branch
	
	ttk::label .llevel -text "Level:"
	ttk::entry .elevel -textvariable level
	
	ttk::label .lseed -text "Seed:"
	ttk::entry .eseed -textvariable seed
	
	ttk::button .go -text "Generate!" -command generate
	ttk::button .save -text "Save" -command save -state disabled
	
	pack .view -side left
	pack .lsize .esize .lbranch .ebranch .llevel .elevel .lseed .eseed .go .save -side top
	
	wm title . "Fractal Generator"
}

proc generate {} {
	global img size branch level seed
	
	if {$seed eq "time"} {
		set seed [clock seconds]
	}
	
	cfractal::frame $size $branch $level $seed $img
	.save configure -state normal
}

proc save {} {
	global img
	
	set filename [tk_getSaveFile -filetypes {{{GIF Image} {.gif}} {{PNG Image} {.png}}}]
	if {$filename eq ""} {return}
	
	$img write $filename -background #000000 -format [lindex [split $filename .] end]
}

buildui