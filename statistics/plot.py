import matplotlib.pyplot as plt 

data_in = open("../in.txt", "r")
data_out = open("../out.txt", "r")

input_arr = data_in.readlines()
input_arr = [int(i) for i in input_arr]
output_arr = data_out.readlines()	
output_arr = [int(i) for i in output_arr]

data = []
for s1,s2 in zip(input_arr, output_arr):
	data.append(s2 - s1)

data_in.close()
data_out.close()

print data
size = len(data)
mean = sum(data)/size
sd = (sum(x*x for x in data) / size - (sum(data) / size) ** 2) ** 0.5

plt.hist(data,bins=size, histtype='bar')
plt.show()