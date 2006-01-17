directory /w/kpathsea:/w/dviljk

define redo
file dvilj4
end

#set env TFMFONTS /w/dvitest/fonts//tfm
#set env PKFONTS /w/dvitest/fonts//pk
#set args --D255 /w/dvitest/story
#set args --D24 /w/dvitest/story
#set args -M0 /w/dvitest/maketex
#set args table
#set args hwir >hwir.lj
#set env TEXFONTS $cm/tfm
#set args -M1 /w/dvitest/magstep -e/dev/null

# Stack underflow. Ruediger Deppe <deppe@Venn.ime.rwth-aachen.de>, 10 Jul 1995.
set args -D1 -M1 me-chap-6.dvi
#set args -D1 -M1 try
