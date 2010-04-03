# bsnl.awk - eliminate backslash-newline sequences
# Public domain.  Originally written 2010, Karl Berry.

# on a line ending with a backslash, save it (minus the backslash).
/\\$/ { 
        buf = buf substr ($0, 1, length ($0) - 1);
        next;
      }

# on other lines, print the buffer if there is one, then the regular line.
      {
        if (buf) {
          printf "%s", buf;  # don't print a newline
          buf = "";
        }
        print;
      }
