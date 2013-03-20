

$instring=@ARGV[0];
$oldPattern=@ARGV[1];

#print "This is what was entered $instring $oldPattern $newPattern\n";
if($instring =~ m/$oldPattern\-/i)
	{print "Y";}
else
	{print "N";};

