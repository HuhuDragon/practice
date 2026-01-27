import os    

main = "frescoR.exe"
if os.path.exists(main):
        # rc,out= subprocess.getstatusoutput(main)
        #rc= subprocess.getstatusoutput(main)
        #rc = os.system('nohup ' + main + ' < fresco.in')
        os.system(main + ' < 26mg8.324.in > test26mg.out')
        # print ('*'*10)
        # print (out)