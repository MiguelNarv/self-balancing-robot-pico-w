# Parses .map and prints % of allocated RAM.
# -narvimg

import os

def print_RAM_Allocation(name, usedInt):
    if(usedInt > 0):
        percentage = (usedInt / ramSize) * 100
        print(f'{name} -> {percentage:.2f}%')

if __name__ == '__main__':

    print("\n--- RAM consumption ---")

    # Retrieve .map path
    file_dir = os.path.dirname(os.path.abspath(__file__))
    mapPath = os.path.join(file_dir, "../build/source/source.elf.map")

    # Define a list with blocks name in RAM memory section
    ramSize = 0x00040000
    ramUsed = 0
    blockList = [".ram_vector_table", ".data", ".igot.plt", ".bss", ".heap"]
    Parse = False

    # Open .map file
    mapFile = open(mapPath, 'r')

    # Iterate through .map lines
    for line in mapFile:
        
        # Start parsing .map from memory configuration section
        if("Memory Configuration" in line):
            Parse = True

        if(Parse):
            
            tmpList = line.split()

            if(tmpList):
                if(tmpList[0] in blockList):
                    
                    # Get block size depending on format and pass it to print func
                    if(len(tmpList) < 2):
                        nextLineBlockSize = int(next(mapFile).split()[1], 16)
                        ramUsed += nextLineBlockSize
                        print_RAM_Allocation(tmpList[0], nextLineBlockSize)
                        
                    else:
                        ramUsed += int(tmpList[2], 16)
                        print_RAM_Allocation(tmpList[0], int(tmpList[2], 16))

                    # Remove block coincidence from list      
                    blockList.remove(tmpList[0])

    # Close files
    mapFile.close()
    
    # Print total RAM used
    print_RAM_Allocation("\nTotal RAM used", ramUsed)
    print("\n")