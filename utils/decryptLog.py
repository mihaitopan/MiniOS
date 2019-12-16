
def decryptFile(filepath):
    contents = open(filepath, "rb").read()
    result = ""
    for bytechar in contents:
        if bytechar == 0:
            result += " "
            continue
        bytechar = chr(bytechar+0x60)
        if bytechar == '~':
            result += "\n"
        else:
            result += bytechar
    toWrite = open("LOG.txt","w+")
    toWrite.write(result)

decryptFile("oslog.txt")
