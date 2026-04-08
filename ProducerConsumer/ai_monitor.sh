#!/bin/bash

echo "========================================"
echo " PRODUCER CONSUMER AI MONITOR SYSTEM"
echo "========================================"

# compile program
echo "Compiling program..."

gcc producer_consumer.c -o producer_consumer -lpthread

if [ $? -ne 0 ]
then
    echo "Compilation failed"
    exit
fi

echo "Compilation successful"

echo ""
echo "Running program..."
echo ""

./producer_consumer

echo ""
echo "========================================"
echo "AI LOG ANALYSIS"
echo "========================================"

if [ ! -f log.txt ]
then
    echo "Log file not found!"
    exit
fi

PROD=$(grep -c "produced" log.txt)
CONS=$(grep -c "consumed" log.txt)

echo "Total Produced = $PROD"
echo "Total Consumed = $CONS"

echo ""

if [ $PROD -gt $CONS ]
then
    echo "AI Prediction:"
    echo "Producers are faster than Consumers"
    echo "Buffer may become FULL"
elif [ $CONS -gt $PROD ]
then
    echo "AI Prediction:"
    echo "Consumers are faster"
    echo "Buffer may become EMPTY"
else
    echo "AI Prediction:"
    echo "System balanced"
fi

echo ""
echo "Producer Operations:"
grep "produced" log.txt

echo ""
echo "Consumer Operations:"
grep "consumed" log.txt

echo ""
echo "========================================"
echo "AI ANALYSIS COMPLETED"
echo "========================================"

