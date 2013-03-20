

$instring=@ARGV[0];
$oldPattern=@ARGV[1];

#print "This is what was entered $instring $oldPattern $newPattern\n";
$instring =~ tr/(\W)$oldPattern\-(\W)/$1$newPattern\*2\-$1/;
print $instring;
