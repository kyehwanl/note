#!/usr/bin/perl

print "\n --- reverse quotation mark(`) \n";
$result = `vmstat 1 5`;
print $result;


print "\n --- input to pipe \n";
open (IN, "vmstat 1 5 |");
while (<IN>) 
{
    print $_;
}
close IN;

print "\n --- output from pipe \n";
open( OUT, "| sort -n");
foreach $i (5,1,10,2) 
{
    print OUT $i."\n";
}
close OUT;
