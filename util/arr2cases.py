# Convert scanval array to switch cases map

codes = [
    0x0b, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a
]
codes = [str(hex(x)) for x in codes]
mapped = [chr(x) for x in range(ord('a'), ord('z') + 1)]
cases = ["case {}: return '{}'; ".format(x, y) for x, y in zip(codes, mapped)]
 
i = 1
for c in cases:
    print(c, end='')
    if i % 3 == 0: print('')
    i += 1




