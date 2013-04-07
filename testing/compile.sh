#!/bin/bash

# This script will compile the three different versions of each implmentation,
# and copy the resulting binary into the right testing/bin folder


# OPTIMIZED
echo "Optimized: implementation 1 building..."
cd ../imp1/
make -B CFLAGS="-O3"
echo "Done"

echo "Optimized: implementation 2 building..."
cd ../imp2/
make -B CFLAGS="-O3"
echo "Done"

echo "Optimized: Copying binaries..."
cd ..
cp imp2/showcase2 testing/bin/optimized/
cp imp1/showcase1 testing/bin/optimized/
echo "Done"
echo ""


# DEBUG
echo "Debug: implementation 1 building..."
cd imp1/
make -B CFLAGS="-g"
echo "Done"

echo "Debug: implementation 2 building..."
cd ../imp2/
make -B CFLAGS="-g"
echo "Done"

echo "Debug: Copying binaries..."
cd ..
cp imp2/showcase2 testing/bin/debug/
cp imp1/showcase1 testing/bin/debug/
echo "Done"
echo ""


# DEBUG
echo "Profiling: implementation 1 building..."
cd imp1/
make -B CFLAGS="-DSELF_TEST -g -pg" LDFLAGS="-pg"
echo "Done"

echo "Profiling: implementation 2 building..."
cd ../imp2/
make -B CFLAGS="-DSELF_TEST -g -pg" LDFLAGS="-pg"
echo "Done"

echo "Profiling: Copying binaries..."
cd ..
cp imp2/showcase2 testing/bin/selfProfiling/
cp imp1/showcase1 testing/bin/selfProfiling/
echo "Done"
echo ""

