#!/bin/bash

ncores=20

seedList="129 137 279 672 195 666 429 762 821 483 999 729 318 349 287 241 476 555 581 573"

#parallel -j ${ncores} bash test.sh {} ::: ${seedList}
parallel -j ${ncores} bash test_py.sh {} ::: ${seedList}


