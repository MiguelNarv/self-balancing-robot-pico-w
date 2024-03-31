# Formats .txt to .SVDat based on input arguments.
# -narvimg

import sys
from pathlib import Path
if __name__ == '__main__':

    # Arguments 
    inputFilePath = sys.argv[1]
    startAddressInLine = sys.argv[2]
    size = sys.argv[3]

    # Open the unformated txt file to read
    dumpFile = open(f"{inputFilePath}", 'r')

    # Open the formated txt file to overwrite contents in bin mode
    svdatFile = open(f"{(sys.argv[1].split('.')[0])}.SVDat", 'wb')

    firstTime = True
    fline = []

    # Go through .txt lines
    for line in dumpFile:
        if(firstTime):
             # Remove address, remove new lines
            tmpList = line[16:].split('\n')[0].split(' ')[(1 + int(startAddressInLine)):]
            byteList = bytes.fromhex(''.join(tmpList))
            fline.extend(tmpList)
            firstTime = False
        else:
            # Remove address, remove new lines
            tmpList = line[16:].split('\n')[0].split(' ')[1:]
            byteList = bytes.fromhex(''.join(tmpList))
            fline.extend(tmpList)
    
    # Get elements up to the specified lenght
    fline = fline[:int(size)]
    # Join the byte list
    flineBytes = bytearray(int(x, 16) for x in fline)

    # Open svdat, write formated lines
    svdatFile.write(flineBytes)

    # Close files
    dumpFile.close()
    svdatFile.close()

        