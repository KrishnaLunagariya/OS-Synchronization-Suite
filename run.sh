#!/bin/bash

echo "====================================="
echo " AI DINING PHILOSOPHER SIMULATOR"
echo "====================================="

echo "Compiling program..."

gcc ai_dining.c -o ai_dining -pthread

if [ $? -ne 0 ]; then
    echo "Compilation Failed!"
    exit 1
fi

echo "Compilation Successful!"
echo "Running simulation..."
echo ""

./ai_dining

echo ""
echo "Simulation Finished"
