import sys
from eeprom import Programmer

if len(sys.argv) < 2:
    print("Usage: clear.py <serial_port>")
    sys.exit()

with Programmer(sys.argv[1]) as prog:
    for i in range(2048):
        prog.write_data(i, 0)
        sys.stdout.write(".")

        if i > 0 and i % (8 * 8) == 63:
            sys.stdout.write("\n")
        elif i > 0 and i % 8 == 7:
            sys.stdout.write(" ")

        sys.stdout.flush()

    print("\nDone")
