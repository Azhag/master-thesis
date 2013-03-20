

$instring=@ARGV[0];
$oldPattern=@ARGV[1];
$newPattern=@ARGV[2];


#print "This is what was entered $instring $oldPattern $newPattern\n";
$instring =~ s/^$oldPattern$/$1$newPattern$2/g;
$instring =~ s/(\W)$oldPattern(\W)/$1$newPattern$2/g;
$instring =~ s/^$oldPattern(\W)/$newPattern$1/g;
$instring =~ s/(\W)$oldPattern$/$1$newPattern/g;
print $instring;
