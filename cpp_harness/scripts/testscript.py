#!/usr/bin/python


# Copyright 2015 University of Rochester
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License. 



from os.path import dirname, realpath, sep, pardir
import sys
import os

# path loading ----------
#print dirname(realpath(__file__))
os.environ['PATH'] = dirname(realpath(__file__))+\
":" + os.environ['PATH'] # scripts
os.environ['PATH'] = dirname(realpath(__file__))+\
"/..:" + os.environ['PATH'] # bin
os.environ['PATH'] = dirname(realpath(__file__))+\
"/../../cpp_harness:" + os.environ['PATH'] # metacmd

# execution ----------------
for i in range(0,5):
	cmd = "metacmd.py dqs -i 10 -m 2 -v -a ../cpp_harness/affinity/even_odds.aff "+\
	" --meta t:1...4:6:8:16:20:24:32:36:37:40:44:48:54:60:64:71"+\
	" --meta r:0...19 -o data/3-15-15.csv"
	#os.system(cmd)
	cmd = "metacmd.py dqs -i 10 -m 0 -v  "+\
	" --meta t:1...4:6:8:16:20:24:32:36:37:40:44:48:54:60:64:71"+\
	" -o data/3-15-15-fai.csv"
	#os.system(cmd)
        cmd = "metacmd.py dqs -i 10 -m 2 -v  "+\
        " --meta t:1...4:6:8:16:20:24:32:36:37:40:44:48:54:60:64:71"+\
        " --meta r:14 -o data/3-15-15.csv"
        os.system(cmd)

