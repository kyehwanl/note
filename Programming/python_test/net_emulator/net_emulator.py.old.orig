#!/usr/bin/env python



import getopt, sys, os
import subprocess

max_count = 500
max_prefix = 600000

dry_run=False
debug_output=False

class bcolors:
    HEADER = '\033[95m'     # purple
    OKBLUE = '\033[94m'     # blue
    OKGREEN = '\033[92m'    # green
    WARNING = '\033[93m'    # yellow
    FAIL = '\033[91m'       # red
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def usage():
    print
    print "usage: %s <option switch> [value]" % sys.modules['__main__'].__file__
    print "  options: "
    print "\t-c, --count      : <value> instance_count"
    print "\t-s, --start      : <value> start_point"
    print "\t-b, --bgpsec     : bgpsec enable"
    print "\t-d, --dry-run    : dry run test"
    print "\t-n, --ns-only    : generate networkname space nodes only"
    print "\t-f, --conf-only  : generate bgp configuration only"
    print "\t-o, --output     : debug output enable/disable "
    print "\t-p, --path       : <path>, directory path for bgpd "
    print "\t-r, --remove     : <0|1>, to remove setting"
    print "\t-x, --prefix     : <number>, the number of prefixes to be generated"
    print "\t-h, --help       : help screen"
    print

