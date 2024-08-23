import numpy as np
out = open("all_runs.txt", "w")
#100 values sampled
x = np.logspace(-1,5,num=100,base = 10.0)
y = x.tolist()
c = 0
for i in y:
    #change 50. to your Ar%
    out.write("50.,"+str(c+1) + "," + str(i)+ "\n")
    c+=1
out.close

