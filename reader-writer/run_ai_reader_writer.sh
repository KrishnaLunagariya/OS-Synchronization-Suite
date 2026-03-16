#!/bin/bash

echo "------ AI Based Reader Writer Automation ------"

# Step 1: Check shared file
if [ ! -f shared.txt ]
then
    echo "Creating shared.txt file..."
    echo "Initial File Data" > shared.txt
else
    echo "shared.txt already exists"
fi

# Step 2: Compile the C program
echo "Compiling C program..."
gcc ai_reader_writer.c -o ai_rw -lpthread

if [ $? -ne 0 ]
then
    echo "Compilation failed!"
    exit 1
fi

echo "Compilation successful."

# Step 3: Run the program
echo "Running AI Reader Writer program..."
./ai_rw

# Step 4: Show final file content
echo ""
echo "Final content of shared.txt:"
cat shared.txt

echo "------ Program Finished ------"
