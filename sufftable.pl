#! /usr/bin/perl -w

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#                                                               #
# Suffix table inducer for the YABC Tagger                      #
# Authors: Vladislav Poritski & Oksana Volchek, BSU             #
# Last revised: 2015-06-26                                      #
#                                                               #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

use strict;
use locale;
use List::Util qw/sum min/;
use Benchmark;
use Getopt::Std;
$| = 1; # autoflush mode on, do not disable

# # # # # # # # # # # # #
# GLOBAL CONSTANTS
# # # # # # # # # # # # #

# Default list of postfixes which trigger line dropping (see -e option below)
my $EXCL = "GUESS|COMP2eq|COMP2neq|COMP3|TAR";

# # # # # # # # # # # # #
# SUBROUTINES
# # # # # # # # # # # # #

sub log2 { return log($_[0]) / log(2); }

sub sum_and_entropy
	{
	my %a = %{$_[0]};
	my $denom = sum (values %a);
	my @p = map { $a{$_} / $denom } keys %a;
	return $denom, sum (map { - $_ * log2($_) } @p);
	}

sub extract_filename
	{
	my @a = split (/\//, shift);
	my $filename = pop @a;
	$filename =~ s/\.txt$//ig;
	return $filename;
	}

sub parse_line_base
	{
	my $line = shift;
	chomp $line;
	if ($line)
		{
		my @output = split (/\t/, $line);
		if ($#output != 2) { die "Invalid input format: 3 fields expected, " . ($#output+1) . " encountered"; }
		return @output;
		}
	}

sub parse_line_corpus
	{
	my $line = shift;
	$line =~ s/\t\n$/\t[NONE]/;
	chomp $line;
	if ($line)
		{
		my @output = split (/\t/, $_);
		if ($#output < 3) { die "Invalid input format: >=4 fields expected, " . ($#output+1) . " encountered"; }
		if ($output[$#output] =~ /^\[$EXCL\]$/) { $output[$#output-1] = "UNK"; }
		return @output[-4..-1];
		}
	}

# # # # # # # # # # # # #
# COMMAND LINE OPTIONS
# # # # # # # # # # # # #

my ($source_type, $source_filename, $source_short_fname, $parse_line, $min_len, $depth);
my %opts;
getopts('d:e:i:l:t:', \%opts);

# -i: Input file
unless ($opts{"i"}) { die "No input file supplied"; }
elsif ($opts{"i"} !~ /\.txt$/i) { die "Invalid input file name (*.txt expected)"; }
else
	{
	$source_filename = $opts{"i"};
	$source_short_fname = extract_filename($source_filename);
	}

# -t: Input file type, base or corpus (base by default)
# The default output format of the grammatical database assembler is taken to be "base" format,
# while the default output format of the tagger is taken to be "corpus" format.
unless ($opts{"t"})
	{
	$source_type = "base";
	$parse_line = \&parse_line_base;
	}
elsif ($opts{"t"} ne "base" && $opts{"t"} ne "corpus") { die "Invalid -t flag ('base' or 'corpus' expected)"; }
else
	{
	$source_type = $opts{"t"};
	$parse_line = \&{"parse_line_" . $opts{"t"}};
	}

# -e: Exclusions (GUESS|COMP2eq|COMP2neq|COMP3|TAR by default)
# This option has effect in corpus mode only. If a line ends in any of the postfixes listed
# under -e flag (with pipe delimitation, no brackets), such a line will be safely ignored.
# Generally, GUESS'ed words should be withdrawn to prevent overfitting, and certain compound
# types may be hard to parse, so it would be safer to withdraw them as well.
if ($opts{"e"} && $opts{"t"} ne "corpus") { warn "Useless -e flag (effective in corpus mode only)"; }
elsif ($opts{"e"}) { $EXCL = $opts{"e"}; }

# -l: Minimal length of a to-be-processed word (3 by default)
unless ($opts{"l"}) { $min_len = 3; }
elsif ($opts{"l"} !~ /^\d+$/ || $opts{"l"} == 0) { die "Invalid -l flag (positive integer expected)"; }
else { $min_len = $opts{"l"}; }

# -d: Maximal number of symbols stripped off the right end of the word (5 by default)
unless ($opts{"d"}) { $depth = 5; }
elsif ($opts{"d"} !~ /^\d+$/ || $opts{"d"} == 0) { die "Invalid -d flag (positive integer expected)"; }
else { $depth = $opts{"d"}; }

# # # # # # # # # # # # #
# MAIN
# # # # # # # # # # # # #

my $start = new Benchmark;

print "Inducing suffix table with the following parameters:\n";
print "- input file destination: $source_filename\n";
print "- input file type: $source_type\n";
if ($source_type eq "corpus") { print "- omit tokens with marks: $EXCL\n"; }
print "- minimal token length: $min_len\n";
print "- maximal suffix length: $depth\n";

my %GUESS; # the suffix table
my ($total, $dropped);

open (my $filein, "<$source_filename") or die "Can't find $source_filename";
while (<$filein>)
	{
	if ($. % 10000 == 0) { printf "%u entries processed\r", $.; }
	++$total;
	
	my ($w, $lemma, $tag) = &$parse_line($_);
	if ($tag eq "UNK" || length($w) <= $min_len) { next; }
	my @lemmas = split (/\|/, $lemma);
	my @tags = map { [split (/\|/, $_)] } split (/\|{2}/, $tag);
	unless ($#lemmas == $#tags)
		{
		warn "Dropping invalid entry (lemmas don't match families of tags)";
		++$dropped;
		next;
		}
	my $limit = min(length($w), $depth);
	foreach my $len (1..$limit)
		{
		my $stem = substr($w, 0, length($w)-$len);
		my $suff_w = substr($w, length($w)-$len, $len);
		foreach my $i (0..$#lemmas)
			{
			if (length($lemmas[$i]) >= length($w)-$len && substr($lemmas[$i], 0, length($w)-$len) eq $stem)
				{
				my $suff_lemma = substr($lemmas[$i], length($w)-$len, length($lemmas[$i])-length($w)+$len);
				foreach my $tag (@{$tags[$i]})
					{ ++$GUESS{$suff_w}{$suff_lemma . "{" . $tag . "}"}; }
				}
			}
		}
	}
close ($filein);

my $output_filename = "guess_" . $source_short_fname . "_l" . $min_len . "_d" . $depth . ".txt";
open (my $fileout, ">$output_filename") or die "Can't write on $output_filename";
foreach my $suff (keys %GUESS)
	{
	my %distr = %{$GUESS{$suff}};
	print $fileout join ("\t", ($suff,
								sum_and_entropy(\%distr),
								join (" ", map { $_."=".$distr{$_} } sort { $distr{$b} <=> $distr{$a} } keys %distr)
								)) . "\n";
	}
close ($fileout);

# Performance statistics
my $end = new Benchmark;
my $diff = timediff($end, $start);
printf "Complete in %.2f seconds.\n", (${$diff}[1] + ${$diff}[2]);
printf "%u entries processed", $total;
if ($dropped) { printf ", %u of them dropped due to inconsistent format", $dropped; }
print ".\n";
