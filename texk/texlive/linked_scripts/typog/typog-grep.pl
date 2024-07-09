#! /usr/bin/env perl

use autodie qw(:all);
use strict;
use warnings;

use Data::Dumper ();
use Encode ();
use English;
use File::Basename ();
use Getopt::Long;
use IO::File;
use IO::Handle;
use Term::ANSIColor ();

use constant COMMAND_NAME => File::Basename::basename($PROGRAM_NAME);

my $DEBUG = 0;
my $MATCH_COUNT = 0;
my $OUTPUT_IS_REDIRECTED;

sub fail_with_error {
    print STDERR join('', COMMAND_NAME, ': ', @_, "\n");
    exit 2;
}

sub issue_warning {
    print STDERR join('', COMMAND_NAME, ': warning: ', @_, "\n");
}

sub debug_print {
    return unless $DEBUG;
    print STDERR "+ @_\n";
}

sub quote_filesystem {qq("$_[0]")}
sub quote_literal {qq(`$_[0]')}

sub limit_string_length {
    my ($a_string, $a_maximum_length) = @_;

    if (length $a_string <= $a_maximum_length) {
        $a_string;
    } else {
        substr($a_string, 0, $a_maximum_length - 3) . '...';
    }
}

##  We set all colors to `undef' and fill them later with the values
##  of the actual configuration.
my $highlight_patterns = {
    PARTIAL_LINE => {
        FONT_SPEC => [qr#
                         \\
                         (?: OMS | OMX | OT1 | T1 | TS1 | U )
                         (?: /[^/]+ ){5} / \S+ \s
                         (?: \([+-]\d+\) )?
                        #x, undef],
        MATH => [qr#
                    \$
                    \\
                    (?: LMS | OML )
                    (?: /[^/]+ ){5} / \S+ \s
                    (?: \([+-]\d+\) )?
                    .*?
                    \$
                   #x, undef]
    },
    WHOLE_LINE => {
        FILL_STATE => [qr#^(?:Under|Over)full \\hbox .*$#, undef],
        FIRST_VBOX => [qr#^%%#, undef],
        HORIZONTAL_BREAKPOINT => [qr#^@@\d+:.*$#, undef],
        HORIZONTAL_BREAK_CANDIDATE => [qr#^@[\\ ].*$#, undef],
        LINE_BREAK_PASS => [qr#^@[a-z]+?pass#, undef],
        TIGHTNESS => [qr#^(?:Loose|Tight) \\hbox .*$#, undef],
        VERTICAL_BREAKPOINT => [qr#^% t=\d+.*$#, undef]
   }
};

sub colorize_line {
    my ($configuration, $line) = @_;

    foreach my $pattern_color_pair (values %{$highlight_patterns->{WHOLE_LINE}}) {
        next unless $pattern_color_pair->[1];
        return Term::ANSIColor::colored($line, $pattern_color_pair->[1])
          if $line =~ $pattern_color_pair->[0];
    }
    return $line if $line =~ m#^\.#; # we do not paint box contents yet

    $line =~ s#$highlight_patterns->{PARTIAL_LINE}->{MATH}->[0]
              #Term::ANSIColor::colored($MATCH, $highlight_patterns->{PARTIAL_LINE}->{MATH}->[1])
              #egx;

    $line =~ s#$highlight_patterns->{PARTIAL_LINE}->{FONT_SPEC}->[0]
              #Term::ANSIColor::colored($MATCH, $highlight_patterns->{PARTIAL_LINE}->{FONT_SPEC}->[1])
              #egx;

    return $line;
}

my $open_or_close_tag_regexp = qr#^</?typog-inspect[ >]#; # somewhat sloppy definition
my $close_tag_regexp = qr#^</typog-inspect>#;
my $open_tag_regexp =
  qr#^
     <typog-inspect \s+
     id="(?<id_match> .*?)" \s+
     job="(?<job_match> .*?)" \s+
     line="(?<line_match> .*?)" \s+
     page="(?<page_match> .*?)"
     >#x;

sub find_encoder {
    my $encoding = shift;

    my $encoder;

    if ($encoding) {
        $encoder = Encode::find_encoding($encoding);
        if (!$encoder) {
            issue_warning("encoding @{[quote_literal($encoding)]} unknown; proceeding without decoder");
        }
    }

    return $encoder;
}

sub grep_log_file {
    my ($options, $configuration, $file, $filename, $id_regexp) = @_;

    my $encoder = find_encoder($options->{ENCODING});
    my $job_name;
    my $line_number = 0; # line number in the log file we are inspecting, i.e., $filename
    my $match_count = 0;
    my $source_line_number; # line number in TeX file the log refers to, i.e., "$job_name.tex"
    my $page_number;
    my $regexp_modifier = $options->{IGNORE_CASE} ? 'i' : '';
    my $id_value;
    my @nesting_levels;

    if ($options->{WORD_REGEXP}) {
        $id_regexp = "\\b$id_regexp\\b";
    }

    while (my $line = readline $file) {
        chomp $line;
        $line_number++;

        $line = Encode::encode_utf8($encoder->decode($line, Encode::FB_CROAK)) if $encoder;

        if ($line =~ $close_tag_regexp) {
            fail_with_error("$filename: $line_number: mismatched open/close tags") unless @nesting_levels;
            pop @nesting_levels;
        }

        if (@nesting_levels and $nesting_levels[-1] and $line !~ $open_or_close_tag_regexp) {
            if ($options->{LOG_LINE_NUMBER}) {
                my $formatted_log_line_number =
                  sprintf $configuration->{LOG_LINE_NUMBER_FORMAT}, $line_number;
                if ($options->{COLORIZE_OUTPUT}) {
                    $formatted_log_line_number =
                      Term::ANSIColor::colored($formatted_log_line_number,
                                               $configuration->{COLORS}->{LOG_LINE_NUMBER});
                }
                print $formatted_log_line_number, ' ';
            }

            print "$job_name: " if $options->{JOB_NAME};

            if ($options->{LINE_NUMBER}) {
                my $formatted_line_number = sprintf $configuration->{LINE_NUMBER_FORMAT}, $source_line_number;
                if ($options->{COLORIZE_OUTPUT}) {
                    $formatted_line_number =
                      Term::ANSIColor::colored($formatted_line_number,
                                               $configuration->{COLORS}->{LINE_NUMBER});
                }
                print $formatted_line_number, ' ';
            }

            if ($options->{PAGE_NUMBER}) {
                my $formatted_page_number = sprintf $configuration->{PAGE_NUMBER_FORMAT}, $page_number;
                if ($options->{COLORIZE_OUTPUT}) {
                    $formatted_page_number =
                      Term::ANSIColor::colored($formatted_page_number,
                                               $configuration->{COLORS}->{PAGE_NUMBER});
                }
                print $formatted_page_number, ' ';
            }

            if ($options->{ID} and not $configuration->{PRINT_ID_AS_HEADING}) {
                my $formatted_id = sprintf $configuration->{ID_INLINE_FORMAT}, $id_value;
                if ($options->{COLORIZE_OUTPUT}) {
                    $formatted_id = Term::ANSIColor::colored($formatted_id ,
                                                             $configuration->{COLORS}->{ID_COLOR});
                }
                print $formatted_id, ' ';
            }

            if ($options->{COLORIZE_OUTPUT}) {
                print colorize_line($configuration, $line);
            } else {
                print $line;
            }
            print "\n";
        }

        if ($line =~ $open_tag_regexp) {
            $id_value = limit_string_length($+{id_match}, $configuration->{ID_MAX_LENGTH});
            $job_name = $+{job_match};
            $source_line_number = $+{line_match};
            $page_number = $+{page_match};

            my $found_matching_id = ($id_value =~ m/(?$regexp_modifier)$id_regexp/) ? 1 : 0;
            push @nesting_levels, $found_matching_id;
            if ($found_matching_id) {
                ++$MATCH_COUNT; # global count -- needed for return code of program
                ++$match_count; # per file count -- needed to be able to separate the hunks

                print "\n" if $match_count >= 2;
                if ($options->{ID} and $configuration->{PRINT_ID_AS_HEADING}) {
                    my $formatted_id = sprintf $configuration->{ID_HEADING_FORMAT}, $id_value;
                    if ($options->{COLORIZE_OUTPUT}) {
                        $formatted_id =
                          Term::ANSIColor::colored($formatted_id,
                                                   $configuration->{COLORS}->{ID_HEADING_COLOR});
                    }
                    print $formatted_id, "\n";
                }
            }
        }
    }

    return $match_count;
}

sub show_ids_in_file {
    my ($options, $configuration, $file, $filename, $id_regexp) = @_;

    my $encoder = find_encoder($options->{ENCODING});
    my $line_number = 0;
    my $match_count = 0;
    my @nesting_levels;

    while (my $line = readline $file) {
        chomp $line;
        $line_number++;

        $line = Encode::encode_utf8($encoder->decode($line, Encode::FB_CROAK)) if $encoder;

        if ($line =~ $close_tag_regexp) {
            fail_with_error("$filename: $line_number: mismatched open/close tags") unless @nesting_levels;
            pop @nesting_levels;
        }

        if ($line =~ $open_tag_regexp) {
            my $id_value = limit_string_length($+{id_match}, $configuration->{ID_MAX_LENGTH});
            my $job_name = $+{job_match};
            my $source_line_number = $+{line_match};
            my $page_number = $+{page_match};

            ++$MATCH_COUNT;
            ++$match_count;
            push @nesting_levels, 1;

            if ($options->{LOG_LINE_NUMBER}) {
                my $formatted_log_line_number =
                  sprintf $configuration->{LOG_LINE_NUMBER_FORMAT}, $line_number;
                if ($options->{COLORIZE_OUTPUT}) {
                    $formatted_log_line_number =
                      Term::ANSIColor::colored($formatted_log_line_number,
                                               $configuration->{COLORS}->{LOG_LINE_NUMBER});
                }
                print $formatted_log_line_number, ' ';
            }

            print "$job_name: " if $options->{JOB_NAME};

            if ($options->{LINE_NUMBER}) {
                my $formatted_line_number = sprintf $configuration->{LINE_NUMBER_FORMAT}, $source_line_number;
                if ($options->{COLORIZE_OUTPUT}) {
                    $formatted_line_number =
                      Term::ANSIColor::colored($formatted_line_number,
                                               $configuration->{COLORS}->{LINE_NUMBER});
                }
                print $formatted_line_number, ' ';
            }

            if ($options->{PAGE_NUMBER}) {
                my $formatted_page_number = sprintf $configuration->{PAGE_NUMBER_FORMAT}, $page_number;
                if ($options->{COLORIZE_OUTPUT}) {
                    $formatted_page_number =
                      Term::ANSIColor::colored($formatted_page_number,
                                               $configuration->{COLORS}->{PAGE_NUMBER});
                }
                print $formatted_page_number, ' ';
            }

            my $indent = $configuration->{ID_INDENT} * (@nesting_levels - 1);
            print ' ' x $indent, $id_value, "\n";
        }
    }

    return $match_count;
}

sub open_file_for_reading {
    my $filename = shift;

    my $file;

    if ($filename eq 'stdin') {
        $file = IO::Handle->new();
        $file->fdopen(fileno(STDIN), 'r') or
          fail_with_error("cannot open stdin: $OS_ERROR");
    } else {
        $file = IO::File->new($filename, 'r') or
          fail_with_error("cannot open @{[quote_filesystem($filename)]}: $OS_ERROR");
    }

    return $file;
}

sub close_file {
    my ($file, $filename) = shift;

    $file->close or
      issue_warning("problems while closing @{[quote_filesystem($filename)]}: $OS_ERROR");
}

sub grep_or_show {
    my ($options, $configuration, $file, $filename, $id_regexp) = @_;

    if ($options->{SHOW_ALL_IDS}) {
        return show_ids_in_file($options, $configuration, $file, $filename, $id_regexp);
    } else {
        return grep_log_file($options, $configuration, $file, $filename, $id_regexp);
    }
}

sub scan_files {
    my ($options, $configuration, $id_regexp, $log_filenames) = @_;

    my $match_count = 0;

    if (@$log_filenames) {
        foreach my $log_filename (@$log_filenames) {
            $log_filename = 'stdin' if $log_filename eq '-';
            if (@$log_filenames >= 2) {
                print "\n" unless $log_filename eq $log_filenames->[0];
                my $filename_header = "==> $log_filename <==\n";
                $filename_header = Term::ANSIColor::colored($filename_header,
                                                            $configuration->{COLORS}->{FILE_HEADER})
                  if $options->{COLORIZE_OUTPUT};
                print $filename_header;
            }
            my $file = open_file_for_reading($log_filename);
            $match_count += grep_or_show($options, $configuration, $file, $log_filename, $id_regexp);
            close_file($file, $log_filename);
        }
    } else {
        my $log_filename = 'stdin';
        my $file = open_file_for_reading($log_filename);
        $match_count = grep_or_show($options, $configuration, $file, $log_filename, $id_regexp);
        close_file($file, $log_filename);
    }

    return $match_count;
}

sub redirect_and_scan_files {
    my ($options, $configuration, $id_regexp, $log_filenames) = @_;

    my $pager;

    my $pid = open($pager, '|-', $configuration->{PAGER}, $configuration->{PAGER_FLAGS});
    fail_with_error('failed to redirect to pager ', quote_literal($configuration->{PAGER}),
                    ' with flags ', quote_literal($configuration->{PAGER_FLAGS}),
                    ": $OS_ERROR")
      unless defined $pid;
    my $stdout = select $pager;

    $pager->autoflush;
    my $match_count = scan_files($options, $configuration, $id_regexp, $log_filenames);

    close $pager or issue_warning "error occurred while closing the pager (pid: $pid) pipe: $OS_ERROR";
    select $stdout;

    return $match_count;
}

########################################################################

my $configuration_key_map = {
    'id-format' => 'ID_INLINE_FORMAT',
    'id-indent' => 'ID_INDENT',
    'id-heading' => 'PRINT_ID_AS_HEADING',
    'id-heading-format' => 'ID_HEADING_FORMAT',
    'id-max-length' => 'ID_MAX_LENGTH',
    'line-number-format' => 'LINE_NUMBER_FORMAT',
    'log-line-number-format' => 'LOG_LINE_NUMBER_FORMAT',
    'page-number-format' => 'PAGE_NUMBER_FORMAT',

    'file-header-color' => 'FILE_HEADER',
    'fill-state-color' => 'FILL_STATE',
    'first-vbox-color' => 'FIRST_VBOX',
    'font-spec-color' => 'FONT_SPEC',
    'horizontal-break-candidate-color' => 'HORIZONTAL_BREAK_CANDIDATE',
    'horizontal-breakpoint-color' => 'HORIZONTAL_BREAKPOINT',
    'id-color' => 'ID_COLOR',
    'id-heading-color' => 'ID_HEADING_COLOR',
    'line-break-pass-color' => 'LINE_BREAK_PASS',
    'line-number-color' => 'LINE_NUMBER',
    'log-line-number-color' => 'LOG_LINE_NUMBER',
    'math-color' => 'MATH',
    'page-number-color' => 'PAGE_NUMBER',
    'pager' => 'PAGER',
    'pager-flags' => 'PAGER_FLAGS',
    'tightness-color' => 'TIGHTNESS',
    'vertical-breakpoint-color' => 'VERTICAL_BREAKPOINT'
};

my $default_configuration = {
    COLORS => {
        FILE_HEADER => 'bold black',
        FILL_STATE => 'bold magenta',
        FIRST_VBOX => 'bold red',
        FONT_SPEC => 'grey12',
        HORIZONTAL_BREAKPOINT => 'bold green',
        HORIZONTAL_BREAK_CANDIDATE => 'blue',
        ID_COLOR => 'white on_black',
        ID_HEADING_COLOR => 'white on_black',
        LINE_BREAK_PASS => 'bold green',
        LINE_NUMBER => 'bold black',
        LOG_LINE_NUMBER => 'italic black',
        MATH => 'yellow',
        PAGE_NUMBER => 'bold white on_red',
        TIGHTNESS => 'bold cyan',
        VERTICAL_BREAKPOINT => 'red'
    },
    ID_INLINE_FORMAT => '%s:',
    ID_HEADING_FORMAT => '--> %s <--',
    ID_INDENT => 8,
    ID_MAX_LENGTH => 40,
    LINE_NUMBER_FORMAT => '%5d',
    LOG_LINE_NUMBER_FORMAT => '%6d',
    PAGE_NUMBER_FORMAT => '[%3d]',
    PAGER => 'less',
    PAGER_FLAGS => '--quit-if-one-screen',
    PRINT_ID_AS_HEADING => 0
};

sub initialize_highlighting_from_configuration {
    my $configuration = shift;

    while (my (undef, $assoc) = each %$highlight_patterns) {
        while (my ($name, $pattern_color_pair) = each %$assoc) {
            $pattern_color_pair->[1] = $configuration->{COLORS}->{$name};
        }
    }
}

sub modify_configuration {
    my ($configuration, $key, $value) = @_;

    fail_with_error('malformed KEY=VALUE pair -- missing key') unless $key;

    if (defined $configuration_key_map->{$key}) {
        if ($key =~ m/-color$/) {
            $configuration->{COLORS}->{$configuration_key_map->{$key}} = $value;
        } else {
            $configuration->{$configuration_key_map->{$key}} = $value;
        }
    } else {
        fail_with_error("@{[quote_literal($key)]} is not a valid configuration KEY");
    }
}

sub setup_configuation {
    my ($config_spec, $configuration) = @_;

    foreach my $spec (split ':', $config_spec) {
        my ($key, $value) = split '=', $spec;
        modify_configuration($configuration, $key, $value);
    }
}

my $default_options = {
    COLORIZE_MODE => 'auto',
    DEBUG => 0,
    ENCODING => undef,
    ID => 0,
    IGNORE_CASE => 0,
    JOB_NAME => 0,
    LINE_NUMBER => 0,
    LOG_LINE_NUMBER => 0,
    PAGE_NUMBER => 0,
    REQUEST_PAGER => 1,
    WORD_REGEXP => 0
};

sub show_help {
    print <<HELP_TEXT;
Usage: @{[COMMAND_NAME]} [OPTION] ID-REGEXP LOG-FILE...
Structured grep for typog-inspect elements that match ID-REGEXP in LOG-FILE.

Options
      --color [WHEN],
      --colour [WHEN]         use color to highlight specific log contents
                              WHEN is 'always', 'never', or 'auto'
  -C, --config KEY=VALUE      set configuration KEY to VALUE
  -E, --encoding ENCODING     set character ENCODING of LOG-FILE
  -i, --[no-]id               print matching id with output lines
  -y, --[no-]ignore-case      ignore case distinctions in patterns and data
  -j, --[no-]job-name         print \\jobname with output lines
  -n, --[no-]line-number      print TeX-source line number with output lines
  -N, --[no-]log-line-number  print log-file line number with output lines
  -p, --[no-]page-number      print page number with output lines
  -P, --[no-]pager            redirect output to pager
  -w, --[no-]word-regexp      match only whole words

  -a, --all, --any            discover all IDs in LOG-FILE

      --debug                 turn on debug output
  -h, --help                  display this help and exit
      --show-config           show default configuration and exit
      --show-encodings        show all known encodings and exit
  -V, --version               show version information and exit

HELP_TEXT

    exit 0;
}

sub show_encodings {
    my @all_encodings = Encode->encodings(':all');

    foreach my $encoding (@all_encodings) {
        print "$encoding\n";
    }

    exit 0;
}

sub show_configuration {
    my $format_string_value = sub {quote_literal($default_configuration->{$_[0]})};

    print <<FIXED_CONFIGURATION_TEXT;
Configuration
Key                                     Default Value
------------------------------------    -------------
id-format                               @{[$format_string_value->('ID_INLINE_FORMAT')]}
id-heading                              $default_configuration->{PRINT_ID_AS_HEADING}
id-heading-format                       @{[$format_string_value->('ID_HEADING_FORMAT')]}
id-indent                               $default_configuration->{ID_INDENT}
id-max-length                           $default_configuration->{ID_MAX_LENGTH}
line-number-format                      @{[$format_string_value->('LINE_NUMBER_FORMAT')]}
log-line-number-format                  @{[$format_string_value->('LOG_LINE_NUMBER_FORMAT')]}
page-number-format                      @{[$format_string_value->('PAGE_NUMBER_FORMAT')]}
pager                                   @{[$format_string_value->('PAGER')]}
pager-flags                             @{[$format_string_value->('PAGER_FLAGS')]}

FIXED_CONFIGURATION_TEXT

    foreach my $configuration_key (sort keys %$configuration_key_map) {
        next unless $configuration_key =~ m/-color$/;
        printf("%-36s    %s\n",
               $configuration_key,
               quote_literal($default_configuration->
                             {COLORS}->
                             {$configuration_key_map->{$configuration_key}}));
    }

    exit 0;
}

sub show_version {
    print <<VERSION_TEXT;
typog-grep 0.4

Copyright (C) 2024 by Ch. L. Spiel
License LPPL: LaTeX Project Public License version 1.3c or later
VERSION_TEXT

    exit 0;
}

sub get_options {
    my ($options, $configuration) = @_;

    Getopt::Long::Configure('gnu_getopt', 'no_ignore_case');

    Getopt::Long::GetOptions('a|all|any' => \$options->{SHOW_ALL_IDS},
                             'color|colour=s' => \$options->{COLORIZE_MODE},
                             'C|configuration=s' => sub{setup_configuation($_[1], $configuration)},
                             'debug+' => \$DEBUG,
                             'E|encoding=s' => \$options->{ENCODING},
                             'h|help' => \&show_help,
                             'i|id!' => \$options->{ID},
                             'y|ignore-case!' => \$options->{IGNORE_CASE},
                             'j|job-name!' => \$options->{JOB_NAME},
                             'n|line-number!' => \$options->{LINE_NUMBER},
                             'N|log-line-number!' => \$options->{LOG_LINE_NUMBER},
                             'p|page-number!' => \$options->{PAGE_NUMBER},
                             'P|pager!' => \$options->{REQUEST_PAGER},
                             'show-encodings' => \&show_encodings,
                             'show-config' => \&show_configuration,
                             'V|version' => \&show_version,
                             'w|word-regexp!' => \$options->{WORD_REGEXP}) or
                               fail_with_error('problems while parsing options');

    if ($options->{COLORIZE_MODE}) {
        fail_with_error("unknown colorize mode @{[quote_literal($options->{COLORIZE_MODE})]}")
          unless $options->{COLORIZE_MODE} =~ m/^(?:always|auto|never)$/i;
    } else {
        $options->{COLORIZE_MODE} = 'auto';
    }
}

sub do_colorize {
    my $colorize_mode = shift;

    if ($colorize_mode =~ m/never/i) {
        0;
    } elsif ($colorize_mode =~ m/always/i) {
        1;
    } elsif ($colorize_mode =~ m/auto/i) {
        not $OUTPUT_IS_REDIRECTED;
    }
}

##  For the comparison with the POSIX spec of grep(1) consult
##          https://pubs.opengroup.org/onlinepubs/9699919799/utilities/grep.html

sub main {
    $OUTPUT_IS_REDIRECTED = -t STDOUT ? 0 : 1;

    my $configuration = {%$default_configuration};
    my $options = {%$default_options};
    debug_print(Data::Dumper::Dumper(['Default Configuration', $configuration]));
    debug_print(Data::Dumper::Dumper(['Default Options', $options]));

    my $user_options = {};
    my $user_configuration = {};
    get_options($user_options, $user_configuration);
    debug_print(Data::Dumper::Dumper(['User Configuration', $user_configuration]));
    debug_print(Data::Dumper::Dumper(['User Options', $user_options]));
    while (my ($key, $value) = each %$user_options) {
        $options->{$key} = $value if $value;
    }
    while (my ($key, $value) = each %$user_configuration) {
        $configuration->{$key} = $value if $value;
    }
    debug_print(Data::Dumper::Dumper(['Final Configuration', $configuration]));
    debug_print(Data::Dumper::Dumper(['Final Options', $options]));

    $options->{COLORIZE_OUTPUT} = do_colorize($options->{COLORIZE_MODE});
    initialize_highlighting_from_configuration($configuration);

    my $id_regexp;
    if ($options->{SHOW_ALL_IDS}) {
        $id_regexp = '^';
        issue_warning("option @{[quote_literal('--id')]} ignored in @{[quote_literal('--all')]} mode")
          if $options->{ID};
    } else {
        fail_with_error('missing ID-REGEXP') unless @ARGV >= 1;
        $id_regexp = shift @ARGV;
    }

    if ($user_options->{REQUEST_PAGER} && $OUTPUT_IS_REDIRECTED) {
        issue_warning("option @{[quote_literal('--pager')]} ignored because output is redirected");
    }
    my $use_pager = $options->{REQUEST_PAGER} && !$OUTPUT_IS_REDIRECTED;
    my $match_count;
    if ($use_pager) {
        $match_count = redirect_and_scan_files($options, $configuration, $id_regexp, \@ARGV);
    } else {
        $match_count = scan_files($options, $configuration, $id_regexp, \@ARGV);
    }

    exit ($match_count == 0);
}

main();
