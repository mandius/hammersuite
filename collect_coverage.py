final_cov = open("final_coverage.stat", "r")
temp= final_cov.readline()
lines = final_cov.readlines()
covered_rows = []
for line in lines:
     row_num = int(line.strip())
     if row_num not in covered_rows:
         covered_rows.append(row_num)
 
final_cov.close()

current_cov = open("run_data/run_coverage.log", "r")
lines = current_cov.readlines()
for line in lines:
    row_num = int(line.strip(), base=16)
    if row_num not in covered_rows:
        covered_rows.append(row_num)

current_cov.close()

final_cov = open("final_coverage.stat", "w")
total_rows =  (4*1024*1024*1024)/(8*8192)
covered_count = len(covered_rows)
percentage_covered = (covered_count/total_rows)*100
final_cov.write("Total Number of Rows = " + str(total_rows) + " Covered Count = " + str(covered_count) + " Percentage Covered = " + str(percentage_covered) + "% \n")
for item in covered_rows:
    final_cov.write(str(item) + "\n");

final_cov.close()


