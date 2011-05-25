#!/usr/bin/wish

global	share
set	share "/usr/share/foo2zjs"

proc replaced {name} {
	exec usb_printerid $name
}

proc main {w} {
    global share

    image create photo icon -file [file join $share hplj1020_icon.gif]

    frame $w.frame
    tixBalloon $w.frame.balloon

    set n 0
    set old 1
    set pwd [pwd]
    
    foreach file [lsort [glob -nocomplain /sys/class/usb/lp*/device]] {
	set old 0
	regsub /.*usb/(lp\[^/]*)/.* $file {\1} lp
	cd $file
	cd ..
	set fp [open "product" "r"]
	gets $fp product
	close $fp
	set fp [open "serial" "r"]
	gets $fp serial
	close $fp
	cd $pwd
	if {$product != "HP LaserJet 1020" && $product != "HP LaserJet 1018"} {
	    continue
	}

	set f $w.frame.frame$n
	set prodsn [concat $product $serial]
	frame $f
	frame $f.sf$n
	label $f.sf$n.label1 -text "$prodsn"
	pack $f.sf$n.label1 -side top -fill y -expand 1
	label $f.sf$n.label2 -text "Replaced the paper?"
	pack $f.sf$n.label2 -side top -fill y -expand 1
	pack $f.sf$n -side left -fill y

	button $f.config$n -text "test" -image icon \
		-command "replaced /dev/usb/$lp"
	pack $f.config$n -side left -fill y
	$w.frame.balloon bind $f.config$n -balloonmsg "Replaced Paper"
	pack $f
	incr n
    }
    if {$old == 1} {
	foreach file [lsort [glob -nocomplain /dev/usb/lp?]] {
	    set f $w.frame.frame$n
	    frame $f
            frame $f.sf$n
            label $f.sf$n.label1 -text "$file"
            pack $f.sf$n.label1 -side top -fill y -expand 1
            label $f.sf$n.label2 -text "Replaced the paper?"
            pack $f.sf$n.label2 -side top -fill y -expand 1
            pack $f.sf$n -side left -fill y

	    button $f.config$n -text "test" -image icon \
		    -command "replaced $file"
	    pack $f.config$n -side left -fill y
	    $w.frame.balloon bind $f.config$n -balloonmsg "Replaced Paper"
	    pack $f
	    incr n
	}
    }
    if {$n == 0} {
	label $w.frame.label -text "No HP LaserJet 1018/1020"
	pack $w.frame.label
	puts "asdsd"
    }

    pack $w.frame -expand 1
}

wm title . "HP LaserJet 1018 and 1020 GUI"

package require Tix

main ""
