# $Id$
# cnf-to-paths.awk - convert texmf.cnf assignments to paths.h #define's.
# Public domain.  Originally written 2011, Karl Berry.

# We assume comments have been stripped.
# 
# we only care about definitions with a valid C identifier (e.g.,
# TEXINPUTS, not TEXINPUTS.latex), that is, lines that look like this:
#   <name> = <value>
# (whitespace around the = is optional)
#
/^[ \t]*[A-Z0-9_]+[ \t]*=/ {
  # On these lines, we distinguish three cases:
  # 
  # 1) definitions referring to SELFAUTO*, which we want to keep.  In
  # particular, this is how the compile-time TEXMFCNF gets defined and
  # thus how texmf.cnf gets found.  In fact, TEXMFCNF is the only
  # relevant compile-time value at all, and since its value is not used
  # from texmf.cnf, we could simplify this whole process in both
  # directions. Maybe someday.
  # 
  # 2) definitions starting with a /, which we also want to keep.  Here
  # we assume a distro maintainer has changed a variable, e.g.,
  # TEXMFMAIN=/usr/share/texmf, so keep it.  (This also preserves the
  # default values for OSFONTDIR and TRFONTS, but that's ok.)
  # 
  # 3) anything else, which we want to convert to a constant /nonesuch.
  # That way, the binaries don't get changed just because we change
  # definitions in texmf.cnf.
  # 
  # The definition of DEFAULT_TEXMFROOT (and other variables)
  # that winds up in the final paths.h will not be used.
  
  # Extract the identifier and the value from the line.  Since
  # gawk's subexpression matching is an extension, do it with copies.
  ident = $0;
  sub(/^[ \t]*/, "", ident);
  sub(/[ \t]*=.*/, "", ident);
  
  val = $0;
  sub(/^.*=[ \t]*/, "", val);
  sub(/[ \t]*$/, "", val);
  #print "got ident", ident, "and val", val >"/dev/stderr"; 
  
  if (val ~ /\$SELFAUTO/) {
    # Replace all semicolons with colons in the SELFAUTO paths we're keeping.
    # (The path-splitting code should be changed to understand both.)
    gsub(/;/, ":", val);
  } else if (val ~ /^\//) {
    # If the value starts with /, presume we're compiling with changes
    # made for a distro, and keep it.  Likewise switch to :.
    gsub(/;/, ":", val);
  } else {
    val = "/nonesuch";
  }
  
  print "#ifndef DEFAULT_" ident;
  print "#define DEFAULT_" ident " \"" val "\"";
  print "#endif";
  print "";
}