def cmdProcess(command):
    if dry_run == True:
        if debug_output == True:
            print bcolors.OKGREEN, 'dry_run: ', command, bcolors.ENDC
        return None

    if debug_output == True:
        print bcolors.BOLD+ "command: ", command, bcolors.ENDC
    process = subprocess.Popen(command,
            shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    process.wait()
    output  = process.stdout.read()
    errout  = process.stderr.read()
    output1 = process.stdout.read().rstrip()

    #if output: print "output :", output
    if debug_output == True:
        if output: print  bcolors.OKGREEN+output+bcolors.ENDC
    if errout: print  bcolors.FAIL+errout+bcolors.ENDC
    return output, errout


def cmdProcess2(command):
    if debug_output == True:
        print bcolors.BOLD+"command: ", command, bcolors.ENDC
    process = subprocess.Popen(command,
            shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    process.wait()


    ret = []
    while True :
        line = process.stdout.readline()
        if line != '':
            ret.append(line.rstrip())
        else:
            break

    if debug_output==True and ret:
        print bcolors.OKGREEN,ret,bcolors.ENDC

    #print "return value: ", [ i for i in reversed(ret) if i != '']
    return [ i for i in reversed(ret) if i != '']



def configure_bridge(bridge):

    print "+++ bridge configuration start "
    str1 = ["brctl addbr", bridge]
    command = " ".join(str1)
    retStr=cmdProcess(command)

    str2 = ["brctl stp", bridge, "off"]
    command = " ".join(str2)
    retStr=cmdProcess(command)

    str3 = ["ifconfig", bridge, "10.1.1.1/24 up"]
    command = " ".join(str3)
    retStr=cmdProcess(command)

    return command, retStr


def removeSetting(bridge):
    print "+++ remove all setting called"

    # remove network namespace nodes
    ret = []
    str1 = ["ip netns | awk '{print $1}'"]
    ret=cmdProcess2(str1)
    #print "return: ", ret

    print "+++ remove network namespace instances"
    for i in range(len(ret)):
        #print ret[i]
        str1 = "ip netns del "+ ret[i]
        cmdProcess(str1)


    # remove bridge interfaces
    print "+++ remove bridge interfaces"
    str1 = ["brctl show", bridge, "| grep veth | awk '{print $4}'"]
    command = " ".join(str1)
    ret=cmdProcess2(command)
    #print "return: ", ret

    for i in range(len(ret)):
        #print ret[i]
        str1 = "brctl delif "+bridge+" "+ret[i]
        cmdProcess(str1)



    # remove bridge
    str1 = ["brctl show | grep", bridge, "| awk '{print $1}'"]
    command = " ".join(str1)
    ret=cmdProcess2(command)
    #print "return: ", ret
    for i in range(len(ret)):
        if bridge == ret[i]:
            print "+++ remove bridge: ", bridge
            str1 = "ip link del "+bridge
            cmdProcess(str1)
            str1 = "brctl delbr "+bridge+" > /dev/null 2>&1"
            cmdProcess(str1)

    # remove bgpd process
    print "+++ remove bgpd processes "
    str1 = "pgrep bgpd"
    ret=cmdProcess2(str1)
    #print "return: ", ret
    for i in range(len(ret)):
        #print "kill bgpd process", ret[i]
        str1 = "kill -9 "+ret[i]
        cmdProcess(str1)


    #just in case, there are still remaining veth interfaces
    command= "ifconfig| grep veth | awk -F ':' '{print $1}'"
    ret=cmdProcess2(command)

    for i in range(len(ret)):
        str1 = "ip link del "+ret[i]
        cmdProcess(str1)

    print



def generateBaseHeader(f, i, bgpsec):
    headers = [
            'hostname node'+str(i+2)+"\n",
            'password z'+"\n",
            'router bgp '+str(i+2+60000)+"\n",
            'bgp router-id 10.1.1.'+str(i+2)+"\n",
            '\n'
            ]

    bgpsecHeaders = [
            'srx bgpsec ski 0 1 C30433FA1975FF193181458FB902B501EA9789DC\n',
            'srx bgpsec active 0\n',
            'srx connect localhost 17900\n',
            'srx evaluation bgpsec\n',
            ]

    for header in headers:
        f.write(header)


    if bgpsec==True:
        for header in bgpsecHeaders:
            f.write(header)





def generateNodeHeader(nodeFile, i):
    headers = [
            'hostname node'+str(i+2)+"\n",
            'password z'+"\n",
            'router bgp '+str(i+2+60000)+"\n",
            'bgp router-id 10.1.1.'+str(i+2)+"\n",
            'neighbor 10.1.1.1 remote-as 60001'+"\n",
            '\n'
            ]

    # in case, base file case
    if i == -1:
        for i in range(len(headers[:-2])):
            nodeFile.write(headers[i])
            #print headers[i].rstrip()
        return

    # cases of normal
    for header in headers:
        nodeFile.write(header)



def generateNodeBodyTail(nodeFile, i, bgpsec):
    tails = [
            '\n',
            'line vty \n',
            'log stdout debugging \n',
            'debug bgp \n',
            'debug bgp fsm \n',
            'debug bgp updates \n',
            'debug bgp keepalives \n',
            'debug bgp events \n',
            'debug bgp filters \n',
            '\n'
            ]
    bgpsecTails = [
            'debug bgp bgpsec\n',
            'debug bgp bgpsec detail\n',
            'debug bgp bgpsec out\n',
            'debug bgp bgpsec in\n',
            ]

    bgpsecBody=[
            '\n',
            'srx bgpsec ski 0 1 45CAD0AC44F77EFAA94602E9984305215BF47DCD \n',
            'srx bgpsec active 0 \n',
            'srx connect localhost 17900 \n',
            'srx evaluation bgpsec \n',
            'neighbor 10.1.1.1 bgpsec both \n',
            '\n'
            ]

    # in case of base file
    if i == -1:
        for tail in tails:
            nodeFile.write(tail)
        if bgpsec == True:
            for bs in bgpsecTails:
                nodeFile.write(bs)
        return


    if bgpsec == True:
        for bs in bgpsecBody:
            nodeFile.write(bs)

    for tail in tails:
        nodeFile.write(tail)

    if bgpsec == True:
        for bs in bgpsecTails:
            nodeFile.write(bs)





def generatePrefix(nodeFile, i, numPrefix):
    if numPrefix: print '+++ generate prefix'

    for n in range(numPrefix):
        buf =  'network 10.'+str(i+2)+'.'+str(n+1)+'.'+ '0/24'+'\n'
        if debug_output == True:
            print buf.rstrip()
        nodeFile.write(buf)




def main():

    # check root-level privileges
    if  os.getuid() != 0:
        print '\033[91m'+"Need root permissions to do this\n",'\033[0m'
        sys.exit(1)

    try:
        opts, args = getopt.getopt(sys.argv[1:], "bc:dfhnp:op:rs:vx:",
                ["help", "output=", "bgpsec", "dry-run", "path="])
    except getopt.GetoptError as err:
        # print help information and exit:
        print bcolors.FAIL+str(err),bcolors.ENDC  # will print something like "option -a not recognized"
        usage()
        sys.exit(2)

    bgpsec=0
    count=0
    ns_start=0
    global dry_run
    global debug_output
    numPrefix = 0
    conf_only = False
    ns_only = False
    path_cwd = os.getcwd()+'/'
    path = path_cwd
    remove = False
    start_ipaddr=2
    bridge="br1"

    if len(sys.argv) < 2:
        usage()
        sys.exit(2)


    output = None
    for o, a in opts:
        if o in ("-b", "--bgpsec"):
            bgpsec = True
        elif o == "-c":
            count = int(a)
        elif o in ("-d", "--dry-run"):
            dry_run = True
        elif o == "-f":
            conf_only = True
        elif o == "-n":
            ns_only = True
        elif o in ("-p", "--path"):
            path = a
        elif o == "-r":
            remove = True
        elif o == "-s":
            ns_start = int(a)
        elif o == "-x":
            numPrefix = int (a)
        elif o in ("-h", "--help"):
            usage()
            sys.exit()
        elif o in ("-o", "--output"):
            output = True
        else:
            assert False, "unhandled option"
    # ...



    """
     condition check
    """
    if count > max_count:
        count = max_count
        print bcolors.WARNING+'set count max\n',bcolors.ENDC

    if numPrefix > max_prefix:
        numPrefix = max_prefix
        print bcolors.WARNING+'set prefix number max\n',bcolors.ENDC

    if ns_only==True and conf_only==True:
        print bcolors.FAIL+"Error: both cannot be True\n",bcolors.ENDC
        sys.exit()

    if count == 0 and remove != True:
        print bcolors.FAIL+'need to set a count value >= 1\n',bcolors.ENDC
        sys.exit(1)

    if output != None and output != False:
        debug_output = True

    if remove == True:
        removeSetting(bridge)
        sys.exit()

    if dry_run == True:
        print bcolors.FAIL+"DRY-RUN Test Start\n",bcolors.ENDC



    """
     initial condition display
    """
    if debug_output:
        print '\n-------------------------'
        print 'init count   :', count
        print 'start point  :', ns_start
        print 'bgpsec       :', bgpsec
        print 'dry-run      :', dry_run
        print 'conf-only    :', conf_only
        print 'ns-only      :', ns_only
        print 'num-Prefix   :', numPrefix
        print 'path         :', path
        print 'remove       :', remove
        print 'debug output :', debug_output
        print '-------------------------\n'



    """
     prerequisite tasks: check bridge configuration
    """
    print bcolors.WARNING+"[+] prerequisite tasks: check netns support",bcolors.ENDC
    if dry_run != True:
        command= "ip netns"
        retStr,errout=cmdProcess(command)
        if 'unknown' in errout: #'Object "netns" is unknown'
            print bcolors.FAIL+"This OS doesn't support 'netns'. Need to install to support netns\n",bcolors.ENDC
            sys.exit()

        command= "brctl show"
        retStr,errout=cmdProcess(command)
        #The program 'brctl' is currently not installed. To run 'brctl' please ask your administrator to install the package 'bridge-utils'
        if 'not installed' in errout or 'not found' in errout:
            print bcolors.FAIL+"brctl is not installed. Need to install to brctl\n",bcolors.ENDC
            sys.exit()



    if conf_only != True:
        print bcolors.WARNING+"[+] prerequisite tasks: check linux bridge instace",bcolors.ENDC
        str1 = ["brctl show | grep", bridge, "| awk '{print $1}'"]
        command = " ".join(str1)
        retStr=cmdProcess2(command)

        exist=False
        for i in range(len(retStr)):
            if retStr[i] == bridge:
                exist = True
                break

        if exist == False:
            print '+++ bridge instance not exist --> configure bridge'
            configure_bridge(bridge)




    """
     create bgpd configuration base file (recv side)
    """
    baseFile=None
    file_recv = "bgpd.conf.base"
    if bgpsec==True:
        file_recv = file_recv+".bgpsec"
        #print "file recv: ", file_recv

    if ns_only != True:
        print bcolors.WARNING+"[+] generate configuration files",bcolors.ENDC
        if dry_run != True:
            print "\n+++ generate bgpd configuration files: ", file_recv
            baseFile = open(file_recv,'w')
            generateBaseHeader(baseFile, -1, bgpsec)
        else:
            print "+++ file", file_recv, "generated"


        for i in range(ns_start, count+ns_start):

            file_name="bgpd.conf.n"+str(i+2)
            if bgpsec==True:
                file_name = file_name+".bgpsec"
            print "+++ generate bgpd configuration files: ", file_name

            if dry_run != True:
                nodeFile=open(file_name, 'w')
                generateNodeHeader(nodeFile, i)
                generatePrefix(nodeFile, i, numPrefix)
                generateNodeBodyTail(nodeFile, i, bgpsec)
                nodeFile.close()

                # base file contents regarding to neighbor and bgpsec
                baseTmpBuf = 'neighbor 10.1.1.'+str(i+2)+ ' remote-as '+str(i+2+60000)+'\n'
                baseFile.write(baseTmpBuf)

                if bgpsec == True:
                    baseTmpBuf = 'neighbor 10.1.1.'+str(i+2)+ ' bgpsec both'+'\n'
                    baseFile.write(baseTmpBuf)



        if baseFile != None:
            generateNodeBodyTail(baseFile, -1, bgpsec)
            baseFile.close()



    """
     netns setting
    """
    if conf_only != True:
        print bcolors.WARNING+"[+] network namespace setting and bgp running",bcolors.ENDC
        for i in range(ns_start, count+ns_start):
            print "+++ create network namespace nodes [%d]" %i
            file_name="bgpd.conf.n"+str(i+2)
            if bgpsec==True:
                file_name = file_name+".bgpsec"

            strs = ["ip netns add ns"+str(i)]
            command = " ".join(strs)
            retStr=cmdProcess(command)

            strs = ["ip link add veth"+str(i*2), "type veth peer name veth"+str(i*2+1)]
            command = " ".join(strs)
            retStr=cmdProcess(command)

            strs = ["ip link set veth"+str(i*2+1), "netns ns"+str(i)]
            command = " ".join(strs)
            retStr=cmdProcess(command)

            strs = ["ip netns exec ns"+str(i), "ifconfig lo up"]
            command = " ".join(strs)
            retStr=cmdProcess(command)

            strs = ["ip netns exec ns"+str(i), "ifconfig veth"+str(i*2+1),
                    "10.1.1."+str(start_ipaddr+i)+"/24 up"]
            command = " ".join(strs)
            retStr=cmdProcess(command)

            strs = ["brctl addif", bridge, "veth"+str(i*2)]
            command = " ".join(strs)
            retStr=cmdProcess(command)

            strs = ["ip link set dev veth"+str(i*2), "up"]
            command = " ".join(strs)
            retStr=cmdProcess(command)

            # run bgp routing
            print "+++ run bgp routing [%d]" % i
            strs = ["ip netns exec ns"+str(i), "bash -c 'ifconfig;pwd'"]
            command = " ".join(strs)
            retStr=cmdProcess(command)

            strs = ["ip netns exec ns"+str(i),
                    "bash -c '"+path+"bgpd -f "+path_cwd+file_name+" -i /tmp/node"+str(i+2)+".pid -d'"]
            command = " ".join(strs)
            retStr=cmdProcess(command)

            strs = ["ps aux | grep bgp[d] | grep n"+str(i+2), "| awk '{print $2}'"]
            command = " ".join(strs)
            retStr=cmdProcess(command)
            print



if __name__ == "__main__":
    main()













