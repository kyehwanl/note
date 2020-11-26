#! /bin/sh -x

#
# Only different sources are allowed to be overwritten, 
# Otherwise, new files, especially configure files, will overwrite all the configs on this machine
#
function dry_run()
{
  rsync -avPuniz --exclude=".git*" --include='*.[ch]' --exclude='_inst' --exclude='*.libs' --exclude='*.deps*' --exclude='*.o' --exclude='autom4te*' --exclude='*.l[oa]' --include='Makefile.am' --exclude='Makefile*' --include='configure.ac' --exclude='config*' --exclude='*.sw*' --exclude="*~" --exclude="*cscope*" --exclude="*tags" --exclude="*.so*" --exclude="rpkirtr_client" --exclude="rpkirtr_svr" --exclude="srx_server" --exclude="srxsvr_client" --exclude="libtool" --exclude="srx_serverd" --exclude="srxproxy64.conf" --exclude="*.spec"  access.antd.nist.gov:~/proj-program_data/Github_usnistgov/NIST-BGP-SRx/ ~/project/Github_usnistgov/NIST-BGP-SRx/ --delete
}

function run_sync()
{
  #rsync -avPuiz  --exclude=".git*" --include='*.[ch]' --exclude='_inst' --exclude='*.libs' --exclude='*.deps*' --exclude='*.o' --exclude='autom4te*' --exclude='*.l[oa]' --include='Makefile.am' --exclude='Makefile*' --include='configure.ac' --exclude='config*' --exclude='*.sw*' --exclude="*~" --exclude="*cscope*" --exclude="*tags" --exclude="*.so*" --exclude="rpkirtr_client" --exclude="rpkirtr_svr" --exclude="srx_server" --exclude="srxsvr_client" --exclude="libtool" --exclude="srx_serverd" --exclude="srxproxy64.conf" --exclude="*.spec"  access.antd.nist.gov:~/proj-program_data/Github_usnistgov/NIST-BGP-SRx/ ~/project/Github_usnistgov/NIST-BGP-SRx/ --delete
}

var=$1
: ${var:=nothing}

if [ "$var" = "nothing" ]; then
  dry_run
elif [ "$var" = "sync" ]; then
  run_sync
fi
