

class IPstat:
    keep = False
    maxNum = 600000
    counter = 0

    def __init__(self, num):
        self.counter = 0
        self.maxNum = num
        self.keep = True

##
# @brief print IP address either terminal or file descriptor
#
# @param ip_attr  : dict variable which contains each ip ranges
# @param N        : prefix length
# @param nodeFile : file descriptor to read or write
# @param dry_run  : boolean variable to enable / disable test run
#
# @return
def printIPaddr(ip_attr, N, nodeFile, output, dry_run, mode, nexthop):

    debug_output=False
    if output != None and output != False:
        debug_output = True

    if dry_run == True:

        if IPstat.keep == True:
            if debug_output == True:
                if mode == 'quagga':
                    print "network %d.%d.%d.%d"\
                        %(ip_attr[0][0],ip_attr[1][0],ip_attr[2][0],ip_attr[3][0])+ '/'+str(N)
                elif mode == 'exabgp':
                    print "route %d.%d.%d.%d"\
                        %(ip_attr[0][0],ip_attr[1][0],ip_attr[2][0],ip_attr[3][0])+ '/'+str(N) \
                            +' next-hop '+nexthop+';'
            IPstat.counter += 1

        if IPstat.counter >= IPstat.maxNum:
            IPstat.keep = False

    else:
        if nodeFile != None:
            if IPstat.keep == True:
                if mode == 'quagga':
                    buf = "network %d.%d.%d.%d"\
                        %(ip_attr[0][0],ip_attr[1][0],ip_attr[2][0],ip_attr[3][0])+ '/'+str(N)+'\n'
                elif mode == 'exabgp':
                    buf = "\t\troute %d.%d.%d.%d"\
                        %(ip_attr[0][0],ip_attr[1][0],ip_attr[2][0],ip_attr[3][0])+ '/'+str(N) \
                            +'   \tnext-hop '+nexthop+';'+'\n'

                if debug_output == True:
                    print buf.rstrip()
                nodeFile.write(buf)

                IPstat.counter += 1

            if IPstat.counter >= IPstat.maxNum:
                IPstat.keep = False

        else:
            print "File not exist"
            return



    #buf =  'network '+str(i+2)+'.'+str(scnd)+'.'+str(n%256)+'.'+ '0/24'+'\n'
    #buf =  'network '+str(i+2)+'.'+str(scnd)+'.'+str(thrd)+'.'+ str((n*2)%256)+'/31'+'\n'



##
# @brief
#
# @param nodeFile   : file descriptor to read or write
# @param i          : max prefix length
# @param numPrefix  : max limit of total number of prefixes to generate
# @param dry_run    : boolean variable to enable / disable test run
#
# @return
def generatePrefix(nodeFile, i, numPrefix, output, dry_run, mode, nexthop):
    if numPrefix and output: print '+++ showing generate prefix'
    IPstat.maxNum = numPrefix
    IPstat.counter = 0
    IPstat.keep = True

    #default init and max
    A=1
    B=0
    C=0
    D=0
    N=0
    init = [A,B,C,D]
    A_max = 224
    B_max = 255
    C_max = 255
    D_max = 255
    N_start = 8
    N_end   = 32

    if i > 1:
        A = i
        A_max = i


    #  [value, max, step]
    ip0 = [A,A_max,1]
    ip1 = [B,B_max,1]
    ip2 = [C,C_max,1]
    ip3 = [D,D_max,1]
    #ip_attr={0: ip0, 1:ip1, 2:ip2, 3:ip3 }

    layer =0
    inc = 1
    layer_break = False

    #for N in range(8,31):
    for N in range(N_start, N_end):
        # init
        ip_attr={0: [A,A_max,1], 1:[B,B_max,1], 2:[C,C_max,1], 3:[D,D_max,1] }
        #print "ip attr:", ip_attr

        p = N / 8  #  p+1 = layer --> ip_attr's index
        q = N % 8
        total = 2 ** q
        interval = 2 ** (8-q)

        ip_attr[p][2] = interval
        #print "N: %2d" % N, " p:", p, " q:", q, " total number: %3d"% total, " interval: %3d"% interval
        #print  ip_attr[p]


        # layer1 : prefix length upto /8
        while ( ip_attr[0][0] <= ip_attr[0][1] ):
            if IPstat.keep == False:
                break
            inc = ip_attr[0][2]
            if q == 0 and p == 1:
                printIPaddr(ip_attr, N, nodeFile, output, dry_run, mode, nexthop)
                ip_attr[0][0] += inc
                continue

            # layer 2 : prefix length /9 - /16
            ip_attr[1][0] = B
            while (ip_attr[1][0] <= ip_attr[1][1]):
                if IPstat.keep == False:
                    break
                if q == 0 and p == 2: # layer 2
                    printIPaddr(ip_attr, N, nodeFile, output, dry_run, mode, nexthop)
                    inc = ip_attr[1][2]
                    ip_attr[1][0] += inc
                    continue
                if p == 1 :
                    printIPaddr(ip_attr, N, nodeFile, output, dry_run, mode, nexthop)
                    inc = interval
                    ip_attr[1][0] += inc
                    continue

                elif p >= 2:

                    # layer 3 : prefix length /17 - /24
                    ip_attr[2][0] = C
                    while ( ip_attr[2][0] <= ip_attr[2][1] ):
                        if IPstat.keep == False:
                            break
                        if q == 0 and p == 3: # layer 3
                            printIPaddr(ip_attr, N, nodeFile, output, dry_run, mode, nexthop)
                            ip_attr[2][0] += ip_attr[2][2]
                            continue

                        if p == 2:
                            printIPaddr(ip_attr, N, nodeFile, output, dry_run, mode, nexthop)
                            ip_attr[2][0] += ip_attr[2][2]
                            continue

                        elif p >= 3:
                            # layer 4
                            ip_attr[3][0] = D
                            while ( ip_attr[3][0] <= ip_attr[3][1] ):
                                if IPstat.keep == False:
                                    break
                                if q == 0 and p == 4:
                                    printIPaddr(ip_attr, N, nodeFile, output, dry_run, mode, nexthop)
                                    ip_attr[3][0] += ip_attr[3][2]
                                    continue

                                if p == 3:
                                    printIPaddr(ip_attr, N, nodeFile, output, dry_run, mode, nexthop)
                                    ip_attr[3][0] += ip_attr[3][2]
                                    continue


                        # step increase for layer3
                        ip_attr[2][0] += ip_attr[2][2]


                # step increase for layer2
                ip_attr[1][0] += ip_attr[1][2]


            # step increase for layer1
            ip_attr[0][0] += ip_attr[0][2]


    #print "total count: %d" % IPstat.counter





