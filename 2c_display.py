import sys
from eeprom import Programmer

# Hex values to display digits 0-9 on a 7-segment display
DIGITS = [0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B]

if len(sys.argv) < 2:
    print("Usage: 2c_display.py <serial_port>")
    sys.exit()

with Programmer(sys.argv[1]) as prog:
    for val in range(256):
        # Regular numbers
        prog.write_data(val,        DIGITS[val % 10])         # 1s
        prog.write_data(val + 256,  DIGITS[(val / 10) % 10])  # 10s
        prog.write_data(val + 512,  DIGITS[(val / 100) % 10]) # 100s
        prog.write_data(val + 768,  0)

        # 2s complement
        if val > 127:
            val2c = abs(val - 256)
            sign = 0x01
        else:
            val2c = val
            sign = 0

        prog.write_data(val + 1024, DIGITS[val2c % 10])         # 1s
        prog.write_data(val + 1280, DIGITS[(val2c / 10) % 10])  # 10s
        prog.write_data(val + 1536, DIGITS[(val2c / 100) % 10]) # 100s
        prog.write_data(val + 1792, sign)

        sys.stdout.write(".")

        if val > 0 and val % (8 * 8) == 63:
            sys.stdout.write("\n")
        elif val > 0 and val % 8 == 7:
            sys.stdout.write(" ")

        sys.stdout.flush()

    print('\nDone')
