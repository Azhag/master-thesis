

$instring=@ARGV[0];
$oldPattern=@ARGV[1];
$newPattern=@ARGV[2];


#print "This is what was entered $instring $oldPattern $newPattern\n";
$instring =~ s/$oldPattern\*/$1$newPattern$2/g;
$instring =~ s/\*$oldPattern/$1$newPattern$2/g;
print $instring;
