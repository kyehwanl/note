-d 20   -> you only get ROAS up to prefix length and
           max length of prefixes of 20 (32 -- default)
         
           a/10 20, b/18 20   OK
           a/21 22, b/18 22   both not OK

-l 2    -> number of levels down the tree 
           a/10, a1/12, a2/14, ....
-l 3       a/10, a1/13, a2/116, ....

-m 2    -> max length of roa == prefix length + value of m
           a/10 12, a1/11 13, a2/12 14
-m 3       a/10 13, a1/11 14, ....

-A --adjust Allows to addjust all values
     -m 4 -d 32 (no -A) 
           a/26 30, a1/27 31, a2/28 32 (stop) 
     -m 4 -d 32 -A
           a/26 30, a1/27 31, a2/28 32, a3/29 32, a4/30 32, ...

     -m 4 -d 20 -A
           a/15 19, a1/16 20, a2/17 20, a3/18 20

-A affects -d and -m

Generate 100 ROA Add's starting with 10.0.0.0/8 max length prefix-length+4
maximum prefix depth is /20 (incl. ROA max len) and skip every other prefix 
level (/8 /10 /12 ....)
php rpki_rtr_feed.php -r 10.0.0.0/8 -d 20 -m 4 -l 2 -v -n 100
     


-- this is my configuration command to make 100k
php rpki_rtr_feed.php -r 10.0.0.0/8 -d 32 -m 4 -l 1 -n 100000 > srx_rpki_data_100k
