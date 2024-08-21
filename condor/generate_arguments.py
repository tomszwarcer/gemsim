out = open("all_runs.txt", "w")
ar_percent = "0."
for i in range(0,2000,1):
    out.write(ar_percent+str(i+1) + "\n")
out.close

