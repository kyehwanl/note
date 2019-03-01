#! /usr/bin/python

class IPstat:
    counter = 0
    keep = True
    maxNum = 100000


def printIPaddr(ip_attr, N):

    if IPstat.keep == True:
        print "ip addr: %3d %3d %3d %3d"\
                %(ip_attr[0][0],ip_attr[1][0],ip_attr[2][0],ip_attr[3][0]), '/', N
        IPstat.counter += 1                                                                                               

    if IPstat.counter >= IPstat.maxNum:
        IPstat.keep = False

def showGeneratePrefix(i, numPrefix):
    if numPrefix: print '+++ showing generate prefix'
    IPstat.maxNum = numPrefix

    A=1
    B=0
    C=0
    D=0
    N=0
    init = [A,B,C,D]
    A_max = 25
    B_max = 255
    C_max = 255
    D_max = 255
    N_start = 15
    N_end   = 29

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
        print "ip addr:", ip_attr

        p = N / 8  #  p+1 = layer --> ip_attr's index
        q = N % 8
        total = 2 ** q
        interval = 2 ** (8-q)

        ip_attr[p][2] = interval
        print "N: %2d" % N, " p:", p, " q:", q, " total number: %3d"% total, " interval: %3d"% interval
        print  ip_attr[p]


        # layer1
        while ( ip_attr[0][0] <= ip_attr[0][1] ):
            if IPstat.keep == False:
                break
            inc = ip_attr[0][2]
            if q == 0 and p == 1: 
                printIPaddr(ip_attr, N)
                ip_attr[0][0] += inc
                continue

            # layer 2
            ip_attr[1][0] = B
            while (ip_attr[1][0] <= ip_attr[1][1]):
                if IPstat.keep == False:
                    break
                if q == 0 and p == 2: # layer 2
                    printIPaddr(ip_attr, N)
                    inc = ip_attr[1][2]
                    ip_attr[1][0] += inc
                    continue
                if p == 1 :
                    printIPaddr(ip_attr, N)
                    inc = interval 
                    ip_attr[1][0] += inc
                    continue

                elif p >= 2:
            
                    # layer 3
                    ip_attr[2][0] = C
                    while ( ip_attr[2][0] <= ip_attr[2][1] ):
                        if IPstat.keep == False:
                            break
                        if q == 0 and p == 3: # layer 3
                            printIPaddr(ip_attr, N)
                            ip_attr[2][0] += ip_attr[2][2]
                            continue

                        if p == 2:
                            printIPaddr(ip_attr, N)
                            ip_attr[2][0] += ip_attr[2][2]
                            continue
                    
                        elif p >= 3:
                            # layer 4
                            ip_attr[3][0] = D
                            while ( ip_attr[3][0] <= ip_attr[3][1] ):
                                if IPstat.keep == False:
                                    break
                                if q == 0 and p == 4: 
                                    printIPaddr(ip_attr, N)
                                    ip_attr[3][0] += ip_attr[3][2]
                                    continue

                                if p == 3:
                                    printIPaddr(ip_attr, N)
                                    ip_attr[3][0] += ip_attr[3][2]
                                    continue


                        # step increase for layer3
                        ip_attr[2][0] += ip_attr[2][2]                                                                    


                # step increase for layer2
                ip_attr[1][0] += ip_attr[1][2]

            
            # step increase for layer1
            ip_attr[0][0] += ip_attr[0][2]

        

    
    print "total count: %d" % IPstat.counter
                                           
                                            
    #buf =  'network '+str(i+2)+'.'+str(scnd)+'.'+str(n%256)+'.'+ '0/24'+'\n' 
    #buf =  'network '+str(i+2)+'.'+str(scnd)+'.'+str(thrd)+'.'+ str((n*2)%256)+'/31'+'\n' 
                                                                                           
                                                                                           
                                                                                    




def main():
    i=0
    numPrefix = 150
    showGeneratePrefix(i, numPrefix)

if __name__ == "__main__":
    
    main()






