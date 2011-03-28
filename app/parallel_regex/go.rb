#!/usr/bin/env ruby -wKU

puts "1. make filler"
`g++ -o filler ./filler.cpp`

puts "2. make data"
`dd if=/dev/zero of=./test_16g.bin bs=1024*1024 count=16*1024`
`./filler ./test_16g.bin`