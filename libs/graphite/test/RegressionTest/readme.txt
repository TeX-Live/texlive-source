Graphite Regression Test

COMPILING
Use makedebug.bat to build a debug version. Creating a release version is not supported.

RUNNING
Extract the fonts from the fonts.zip file and install them.

Options:
/d - breaks into the debugger (if any) when an error occurs
/s - silent mode; does not write to the standard output but only to the log file

The program generates a file called grregtest.log with a list of errors.

A file called tracelog.txt will be output for the tests for which tracing is turned on.

ADDING TESTS
See TestCase.cpp.

