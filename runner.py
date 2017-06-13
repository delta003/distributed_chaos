#!/usr/bin/python

import subprocess

BOOTSTRAP_PORT = 9080
BOOTSTRAP_LANG = None
START_PORT = 10000
PORTS = []
LANG = {
    'cpp': 'cpp',
    'java': 'java',
    'py': 'python'
}

# Script execution
# ------------------------------------------------------------------------------
def exec_build(lang):
    subprocess.call("./" + lang + "/runner/build", shell=True)

def exec_kill_node(lang, port):
    subprocess.call("./" + lang + "/runner/kill_node " + str(port), shell=True)

def exec_start_bootstrap():
    subprocess.call("./" + lang + "/runner/start_bootstrap", shell=True)

def exec_start_node(lang, port):
    subprocess.call("./" + lang + "/runner/start_node " + str(port), shell=True)

def exec_kill_node(lang, port):
    subprocess.call("./" + lang + "/runner/kill_node " + str(port), shell=True)
# ------------------------------------------------------------------------------

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

def cmd_build(lang):
    pass

def cmd_startboot(lang):
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

def stop_everything():


def execute(cmd):
    global LANG
    if cmd == "help":
        cmd_help()
    elif cmd == "ports":
        cmd_ports()
    elif cmd.startswith("build "):
        parts = cmd.split(' ')
        if len(parts) != 2 or parts[1] not in LANG.keys():
            print "Invalid command"
            return
        cmd_build(LANG[parts[1]])
    elif cmd.startswith("startboot "):
        parts = cmd.split(' ')
        if len(parts) != 2 or parts[1] not in LANG.keys():
            print "Invalid command"
            return
        cmd_startboot(LANG[parts[1]])
    elif cmd.startswith("startnodes "):
        parts = cmd.split(' ')
        if len(parts) != 3 or parts[1] not in LANG.keys():
            print "Invalid command"
            return
        cmd_startnodes(LANG[parts[1]], int(parts[2]))
    elif cmd.startswith("stop "):
        parts = cmd.split(' ')
        if len(parts) != 2:
            print "Invalid command"
            return
        port = int(parts[1])
        cmd_stop(port)
    elif cmd.startswith("stop2 "):
        parts = cmd.split(' ')
        if len(parts) != 3:
            print "Invalid command"
            return
        port1 = int(parts[1])
        port2 = int(parts[2])
        cmd_stop2(port1, port2)
    elif cmd == "stopboot":
        cmd_stopboot()
    else:
        print "Unknown command :("


if __name__ == '__main__':
    print "Welcome to node runner!"
    print "Type help for instructions"
    while (True):
        cmd = raw_input("Command: ")
        if cmd == "end":
            stop_everything()
            break
        execute(cmd)
    print "Goodbye!"
