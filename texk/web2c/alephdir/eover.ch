% Aleph version-specific stuff;
% * we define the banner and everything to be
%   Aleph specific (with Aleph version numbering
%   added to the sequence --yes, I know it sucks)
% * we add both \eTeX-like and \OMEGA-like
%   commands to display the version
@x
@d banner=='This is Aleph, Version 3.14159265--1.15--2.1' {printed when \TeX\ starts}
@#
@d eTeX_version_string=='3.14159265--1.15--2.1' {current \eTeX\ version}
@d eTeX_version=2 { \.{\\eTeXversion} }
@d eTeX_revision==".1" { \.{\\eTeXrevision} }
@#
@d eTeX_banner=='This is Aleph, Version ',eTeX_version_string
  {printed when \eTeX\ starts}
@y
@d eTeX_version_banner=='2.1' {current \eTeX\ version}
@d Omega_version_banner=='1.15' {current $\Omega$ version}
@d Aleph_version_banner=='0.1' {current \Aleph\ version}
@d eTeX_version_string=="2.1" {current \eTeX\ version}
@d Omega_version_string=="1.15" { \.{\\OmegaVersion} }
@d Aleph_version_string=="0.0" { \.{\\AlephVersion} }
@d eTeX_version=2 { \.{\\eTeXversion} }
@d Omega_version=1 { \.{\\Omegaversion} }
@d Aleph_version=0 { \.{\\Alephversion} }
@d eTeX_minor_version=1 { \.{\\eTeXminorversion} }
@d Omega_minor_version=15 { \.{\\Omegaminorversion} }
@d Aleph_minor_version=0 { \.{\\Alephminorversion} }
@d eTeX_revision==".1" { \.{\\eTeXrevision} }
@d Omega_revision==".15" { \.{\\Omegarevision} }
@d Aleph_revision==".0" { \.{\\Alephrevision} }
@#
@d banner=='This is Aleph, Version 3.14159265-',Omega_version_banner,'-',eTeX_version_banner,'-',Aleph_version_banner {printed when \TeX\ starts}
@d eTeX_banner==banner
  {printed when \eTeX\ starts}
@z

% === Aleph history ===
%----------------------------------------
%       Version: 0.0 (RC0)
% Internal name: the successfull merge
%   Achievement: main executable that would run
%  Release date: 20030131
%----------------------------------------
%       Version: 0.0 (RC1)
% Internal name: the successfull trip (NOT)
%   Achievement: fixed the major 'trip' bug
%                making Aleph usable for
%                production use
%  Release date: 20030511
%----------------------------------------
%       Version: 0.0 (RC2)
% Internal name: the name change
%   Achievement: add \boxdir
%                add versioning info
%                add version-retrieving commands
%                fix \overfullrule>0pt
%                fix \charit
%                fix non-TLT text offset
%                fix leaders crashing or stalling
%  Release date: 20040322
%----------------------------------------
%       Version: 0.0 (RC3)
% Internal name: Oh Tea Peas
%   Achievement: fix some OTP/OCP-related bugs
%  Release date: 20040609
%----------------------------------------
%       Version: 0.0 (RC4)
% Internal name: Nothing really
%   Achievement: adapt to latest web2c changes
%  Release date: 20040909
%----------------------------------------

@x
@d eTeX_int=glue_val+4 {first of \eTeX\ codes for integers}
@y
@d Aleph_int=glue_val+4 {first of \Aleph\ codes for integers}
@d Aleph_int_num=5 {number of \Aleph\ integers}
@d eTeX_int=Aleph_int+Aleph_int_num {first of \eTeX\ codes for integers}
@z

@x
@d omega_code=5 {command code for \.{\\OmegaVersion}}
@d job_name_code=6 {command code for \.{\\jobname}}
@y
@d etex_code=5 {command code for \.{\\eTeXVersion}}
@d omega_code=6 {command code for \.{\\OmegaVersion}}
@d aleph_code=7 {command code for \.{\\AlephVersion}}
@d job_name_code=8 {command code for \.{\\jobname}}
@z

