package main

import (
    "fmt"
    )

func main() {
  fmt.Println("Hello, playground")    

    var a uint32
    a = 0x12345678

    fmt.Printf("a = Ox%08x\n", a)
    fmt.Printf("a = Ox%08x\n", (0xff & a)<<24 | (0xff00 & a)<<8|(0xff0000 & a)>>8 |(0xff000000 & a)>>24 )
    fmt.Printf("a = Ox%08x\n", (0xff & a)<<24 | (0xff & (a>>8))<<16 | (0xff & (a>>16))<<8 | (0xff & (a>>24)))
}


/* Result 

Hello, playground
a = Ox12345678
a = Ox78563412
a = Ox78563412


source: https://play.golang.org/p/ebUPB94crQn
*/
