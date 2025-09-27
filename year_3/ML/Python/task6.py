def check(x: str, file: str):
    f = open(file, "w")
    s = x.lower().split()
    d = {}
    for i in s:
        if i in d:
            d[i] += 1
        else:
            d[i] = 1
    d_sort = sorted(d.items())
    for i,j in d_sort:
        s =  str(i)+" "+str(j)+"\n"
        f.write(s)

            




