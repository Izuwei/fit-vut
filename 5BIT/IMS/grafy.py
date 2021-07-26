import subprocess
import time
import sys

def createFile(num, data):
    f = open(f"HrabOut{num}", "w")
    f.write(data)
    f.close()

def callProcess(ortersNum):
    process = subprocess.run([f'./hrabosi -h {ortersNum} -i 520'],shell=True, check=True, stdout=subprocess.PIPE, universal_newlines=True)
    output = process.stdout
    return output

def main(count, otrsCnt):
    for i in range(1, count):
        start_time = time.time()
        try:
            data = callProcess(otrsCnt)
            createFile(i, data)
            print("Hrabosi: %d , Zabrali: %s s" % (i, time.time() - start_time))
            print("\n".join(data.split("\n")[-7:]))
        except Exception:
            print("Chyba programu")

if __name__ == "__main__":
    if len(sys.argv) <= 1:
        main(10)
    else:
        main(int(sys.argv[1]), sys.argv[2])