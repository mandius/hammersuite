import re
import sys
import matplotlib.pyplot as plt

filename = sys.argv[1]

fh = open(filename, "r")
rows0_to_1 = []
banks0_to_1 = []
rows1_to_0 =[] 
banks1_to_0 = []

for line in fh:
    m = re.search(r'row =\d*', line)
    print(m.group())
    row = int(m.group().split("=")[1].strip())
    m = re.search(r'bank =\d*', line)
    print(m.group())
    bank = int(m.group().split("=")[1].strip())
    m = re.search(r'Expected\ Value\ =\ .' , line)
    print(m.group())
    direction = int(m.group().split("=")[1].strip()[0], base=16)
    if(direction):
        rows1_to_0.append(row)
        banks1_to_0.append(bank)
    else:
        rows0_to_1.append(row)
        banks0_to_1.append(bank)

print("Size 1_to_0", len(rows1_to_0))
print("Size 0_to_1", len(rows0_to_1))


plt.scatter(banks0_to_1, rows0_to_1)
plt.scatter(banks1_to_0, rows1_to_0)
plt.legend(["0_to_1", "1_to_0"])
plt.show()

