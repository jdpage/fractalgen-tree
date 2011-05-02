#!/usr/bin/env wish8.5

catch {load [file join [file dirname [info script]] libcfractal.so]}

package require Tk
package require cfractal

set img [image create photo -palette grayscale]
set size 500
set branch 4
set level 7
set seed "time"
set kernel {}

proc buildui {} {
	global img size branch level seed kernel
	canvas .view
	.view create image 0 0 -image $img -anchor nw
	
	ttk::label .lsize -text "Size:"
	ttk::entry .esize -textvariable size
	
	ttk::label .lbranch -text "Branches:"
	ttk::entry .ebranch -textvariable branch
	bind .ebranch <FocusOut> {
		set kernel [cfractal::newkernel $branch $seed]
	}
	
	ttk::label .llevel -text "Level:"
	ttk::entry .elevel -textvariable level
	
	ttk::label .lseed -text "Seed:"
	ttk::entry .eseed -textvariable seed
	
	ttk::label .lkernel -text "Kernel:"
	ttk::entry .ekernel -textvariable kernel
	
	ttk::button .go -text "Generate!" -command generate
	ttk::button .seed -text "Random Seed" -command {
		set seed [expr {int(rand() * 2**30)}]
		set kernel [cfractal::newkernel $branch $seed]
	}
	ttk::button .time -text "Time Seed" -command {
		set seed [clock seconds]
		set kernel [cfractal::newkernel $branch $seed]
	}
	ttk::button .uplevel -text "Incr level" -command {
		incr level
		generate
	}
	ttk::button .save -text "Save" -command save -state disabled
	ttk::button .kernel -text "Toggle angles" -command toggleangles -state disabled
	
	pack .view -side left
	pack .lsize .esize .lbranch .ebranch .llevel .elevel .lseed .eseed .lkernel .ekernel \
		.go .seed .time .uplevel .save .kernel \
		-side top -padx 2 -pady 1
	
	wm title . "Fractal Generator"
}

proc generate {} {
	global img size branch level seed kernel
	
	if {$seed eq "time"} {
		set seed [clock seconds]
	}
	
	if {$kernel eq ""} {
		set kernel [cfractal::newkernel $branch $seed]
	}
	
	cfractal::frame $size $kernel $level $img
	.view configure -width $size -height $size
	.save configure -state normal
	.kernel configure -state normal
	
	toggleangles
	toggleangles
}

set angleson 0

proc toggleangles {} {
	global kernel angleson size
	
	if {!$angleson} {
		set angleson 1
		set halfsize [expr {$size * 0.5}]
		foreach angle $kernel {
			set x [expr {$halfsize + $halfsize * cos($angle)}]
			set y [expr {$halfsize + $halfsize * sin($angle)}]
			.view create line $x $y $halfsize $halfsize -tags {angle} -fill red
		}
	} else {
		set angleson 0
		.view delete angle
	}
}

proc save {} {
	global img
	
	set filename [tk_getSaveFile -filetypes {{{GIF Image} {.gif}} {{PNG Image} {.png}}}]
	if {$filename eq ""} {return}
	
	$img write $filename -background #000000 -format [lindex [split $filename .] end]
}

buildui