#!/usr/bin/python

import subprocess

BOOTSTRAP_PORT = 9080
START_PORT = 10000
PORTS = []

# Script execution
# ---------------------------------------------
def exec_start_node(port):
    subprocess.call("./start_node_background " + str(port), shell=True)

def exec_kill_with_port(port):
    subprocess.call("./kill_with_port " + str(port), shell=True)

def exec_start_bootstrap():
    subprocess.call("./start_bootstrap_background", shell=True)
# ---------------------------------------------

def start_bootstrap():
    exec_start_bootstrap()

def stop_bootstrap():
    exec_kill_with_port(BOOTSTRAP_PORT)

def start_node(port):
    global PORTS
    print "Starting node on port " + str(port) + "..."
    if port in PORTS:
        print "ERROR: Node already running on port " + str(port)
        return
    exec_start_node(port)
    print "Node started"

def stop_node(port):
    global PORTS
    print "Stopping node on port " + str(port) + "..."
    if port not in PORTS:
        print "ERROR: Node not running on port " + str(port)
        return
    exec_kill_with_port(port)
    print "Node stopped"

def start_nodes(n):
    global START_PORT
    global PORTS
    print "Starting " + str(n) + " nodes..."
    for i in range(0, n):
        start_node(START_PORT)
        PORTS.append(START_PORT)
        START_PORT += 1
    print "Nodes started"

def stop_all_nodes():
    global PORTS
    print "Stopping all nodes..."
    for port in PORTS:
        stop_node(port)
    print "Nodes stopped"

# Commands
# ---------------------------------------------
def cmd_help():
    print "help - Prints commands"
    print "ports - List used node ports"
    print "startboot - Start bootstrap server"
    print "startnodes N - Starts N node servers"
    print "stop P - Stops server with port"
    print "stop2 P1 P2 - Stops two servers"
    print "stopboot - Stops bootstrap server"
    print "end - Stops everything and exits"

def cmd_ports():
    global PORTS
    for port in PORTS:
        print port

def cmd_startboot():
    start_bootstrap()

def cmd_stopboot():
    stop_bootstrap()

def cmd_startnodes(cmd):
    parts = cmd.split(' ')
    if len(parts) != 2:
        print "Invalid command"
        return
    count = int(parts[1])
    start_nodes(count)

def cmd_stop(cmd):
    global PORTS
    parts = cmd.split(' ')
    if len(parts) != 2:
        print "Invalid command"
        return
    port = int(parts[1])
    stop_node(port)
    PORTS.remove(port)

def cmd_stop2(cmd):
    global PORTS
    parts = cmd.split(' ')
    if len(parts) != 3:
        print "Invalid command"
        return
    port1 = int(parts[1])
    port2 = int(parts[2])
    stop_node(port1)
    stop_node(port2)
    PORTS.remove(port1)
    PORTS.remove(port2)

def cmd_end():
    stop_bootstrap()
    stop_all_nodes()

# ---------------------------------------------

if __name__ == '__main__':
    print "Welcome to node runner!"
    print "Type help for instructions"
    while (True):
        cmd = raw_input("Command: ")
        if cmd == "help":
            cmd_help()
        if cmd == "ports":
            cmd_ports()
        elif cmd == "startboot":
            cmd_startboot()
        elif cmd.startswith("startnodes "):
            cmd_startnodes(cmd)
        elif cmd.startswith("stop "):
            cmd_stop(cmd)
        elif cmd.startswith("stop2 "):
            cmd_stop2(cmd)
        elif cmd.startswith("stopboot"):
            cmd_stopboot()
        elif cmd == "end":
            cmd_end()
            break
    print "Goodbye!"
