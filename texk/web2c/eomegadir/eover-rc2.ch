% e-Omega version-specific stuff;
% * we define the banner and everything to be
%   e-Omega specific (with e-Omega version numbering
%   added to the sequence --yes, I know it sucks)
% * we add both \eTeX-like and \OMEGA-like
%   commands to display the version
@x
@d banner=='This is e-Omega, Version 3.141592--1.15--2.1' {printed when \TeX\ starts}
@#
@d eTeX_version_string=='3.141592--1.15--2.1' {current \eTeX\ version}
@d eTeX_version=2 { \.{\\eTeXversion} }
@d eTeX_revision==".1" { \.{\\eTeXrevision} }
@#
@d eTeX_banner=='This is e-Omega, Version ',eTeX_version_string
  {printed when \eTeX\ starts}
@y
@d banner=='This is e-Omega, Version 3.141592--1.15--2.1--RC2--pre' {printed when \TeX\ starts}
@#
@d eTeX_version_string=='3.141592--1.15--2.1--RC2--pre' {current \eTeX\ version}
@d eTeX_version=2 { \.{\\eTeXversion} }
@d eTeX_revision==".1" { \.{\\eTeXrevision} }
@d Omega_version_string=="1.15" { \.{\\OmegaVersion} }
@d eOmega_version_string=="0.3--RC2" { \.{\\eOmegaVersion} }
@d eOmega_version=0 { \.{\\eOmegaversion} }
@d eOmega_revision==".3--RC2" { \.{\\eOmegarevision} }
@#
@d eTeX_banner==banner
  {printed when \eTeX\ starts}
@z

% === e-Omega history ===
%----------------------------------------
%       Version: 0.1 (RC0)
% Internal name: the successfull merge
%   Achievement: main executable that would run
%  Release date: 20030131
%----------------------------------------
%       Version: 0.2 (RC1)
% Internal name: the successfull trip (NOT)
%   Achievement: fixed the major 'trip' bug
%                making e-Omega usable for
%  Release date: 20030511
%----------------------------------------
%       Version: 0.3 (RC2)
% Internal name: <yet unnamed>
%   Achievement: <planned: fix the minor 'trip' bug>
%                <implemented: add \boxdir>
%                <implemented: add versioning info>
%                <implemented: add version-retrieving commands>
%  Release date: <yet to be released>
%----------------------------------------

@x
@d eTeX_int=glue_val+4 {first of \eTeX\ codes for integers}
@y
@d eOmega_int=glue_val+4 {first of \eOmega\ codes for integers}
@d eOmega_int_num=1 {number of \eOmega\ integers}
@d eTeX_int=eOmega_int+eOmega_int_num {first of \eTeX\ codes for integers}
@z

@x
@d omega_code=5 {command code for \.{\\OmegaVersion}}
@d job_name_code=6 {command code for \.{\\jobname}}
@y
@d omega_code=5 {command code for \.{\\OmegaVersion}}
@d eomega_code=6 {command code for \.{\\eOmegaVersion}}
@d job_name_code=7 {command code for \.{\\jobname}}
@z

@x
primitive("OmegaVersion",convert,omega_code);@/
@!@:omega_version_}{\.{\\OmegaVersion} primitive@>
@y
primitive("OmegaVersion",convert,omega_code);@/
@!@:omega_version_}{\.{\\OmegaVersion} primitive@>
primitive("eOmegaVersion",convert,eomega_code);@/
@!@:eomega_version_}{\.{\\eOmegaVersion} primitive@>
@z

@x
  omega_code: print_esc("OmegaVersion");
  @/@<Cases of |convert| for |print_cmd_chr|@>@/
@y
  omega_code: print_esc("OmegaVersion");
  @/@<Cases of |convert| for |print_cmd_chr|@>@/
  eomega_code: print_esc("eOmegaVersion");
  @/@<Cases of |convert| for |print_cmd_chr|@>@/
@z

@x
omega_code:;
job_name_code: if job_name=0 then open_log_file;
@y
omega_code: do_nothing;
eomega_code: do_nothing;
job_name_code: if job_name=0 then open_log_file;
@z

@x
omega_code: print("1.15");
job_name_code: print(job_name);
@y
omega_code: print(Omega_version_string);
eomega_code: print(eOmega_version_string);
job_name_code: print(job_name);
@z

@x
@d eTeX_version_code=eTeX_int {code for \.{\\eTeXversion}}
@d eTeX_revision_code=job_name_code+1 {command code for \.{\\eTeXrevision}}

@<Generate all \eTeX...@>=
primitive("lastnodetype",last_item,last_node_type_code);
@!@:last_node_type_}{\.{\\lastnodetype} primitive@>
primitive("eTeXversion",last_item,eTeX_version_code);
@!@:eTeX_version_}{\.{\\eTeXversion} primitive@>
primitive("eTeXrevision",convert,eTeX_revision_code);@/
@!@:eTeX_revision_}{\.{\\eTeXrevision} primitive@>

@ @<Cases of |last_item| for |print_cmd_chr|@>=
last_node_type_code: print_esc("lastnodetype");
eTeX_version_code: print_esc("eTeXversion");

@ @<Cases for fetching an integer value@>=
eTeX_version_code: cur_val:=eTeX_version;

@ @<Cases of |convert| for |print_cmd_chr|@>=
eTeX_revision_code: print_esc("eTeXrevision");

@ @<Cases of `Scan the argument for command |c|'@>=
eTeX_revision_code: do_nothing;

@ @<Cases of `Print the result of command |c|'@>=
eTeX_revision_code: print(eTeX_revision);
@y
@d eOmega_version_code=eOmega_int {code for \.{\\eOmegaversion}}
@d eTeX_version_code=eTeX_int {code for \.{\\eTeXversion}}
@d eOmega_revision_code=job_name_code+1 {command code for \.{\\eOmegarevision}}
@d eTeX_revision_code=job_name_code+2 {command code for \.{\\eTeXrevision}}

@<Generate all \eTeX...@>=
primitive("lastnodetype",last_item,last_node_type_code);
@!@:last_node_type_}{\.{\\lastnodetype} primitive@>
primitive("eOmegaversion",last_item,eOmega_version_code);
@!@:eTeX_version_}{\.{\\eOmegaversion} primitive@>
primitive("eTeXversion",last_item,eTeX_version_code);
@!@:eTeX_version_}{\.{\\eTeXversion} primitive@>
primitive("eOmegarevision",convert,eOmega_revision_code);@/
@!@:eTeX_revision_}{\.{\\eOmegarevision} primitive@>
primitive("eTeXrevision",convert,eTeX_revision_code);@/
@!@:eTeX_revision_}{\.{\\eTeXrevision} primitive@>

@ @<Cases of |last_item| for |print_cmd_chr|@>=
last_node_type_code: print_esc("lastnodetype");
eOmega_version_code: print_esc("eOmegaversion");
eTeX_version_code: print_esc("eTeXversion");

@ @<Cases for fetching an integer value@>=
eOmega_version_code: cur_val:=eOmega_version;
eTeX_version_code: cur_val:=eTeX_version;

@ @<Cases of |convert| for |print_cmd_chr|@>=
eOmega_revision_code: print_esc("eOmegarevision");
eTeX_revision_code: print_esc("eTeXrevision");

@ @<Cases of `Scan the argument for command |c|'@>=
eOmega_revision_code: do_nothing;
eTeX_revision_code: do_nothing;

@ @<Cases of `Print the result of command |c|'@>=
eOmega_revision_code: print(eOmega_revision);
eTeX_revision_code: print(eTeX_revision);
@z
