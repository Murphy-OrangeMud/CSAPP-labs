#!/usr/bin/perl

use Getopt::Std;

$progdir = "./bddcheck";
$Fsolve = "bits.c";
$Fref = "tests.c";
$Finfo = "$progdir/all-functions.txt";
$FLAGS = "-t";
$PROG = "$progdir/checkprogs.pl";

getopts('hgtvkf:r:');

if ($opt_h) {
  print STDOUT "Usage $0 [-hgkv] [-f fun]\n";
  print STDOUT "     -h         Print this message\n";
  print STDOUT "     -g         Generate grading information\n";
  print STDOUT "     -k         Keep intermediate files\n";
  print STDOUT "     -v         Verbose mode\n";
  print STDOUT "     -f fun     Test only function fun\n";
  print STDOUT "     -r rating  Set rating of all functions\n";
  exit(0);
}

if ($opt_g) {
  printf "Score\tRating\tErrors\tFunction\n";
  $FLAGS = "";
}

if ($opt_k) {
  $FLAGS = "$FLAGS -k";
}

if ($opt_v) {
  $FLAGS = "$FLAGS -v";
  $verbose = 1;
} else {
  $verbose = 0;
}

# Set time limit
$FLAGS = $FLAGS . " -T 30";

# # ======================= my code begin
# # given a file, decode it, generate a tmp file which is decoded, use it , and then delete it.
# system "cd ../";    # enter the upper dir
# $decodeinfile = "tests.c";
# $duplicatefile = "tests-save.c";

# open(PYTHONFILE, ">tt.py") || die("Couldn't open test file $testfile\n");

# print PYTHONFILE <<HERE;

# import base64
# import argparse

# parser = argparse.ArgumentParser()
# parser.add_argument('-infile', '-f', default='tests.c', type=str, help = 'choose a file') 
# parser.add_argument('-outfile', '-F', default='tests_decoder.c', type=str, help = 'choose a file') 
# args = parser.parse_args()

# f = open(args.infile, 'r')
# lines = f.readlines()
# f.close()

# f_out = open(args.outfile, 'w')


# for line in lines:
#   s1 = base64.decodestring(line)
#   f_out.write(s1)
# f_out.close()
# HERE

# system "cp -f $decodeinfile $duplicatefile";
# system "python tt.py -infile $decodeinfile -outfile $decodeinfile";
# system("rm -rf tt.py");
# system("cd bddcheck");

# # ================ my code end


$totalscore = 0;
$maxscore = 0;
$passed = 0;
$attempted = 0;

if ($opt_f) {
  if ($verbose) {
    print "Looking for function '$opt_f' in $Finfo\n";
  }
  $entry = `grep \"$opt_f \" $Finfo` || die "Couldn't find entry for function '$opt_f'\n";
  chomp $entry;
  ($fun,$rating,$pat) = split(/[ \t]+/, $entry);
  if ($opt_r) {
    $rating = $opt_r;
  }
  $patarg = "";
  if (!($pat eq "all")) {
    $patarg = " -a " . $pat;
  }
  if ($verbose) {
    print "Executing $PROG $FLAGS $patarg -f $Fsolve -p $fun -F $Fref -P test_$fun\n";
  }
  $result =  `$PROG $FLAGS $patarg -f $Fsolve -p $fun -F $Fref -P test_$fun` ||
    die "Executing '$PROG $FLAGS $patarg -f $Fsolve -p $fun -F $Fref -P test_$fun gives ERROR\n$result";
  if (!$opt_g) {
    print $result;
  }
  $attempted++;
  $maxscore += $rating;
  $ok = !($result =~ /\.\. Mismatch/ || $result =~ /Error/);
  $errors = 1-$ok;
  $passed += $ok;
  $score = ($ok*$rating);
  $totalscore += $score;
  if ($opt_g) {
    print " $score\t$rating\t$errors\t$fun\n";
  } else {
    print "Check $fun score: $score/$rating\n";
  }
} else {
  # Try out all functions in file
  open TESTFILE, $Fref || die "Couldn't open reference file $Fref\n";
  while (<TESTFILE>) {
    $line = $_;
  # my code here
    if ($line =~ /[\s]*(unsigned|int|long long)[\s]+test_([a-zA-Z0-9_]+)[\s]*\(/) {
      $funname = $2;
      if ($verbose) {
  print "Testing function '$funname'\n";
  print "Looking for function '$funname' in $Finfo\n";
      }
      $entry = `grep \"$funname \" $Finfo` || die "Couldn't find entry for function '$funname'\n";
      chomp $entry;
      ($fun,$rating,$pat) = split(/[ \t]+/, $entry);
      if ($opt_r) {
  $rating = $opt_r;
      }
  
  
      $patarg = "";
      if (!($pat eq "all")) {
  $patarg = " -a " . $pat;
      }
      if ($verbose) {
  print "Executing $PROG $FLAGS $patarg -f $Fsolve -p $fun -F $Fref -P test_$fun\n";
      }
      $result =  `$PROG $FLAGS $patarg -f $Fsolve -p $fun -F $Fref -P test_$fun` ||
  "Executing '$PROG $FLAGS $patarg -f $Fsolve -p $fun -F $Fref -P test_$fun' gives Error\n";
      if (!$opt_g) {
  print $result;
      }

  # my code here
  if ($rating)
  {
        $attempted++;
        $maxscore += $rating;
        $ok = !($result =~ /\.\. Mismatch/ || $result =~ /Error/);
        $errors = 1-$ok;
        $passed += $ok;
        $score = ($ok*$rating);
        $totalscore += $score;
  }
  # my code end
      if ($opt_g) {
  print " $score\t$rating\t$errors\t$fun\n";
      } else {
  print "Check $fun score: $score/$rating\n";
      }
    }
  }
}

if (!$opt_g) {
  print "Overall correctness score: $totalscore/$maxscore\n";
  if ($passed == $attempted) {
    print "All tests passed\n";
  }
} else {
  print "Total points: $totalscore/$maxscore\n";
}

# # ======== my code begin
# system("cd ../");

# system "rm -rf $decodeinfile";
# system "mv $duplicatefile $decodeinfile"; 

# #  ============ my code end