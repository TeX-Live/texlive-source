% This changefile gets rid of all the remaining eqtb references
% and fixes other similar issues
% (hopefully ...)
%----------------------------------------
@x
begin if eTeX_ex and(eqtb[p].int=w) then
@y
begin if eTeX_ex and(new_eqtb_int(p)=w) then
@z
%----------------------------------------
% eomega.web line 29099
@x
@d eTeX_state(#)==eqtb[eTeX_state_base+#].int {an \eTeX\ state variable}
@y
@d eTeX_state(#)==new_eqtb_int(eTeX_state_base+#) {an \eTeX\ state variable}
@z
%----------------------------------------
@x
@d eTeX_revision_code=6 {command code for \.{\\eTeXrevision}}
@y
@d eTeX_revision_code=job_name_code+1 {command code for \.{\\eTeXrevision}}
@z

