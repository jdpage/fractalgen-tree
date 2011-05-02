#!/usr/bin/env tclsh8.5

catch {load [file join [file dirname [info script]] libcfractal.so]}

package require Tk
package require cfractal

set img [image create photo -palette grayscale]

proc gen {s n} {
	set m {}
	for {set k 0} {$k < $s} {incr k} {
		lappend m [expr {($n + (360.0/$s) * $k) * 3.14159 / 180}]
	}
	return $m
}

set kernel [gen 5 0]

label .img -image $img
pack .img

proc fix {n} {
	return [expr {$n > 6.28 ? $n - 6.28 : $n}]
}

# for {set k 0} {$k < 126} {incr k} {
while {1} {
	set nk [list]
	foreach k $kernel {
		lappend nk [fix [expr {$k + 0.01}]]
	}
	
	set kernel $nk
	
	cfractal::frame 500 $kernel 7 $img
	
	# $img write frame[format "%04d" $k].gif 
	
	update
	after 20
}
