def get_bit(val, bit):
    return ((val & 1<<bit)>>bit)

def calc_mappings(addr):
    bank = (get_bit(addr, 13)^get_bit(addr,16)) | ((get_bit(addr, 14)^get_bit(addr,17))<<1) | ((get_bit(addr, 15)^get_bit(addr,18))<<2) 
    row = (addr&0xffff0000)>>16

    return [bank, row]



fp = open("addr.log", "r")

lines = fp.readlines()

entries_verified =0
for i in range(1,len(lines)):
    if not (lines[i][0]== "="):
        entries = lines[i].strip().split("|")
        v_addr = entries[2].strip()
        p_addr = entries[3].strip()
        bank = int(entries[0].strip(), base=16)
        row = int(entries[5].strip(), base=16)

        [bank_calc, row_calc] = calc_mappings(int(p_addr, base=16))
        

        #print(v_addr, p_addr, row_calc, bank_calc)
            
        if not (bank == bank_calc):
            print("Bank calculation for address not correct", p_addr, bank, bank_calc)
            
        if not (row == row_calc):
            print("Row calculation for address not correct", p_addr, row, row_calc)
        entries_verified = entries_verified +1;
print("entries verified " , entries_verified)
        

