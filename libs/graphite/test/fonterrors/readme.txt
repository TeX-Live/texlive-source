FONTS FOR TESTING ERROR HANDLING

This directory constains a set of fonts that have been deliberately corrupted in order to test Graphite's error recovery capabilities.

The "correct" behavior of the font is that vowels are rendered as lowercase, consonants as uppercase, and digits are raised vertically. For some of the fonts, recovery will be adequate to show some smart behaviors; for others the engine will revert to pure dumb rendering.


* GrErr BadGloc - errBadGloc.ttf
	The stated number of attributes exceeds actual number. This actually should not cause any real error, since the attributes are never read.

* GrErr BadGloc2 - errBadGloc2.ttf
	The stated number of attributes is less than actual number. This causes attributes greater than the number specified to have the value zero.

* GrErr BadPass - errBadPass.ttf
	One of the offsets to the pass information points off to somewhere nonsensical.

* GrErr BadStackMachine - errStackMachine.ttf
	Extra bytes have been inserted into the stack machine commands for both of the substitution rules. The engine manages to recover and handle the rule correctly in this case. (Reproduce using lowercase consonants or uppercase vowels.)

* GrErr BadSubs - errSubs.ttf
	A PutSubs command references a non-existent replacement class. This causes the glyph to appear as a square box. (Reproduce using lowercase consonants or uppercase vowels.)

* GrErr BadVersion - errBadVersion.ttf
	Bad version numbers are recorded in the Silf, Gloc, Glat, and Feat tables. Reading the Silf table will trigger an error being thrown, and the font will fall back to dumb rendering.

* GrErr GarbageFsm - errGarbageFsm.ttf
	The state table portions of the FSMs have been overwritten with garbage. Font will fall back to dumb rendering when any rules are fired.

* GrErr GarbageGlat - errGarbageGlat.ttf
	A section of Glat is overwritten with garbage. The "normal" behavior of the font is to raise the digits by 400m. But the lower digits (1 and 2) have been corrupted so they should simply not show that behavior.

* GrErr GarbageSilf - errGarbageSilf.ttf
	A section of Silf table is overwritten with garbage. Font will fall back to dumb rendering.

* GrErr SilfVersionMismatch - errSilfVerMismatch.ttf
	The Silf table claims to be version 1.0, but is actually version 2.0. Font will fall back to dumb rendering.

* GrErr SubsUnderflow - errSubsUnderflow.ttf
	A bogus command is inserted into the stack machine commands for both the substitution rules, causing an underflow when running the rules. The engine manages to recover and handle the rule correctly in this case. (Reproduce using lowercase consonants or uppercase vowels.)

* GrErr TooManyClasses - errTooManyClasses.ttf
	Very large number of classes in the list of replacement classes, larger than are actually there.

