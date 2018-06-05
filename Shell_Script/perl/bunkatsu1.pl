#!/usr/bin/perl

use strict;

sub  split_words {
    my $word;
    my @words;

    while (<>) {
        chomp;
        @words = split ( /\s+/, $_ );
        foreach $word (@words) {
            print $word . "\n";
        }
    }

}

MAIN: {
    split_words();
}
