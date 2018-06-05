#!/usr/bin/perl

use strict;

sub  split_words {
    my ($word, $pre);
    my (@words, @bigram);

    $pre ="";
    while (<>) {
        chomp;
        @words = split ( /\s+/, $_ );
        foreach $word (@words) {
            if( $pre eq "You") {
                push( @bigram, $word);
            }
            $pre = $word;
        }
    }
    return @bigram;

}

MAIN: {
    my $word;
    my @bigram;

    @bigram = split_words();

    foreach $word ( @bigram) {
        print $word . "\n";
    }
}
