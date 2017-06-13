#!/usr/bin/python

import subprocess

BOOTSTRAP_PORT = 9080
BOOTSTRAP_LANG = None
START_PORT = 4000
NODES = []
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

def exec_start_bootstrap(lang):
    subprocess.call("./" + lang + "/runner/start_bootstrap", shell=True)

def exec_start_node(lang, port):
    subprocess.call("./" + lang + "/runner/start_node " + str(port), shell=True)

def exec_stop_bootstrap(lang):
    subprocess.call("./" + lang + "/runner/stop_bootstrap", shell=True)
# ------------------------------------------------------------------------------

# Commands
# ------------------------------------------------------------------------------
def cmd_help():
    print "help - Prints commands."
    print "nodes - Lists used nodes with implementation and port."
    print "startboot $LANG - Starts $LANG implementation of bootstrap server."
    print "startnodes $LANG $N - Starts $N node servers with $LANG implementation."
    print "stop $PORT - Stops server with port $PORT."
    print "stop2 $PORT1 $PORT2 - Stops two servers with given ports."
    print "stopboot - Stops bootstrap server."
    print "end - Stops everything and exits."

def cmd_nodes():
    global NODES
    for node in NODES:
        print node[0] + " : " + str(node[1])

def cmd_build(lang):
    print "Building for " + lang + "..."
    exec_build(lang)
    print "Building for " + lang + " completed."

def cmd_startboot(lang):
    print "Staring bootstrap for " + lang + "..."
    global BOOTSTRAP_LANG
    if BOOTSTRAP_LANG is not None:
        print "ERROR: Bootstrap already running."
        return
    BOOTSTRAP_LANG = lang
    exec_start_bootstrap(lang)
    print "Staring bootstrap for " + lang + " completed."

def cmd_stopboot():
    print "Stopping bootstrap for " + lang + "..."
    global BOOTSTRAP_LANG
    if BOOTSTRAP_LANG is None:
        print "ERROR: Bootstrap not running."
        return
    exec_stop_bootstrap(BOOTSTRAP_LANG)
    print "Stopping bootstrap for " + lang + " completed."

def cmd_startnodes(lang, count):
    print "Starting " + str(count) + " nodes for " + lang + "..."
    global START_PORT
    global NODES
    for i in range(0, count):
        node = (lang, START_PORT)
        exec_start_node(lang, START_PORT)
        START_PORT += 1
        NODES.append(node)
        print "New node on port " + str(node[1])
    print "Starting " + str(count) + " nodes for " + lang + " completed."

def cmd_stop(port):
    print "Stopping node on port " + str(port) + "..."
    global NODES
    index = -1
    for i, node in enumerate(NODES):
        if node[1] == port:
            index = i
            exec_kill_node(node[0], node[1])
            break
    if index == -1:
        print "ERROR: Node not found."
    else:
        NODES.pop(index)
    print "Stopping node on port " + str(port) + " completed."

def cmd_stop2(port1, port2):
    print "Stopping two nodes on ports " + str(port1) + " and " + str(port2) + "..."
    cmd_stop(port1)
    cmd_stop(port2)
    print "Stopping two nodes on ports " + str(port1) + " and " + str(port2) + " completed."

# ------------------------------------------------------------------------------

def stop_everything():
    print "Stopping everything..."
    global BOOTSTRAP_PORT
    global BOOTSTRAP_LANG
    global NODES
    if BOOTSTRAP_LANG is not None:
        print "Stopping bootstrap"
        exec_stop_bootstrap(BOOTSTRAP_LANG)
    for node in NODES:
        print "Stopping " + node[0] + " node on port " + str(node[1])
        exec_kill_node(node[0], node[1])
    print "Stopping completed."


def execute(cmd):
    global LANG
    if cmd == "help":
        cmd_help()
    elif cmd == "nodes":
        cmd_nodes()
    elif cmd.startswith("build "):
        parts = cmd.split(' ')
        if len(parts) != 2 or parts[1] not in LANG.keys():
            print "Invalid usage"
            return
        cmd_build(LANG[parts[1]])
    elif cmd.startswith("startboot "):
        parts = cmd.split(' ')
        if len(parts) != 2 or parts[1] not in LANG.keys():
            print "Invalid usage"
            return
        cmd_startboot(LANG[parts[1]])
    elif cmd.startswith("startnodes "):
        parts = cmd.split(' ')
        if len(parts) != 3 or parts[1] not in LANG.keys():
            print "Invalid usage"
            return
        cmd_startnodes(LANG[parts[1]], int(parts[2]))
    elif cmd.startswith("stop "):
        parts = cmd.split(' ')
        if len(parts) != 2:
            print "Invalid usage"
            return
        port = int(parts[1])
        cmd_stop(port)
    elif cmd.startswith("stop2 "):
        parts = cmd.split(' ')
        if len(parts) != 3:
            print "Invalid usage"
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
    print ""
    while (True):
        print "----------------------------------------------------------------"
        cmd_help()
        print "----------------------------------------------------------------"
        cmd = raw_input("Command: ")
        if cmd == "end":
            stop_everything()
            break
        execute(cmd)
    print "Goodbye!"
