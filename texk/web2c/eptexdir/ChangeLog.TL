ChangeLog.TL: TeX Live (TL) changes for e-pTeX
==============================================

2011-01-19  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* eptex.ech, etex.ch0: Adapt to modified ../ptexdir/ptex-base.ch.

	* eptex.ech: Make \lastnodetype consistent with e-TeX, based on
	code from Hironori Kitagawa <h_kitagawa2001@yahoo.co.jp>.

2011-01-17  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* eptex.ech: banner_k==banner for e-pTeX.

	* am/eptex.am (eptrip.diffs): Use $(triptrap_diffs).

2011-01-14  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* am/eptex.am (eptrip-clean): Use TRIPTRAP_CLEAN.

2011-01-13  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* eptrip.test (new): Shell script for e-pTRIP test,
	from Hironori Kitagawa <h_kitagawa2001@yahoo.co.jp>.
	* eptrip.diffs (new): Expected e-pTRIP test results.

	* eptex.ech: Increase pTeX if_*_codes above e-TeX codes.
