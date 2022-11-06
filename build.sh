#!/bin/bash

dst=/tmp
path=example

flags=(-I include -m64 -std=c++23 -s -Wall -O3)
executables=(fuple indexer lists reflect smp visitor)

for bin in ${executables[@]}; do
      g++ "${flags[@]}" -o ${dst}/${bin} ${path}/${bin}.cpp
done

echo Please check the executables at ${dst}
