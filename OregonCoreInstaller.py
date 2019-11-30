#!/usr/bin/python

#############
## Intro ####

import time
import os
import random
import getpass

########################################
## OregonCore Easy Linux Installer
########################################
##  ABOUT THE AUTHOR                  ##
##  ___________________________       ##
##  Name       : Jay                  ##
##  Discord    : ZERO#9272            ##
##  website    : coming soon          ##
##                                    ##
########################################



## Variables    #########
#########################

system   = os.name
username = getpass.getuser()



## Functions    #########
#########################

def corecheck(cores = 1):
	print("")
	print("")
	cores = int(input("*** cores to use? > "))
	return cores

def core():
	cores = corecheck()
	if type(cores) == str:
		print("*** WARNING VALUE WAS NOT A NUMBER")
		corecheck()
	elif cores == None or cores == "" :
		print("*** Default Cores Selected ***")
		os.system("make ~/wow/OregonCore/")
	else:
		os.system("make ~/wow/OregonCore/ -j %s" %cores)


## Preparing    #########
#########################

if os.name == "nt":
	print("Error this is script is only intended to run on Ubuntu 16")
	exit()
else:
	## Continue
	os.system("clear")
	print("*** WARNING ***")
	print("*** This script is only intended to run on Ubuntu 16 ***")
	print("*** And will only run as root ***")
	print("*** Checking if script has privileges ***")

	time.sleep(2)
	if  username != "root":
	    print ("*** WARNING PROGRAM NEEDS TO BE RUN AS ROOT ***")
	    print ("*** PROGRAM TERMINATED ***")	    
	else:
	    os.system("clear")
	    print ("*** ACCESS GRANTED ***")
	    print ("*** INSTALLING DEPENDENCIES ***")
	    time.sleep(5)

	    os.system("sudo apt-get update && apt-get upgrade && sudo apt-get install git cmake make gcc g++ clang libmysqlclient-dev libssl-dev libbz2-dev libreadline-dev libncurses-dev mysql-server libace-6.* libace-dev  build-essential cmake git binutils-dev libiberty-dev libbz2-dev openssl libssl-dev zlib1g-dev libtool mysql-client unrar libace-dev unzip libncurses-dev -y")


	    ###################################################################################################################
	    ###################################################################################################################
	    ###################################################################################################################
	    ###################################################################################################################

    	## PART ONE
    	#######################
	    time.sleep(3)
	    print ("*** PART ONE ***")
	    print ("*** DONE! ***")
	    print ("*** FETCHING FILES FROM OREGONCORE GIT ***")
	    time.sleep(6)
	    os.system("clear")



	    ###################################################################################################################
	    ###################################################################################################################
	    ###################################################################################################################
	    ###################################################################################################################
	    ###################################################################################################################
	    ###################################################################################################################




    	## PART TWO
    	#######################

	    print ("*** PART TWO ***")

	    print ("*** CREATING FOLDER wow IN HOME DIRECTORY ***")
	    os.system("mkdir ~/wow")
	    os.system("mkdir ~/wow/OregonCore")
	    
	    print ("*** CLONING OREGONCORE GIT ***")
	    time.sleep(5)
	    os.system("git clone https://github.com/talamortis/OregonCore.git ~/wow/OregonCore")
	    time.sleep(5)
	    print ("*** GITTING COMPLETE ***")
	    print ("*** DID YOU GIT IT? ok its a joke ***")
	    time.sleep(5)




	    ###################################################################################################################
	    ###################################################################################################################
	    ###################################################################################################################
	    ###################################################################################################################
	    ###################################################################################################################
	    ###################################################################################################################


    	## PART THREE
    	#######################
	    print ("*** PART THREE ***")
    	
	    print ("*** got inside OregonCore ***")
	    os.system("mkdir ~/wow/OregonCore/build")
	    print ("*** creating build ***")
	    print ("*** got inside build ***")
	    print ("*** preparing and cmake ***")
	    os.system('cmake ~/wow/OregonCore/ -DCMAKE_INSTALL_PREFIX=/home/build -DCMAKE_C_COMPILER="gcc" -DCMAKE_CXX_COMPILER="g++" -DSERVERS=1 -DTOOLS=1 -DSCRIPTS=1 -DWITH_DOCS=0 -DWITH_WARNINGS=0 -DWITH_COREDEBUG=0')
	    print ("*** BUILDING OregonCore ***")
	    time.sleep(3)
	    os.system("clear")
	    print ("*** need your attention in the next section ***")
	    time.sleep(3)

	    ###################################################################################################################
	    ###################################################################################################################
	    ###################################################################################################################
	    ###################################################################################################################
	    ###################################################################################################################
	    ###################################################################################################################






	## PART FOUR
    	#######################

	    print ("*** PART FOUR ***")
    	
	    print ("*** this section will take A lot of time ***")
	    print ("*** we can accelerate the process by using more cores ***")
	    print ("*** by default the value is 1 ***")
	    print ("#################################")
	    
	    core()

	    
	    ###################################################################################################################
	    ###################################################################################################################
	    ###################################################################################################################
	    ###################################################################################################################
	    ###################################################################################################################



		## PART FIVE
	    ###################################################################################################################


	    os.system("make install ~/wow/OregonCore/build/")
	    print ("*** PART FIVE  DATABASE ***")


