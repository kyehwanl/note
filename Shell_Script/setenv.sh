#!/bin/bash
#
# Execute with ". ./setenv.sh"

LOCAL="$PWD/_local-$HOSTNAME"
echo "Local = $LOCAL"

# Local - binaries
export PATH="$PATH:$LOCAL/bin"

# Local - man pages
export MANPATH="$MANPATH:$LOCAL/man"

# Local - shared library
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$LOCAL/lib"

# Perl modules
PMOD5="$LOCAL/lib/perl5"
PARCH="i386-linux-thread-multi"
PERL5LIB="$PERL5LIB:$PMOD5/5.8.8:$PMOD5/5.8.8/$PARCH"
export PERL5LIB="$PERL5LIB:$PMOD5/site_perl/5.8.8:$PMOD5/site_perl/5.8.8/$PARCH"

# Maven
JAVADIR="$PWD/_java"
export PATH="$PATH:$JAVADIR/apache-maven/bin"

# Java2EE
export PATH="$PATH:$JAVADIR/SDK/bin"
export CLASSPATH="$CLASSPATH:$PWD/_java/SDK/lib/j2ee.jar"

# BBN software
export APKI_DB="BBN_APKI"
export APKI_PORT=7344
export APKI_ROOT="$PWD/BBN_RPKI_software-$HOSTNAME"