@x
primitive("OmegaVersion",convert,omega_code);@/
@!@:omega_version_}{\.{\\OmegaVersion} primitive@>
@y
primitive("eTeXVersion",convert,etex_code);@/
@!@:omega_version_}{\.{\\OmegaVersion} primitive@>
primitive("OmegaVersion",convert,omega_code);@/
@!@:omega_version_}{\.{\\OmegaVersion} primitive@>
primitive("AlephVersion",convert,aleph_code);@/
@!@:aleph_version_}{\.{\\AlephVersion} primitive@>
@z

% NOTE: we also put jobname in its own
% field. Why was it not put there?
@x
  omega_code: print_esc("OmegaVersion");
  @/@<Cases of |convert| for |print_cmd_chr|@>@/
  othercases print_esc("jobname")
@y
  etex_code: print_esc("eTeXVersion");
  omega_code: print_esc("OmegaVersion");
  aleph_code: print_esc("AlephVersion");
  job_name_code: print_esc("jobname");
  @/@<Cases of |convert| for |print_cmd_chr|@>@/
  othercases print_esc("???")
@z

@x
omega_code:;
job_name_code: if job_name=0 then open_log_file;
@y
etex_code: do_nothing;
omega_code: do_nothing;
aleph_code: do_nothing;
job_name_code: if job_name=0 then open_log_file;
@z

@x
omega_code: print("1.15");
job_name_code: print(job_name);
@y
etex_code: print(eTeX_version_string);
omega_code: print(Omega_version_string);
aleph_code: print(Aleph_version_string);
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
@d Aleph_version_code=Aleph_int {code for \.{\\Alephversion}}
@d Omega_version_code=Aleph_int+1 {code for \.{\\Omegaversion}}
@d Aleph_minor_version_code=Aleph_int+2 {code for \.{\\Alephminorversion}}
@d Omega_minor_version_code=Aleph_int+3 {code for \.{\\Omegaminorversion}}
@d eTeX_minor_version_code=Aleph_int+4 {code for \.{\\eTeXminorversion}}
@d eTeX_version_code=eTeX_int {code for \.{\\eTeXversion}}
@d Aleph_revision_code=job_name_code+1 {command code for \.{\\Alephrevision}}
@d Omega_revision_code=job_name_code+2 {command code for \.{\\Omegarevision}}
@d eTeX_revision_code=job_name_code+3 {command code for \.{\\eTeXrevision}}

@<Generate all \eTeX...@>=
primitive("lastnodetype",last_item,last_node_type_code);
@!@:last_node_type_}{\.{\\lastnodetype} primitive@>
primitive("Alephversion",last_item,Aleph_version_code);
@!@:eTeX_version_}{\.{\\Alephversion} primitive@>
primitive("Omegaversion",last_item,Omega_version_code);
@!@:eTeX_version_}{\.{\\Omegaversion} primitive@>
primitive("eTeXversion",last_item,eTeX_version_code);
@!@:eTeX_version_}{\.{\\eTeXversion} primitive@>
primitive("Alephminorversion",last_item,Aleph_minor_version_code);
@!@:eTeX_minor_version_}{\.{\\Alephminorversion} primitive@>
primitive("Omegaminorversion",last_item,Omega_minor_version_code);
@!@:eTeX_minor_version_}{\.{\\Omegaminorversion} primitive@>
primitive("eTeXminorversion",last_item,eTeX_minor_version_code);
@!@:eTeX_minor_version_}{\.{\\eTeXminorversion} primitive@>
primitive("Alephrevision",convert,Aleph_revision_code);@/
@!@:eTeX_revision_}{\.{\\Alephrevision} primitive@>
primitive("Omegarevision",convert,Omega_revision_code);@/
@!@:eTeX_revision_}{\.{\\Omegarevision} primitive@>
primitive("eTeXrevision",convert,eTeX_revision_code);@/
@!@:eTeX_revision_}{\.{\\eTeXrevision} primitive@>

@ @<Cases of |last_item| for |print_cmd_chr|@>=
last_node_type_code: print_esc("lastnodetype");
Aleph_version_code: print_esc("Alephversion");
Omega_version_code: print_esc("Omegaversion");
eTeX_version_code: print_esc("eTeXversion");
Aleph_minor_version_code: print_esc("Alephminorversion");
Omega_minor_version_code: print_esc("Omegaminorversion");
eTeX_minor_version_code: print_esc("eTeXminorversion");

@ @<Cases for fetching an integer value@>=
Aleph_version_code: cur_val:=Aleph_version;
Omega_version_code: cur_val:=Omega_version;
eTeX_version_code: cur_val:=eTeX_version;
Aleph_minor_version_code: cur_val:=Aleph_minor_version;
Omega_minor_version_code: cur_val:=Omega_minor_version;
eTeX_minor_version_code: cur_val:=eTeX_minor_version;

@ @<Cases of |convert| for |print_cmd_chr|@>=
Aleph_revision_code: print_esc("Alephrevision");
Omega_revision_code: print_esc("Omegarevision");
eTeX_revision_code: print_esc("eTeXrevision");

@ @<Cases of `Scan the argument for command |c|'@>=
Aleph_revision_code: do_nothing;
Omega_revision_code: do_nothing;
eTeX_revision_code: do_nothing;

@ @<Cases of `Print the result of command |c|'@>=
Aleph_revision_code: print(Aleph_revision);
Omega_revision_code: print(Omega_revision);
eTeX_revision_code: print(eTeX_revision);
@z
