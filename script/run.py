import paramiko
import sys
import time
import random
from reprint import output
import threading
import time
import os
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
	for node in nodes.values():
		for gather_log_path,remote_file in pairs.items():
			log_post_fix = node.ip+"_"+time.strftime("%Y-%m-%d_%H:%M:%S", time.localtime())
			res = os.system("scp %s@%s:%s %s"%(node.user, node.ip, remote_file, gather_log_path+"/"+log_post_fix))
			if res != 0:
				print("ERROR of fetch log")
				exit(1)
def sync(nodes,pairs):
	for node in nodes.values():
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
	stdin, stdout, stderr = ssh.exec_command('cd %s && %s'%(node.path,node.cmd))
	
	for line in iter(stdout.readline,""): 
		node.output.append(line.replace("\n", "")[:200])
		node.output.pop(0)
	node.status = STATUS_DONE

class Node:
	def __init__(self, user, ip, path, cmd):
		self.user = user
		self.path = path
		self.ip = ip
		self.cmd = cmd
		self.output = ["\n" for i in range(MAX_LINE)]
		self.status = STATUS_RUNNING 

if __name__ == "__main__":
	nodes = {}
	user = "cj"
	machine_ips = ["10.0.0.5", "10.0.0.6"]
	log_pair = {"../all_logs" : "/home/cj/study/libr/log"} # dest_path : remote_log
	sync_pairs = {#remote_path : cur_file
		"/home/cj/study/libr/build" : "../build/demo"
	}
	run_cmd_path = "/home/cj/study/libr/build"
	
	cmd = """
		n=0
		while(($n<6))
		do
			ls | tee ../log
			echo $n | tee -a ../log
			n=$((n+1))
			sleep 1
		done
	"""
	cmd = "./demo | tee ../log"

	#initial nodes
	index = 0
	for ip in machine_ips:
		cmd = "./demo %d %s %d | tee ../log" %(index, machine_ips[0], len(machine_ips))
		index = index + 1
		nodes[ip] = Node(user, ip, run_cmd_path, cmd)
	
	if len(sys.argv) == 2 and sys.argv[1] == "kill":
		print("Killing processes: %s"%cmd)
		for ip in machine_ips:
			kill(nodes[ip],nodes[ip].cmd)
		exit(0)

	#transfer executable file
	sync(nodes,sync_pairs)

	#run asyc
	for node in nodes.values():
		threading.Thread(target=task, args=([node])).start()

	#print result
	with output(initial_len=len(machine_ips)*(MAX_LINE+1), interval=0) as output_lines:
		while True:
			cur_line = 0
			for ip in machine_ips:
				output_lines[cur_line] = ip + " : " + nodes[ip].status
				cur_line += 1
				for j in range(MAX_LINE):
					output_lines[cur_line] = nodes[ip].output[j]
					cur_line += 1
			num_done = 0
			for ip in machine_ips:
				if nodes[ip].status != STATUS_RUNNING:
					num_done+=1
			if num_done == len(machine_ips):
				break
			time.sleep(0.01)
	
	#fetch log
	fetch_log(nodes, log_pair)