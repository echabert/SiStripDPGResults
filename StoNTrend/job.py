import subprocess
import sys

f = open("era_input", 'r')
for line in f:
 res = line.split()
 command = ["brilcalc","lumi","--begin",res[1],"--end",res[2]]
 #print command
 p = subprocess.Popen(command, stdout=subprocess.PIPE)
 #print p.communicate()
 (stdout,stderr)  = p.communicate()
 #print stdout
 output =  stdout.split('\n')[-4].replace(" ", "").split('|')
 print res[0],output[5], output[6]
 #sys.exit(-1)
