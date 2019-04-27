

/* 1st */
package main

import (
    "encoding/binary"
    "fmt"
)

func uintsToBytes(vs []uint32) []byte {
    buf := make([]byte, len(vs)*4)
    for i, v := range vs {
        binary.LittleEndian.PutUint32(buf[i*4:], v)
    }
    return buf
}

func bytesToUints(vs []byte) []uint32 {
    out := make([]uint32, len(vs)/4)
    for i := range out {
        out[i] = binary.LittleEndian.Uint32(vs[i*4:])
    }
    return out
}
func main() {
    src := []uint32{1, 2, 3}
    b := uintsToBytes(src)
    fmt.Printf("encoded: % v\n", b)
    fmt.Printf("decoded: % v\n", bytesToUints(b))
}


/* 1st Result

encoded: [ 1  0  0  0  2  0  0  0  3  0  0  0]
decoded: [ 1  2  3]

*/





/* 2nd */

package main

import (
    "bytes"
    "encoding/binary"
    "fmt"
)

func main() {
    buf := new(bytes.Buffer)
    source := []uint32{1, 2, 3}
    err := binary.Write(buf, binary.LittleEndian, source)
    if err != nil {
        fmt.Println("binary.Write failed:", err)
    }
    fmt.Printf("Encoded: % x\n", buf.Bytes())

    check := make([]uint32, 3)
    rbuf := bytes.NewReader(buf.Bytes())
    err = binary.Read(rbuf, binary.LittleEndian, &check)
    if err != nil {
        fmt.Println("binary.Read failed:", err)
    }
    fmt.Printf("Decoded: %v\n", check)

}


/* 2nd Result 

Encoded: 01 00 00 00 02 00 00 00 03 00 00 00
Decoded: [1 2 3]


source : https://play.golang.org/p/7oAS0adn1DP

*/
