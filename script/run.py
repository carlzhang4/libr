#!/usr/bin/python3.7
import paramiko
import sys
import time
import random
from reprint import output
import threading
import time
import os
import json5
#############################defines
MAX_LINE = 5
STATUS_RUNNING = "\033[4;33m Running:\t \033[0m"
STATUS_DONE = "\033[37;42m Done:\t \033[0m"
STATUS_CRASH = "\033[1;31;40m Error:\t \033[0m"
#############################
def kill(node, cmd):
	ssh = paramiko.SSHClient()
	key = paramiko.AutoAddPolicy()
	ssh.set_missing_host_key_policy(key)
	ssh.connect(node.ip, 22, node.user, '' ,timeout=5)
	stdin, stdout, stderr = ssh.exec_command("ps -ef |grep '%s' |grep -v grep | awk '{print $2}' | xargs kill -9"%cmd)
	for line in iter(stdout.readline,""): 
		print(line)
def fetch_log(nodes,pairs):
	for node in nodes:
		for gather_log_path,remote_file in pairs.items():
			log_post_fix = node.ip+"_"+time.strftime("%Y-%m-%d_%H:%M:%S", time.localtime())
			res = os.system("scp %s@%s:%s %s"%(node.user, node.ip, remote_file, gather_log_path+"/"+log_post_fix))
			if res != 0:
				print("ERROR of fetch log")
				exit(1)
def sync(nodes,pairs):
	for node in nodes:
		for remote_path,f in pairs.items():
			res = os.system("scp %s %s@%s:%s"%(f, node.user, node.ip, remote_path))
			if res != 0:
				print("ERROR of sync")
				exit(1)
def task(node):
	ssh = paramiko.SSHClient()
	key = paramiko.AutoAddPolicy()
	ssh.set_missing_host_key_policy(key)
	ssh.connect(node.ip, 22, node.user, '' ,timeout=5)
	stdin, stdout, stderr = ssh.exec_command('cd %s && rm log && %s'%(node.path,node.cmd))
	
	node.output.append("Run task: node.ip:%s node.cmd:%s"%(node.ip,node.cmd))
	node.output.pop(0)
	
	for line in iter(stdout.readline,""): 
		node.output.append(line.replace("\n", "")[:200])
		node.output.pop(0)
	node.status = STATUS_DONE
	time.sleep(1)
	ssh.close()

class Node:
	def __init__(self, user, ip, path, cmd):
		self.user = user
		self.path = path
		self.ip = ip
		self.cmd = cmd
		self.output = ["\n" for i in range(MAX_LINE)]
		self.status = STATUS_RUNNING 

if __name__ == "__main__":
	nodes = []
	threads = []

	f = open('config.json5')
	data = json5.load(f)
	user 			= data["user"]
	machine_ips 	= data["machine_ips"]
	log_pair 		= data["log_pair"]
	sync_pairs 		= data["sync_pairs"]
	run_cmd_path 	= data["run_cmd_path"]
	base_cmd 		= data["base_cmd"]
	extra_param		= data["extra_param"]
	
	base_cmd += " -serverIp %s "%machine_ips[0]  #server ip
	#initial nodes
	index = 0
	for ip in machine_ips:
		cmd = base_cmd + " -numNodes %d -nodeId %d "%(len(machine_ips), index)
		cmd += extra_param
		cmd += " |tee log"
		print(cmd)
		index += 1
		nodes.append(Node(user, ip, run_cmd_path, cmd))
	
	if len(sys.argv) == 2 and sys.argv[1] == "kill":
		print("Killing processes: %s"%cmd)
		for node in nodes:
			kill(node,node.cmd)
		exit(0)
	elif len(sys.argv) == 2 and sys.argv[1] == "sync":
		sync(nodes,sync_pairs)
		exit(0)
	elif len(sys.argv) == 2:
		print("Unknow command")
		exit(0)
	#transfer executable file
	sync(nodes,sync_pairs)

	#run asyc
	for node in nodes:
		thread = threading.Thread(target=task, args=([node]))
		thread.start()
		threads.append(thread)
	#print result
	with output(initial_len=len(machine_ips)*(MAX_LINE+1), interval=0) as output_lines:
		while True:
			cur_line = 0
			for node in nodes:
				output_lines[cur_line] = node.ip + " : " + node.status
				cur_line += 1
				for j in range(MAX_LINE):
					output_lines[cur_line] = node.output[j]
					cur_line += 1
			num_done = 0
			for node in nodes:
				if node.status != STATUS_RUNNING:
					num_done+=1
			if num_done == len(machine_ips):
				break
			time.sleep(0.01)
	
	#fetch log
	fetch_log(nodes, log_pair)