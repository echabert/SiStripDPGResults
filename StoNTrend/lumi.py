
sum_prod = 0
sum_rec = 0

f = open('lumi_out')
print "era\t","prod\t","rec\t","sum prod\t", "sum rec\t"
for line in f:
  res = line.split()
  prod = round(float(res[1])/1e9,2)
  rec = round(float(res[1])/1e9,2)
  sum_prod+=prod
  sum_rec+=rec
  #print res[0], str(round(float(res[1])/1e9,2)), str(round(float(res[1])/1e9,2))
  print res[0],"\t", str(prod),"\t", str(rec),"\t", sum_prod, "\t", sum_rec
