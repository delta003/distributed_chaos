class NodeLinks:
    def __init__(self):
        self.wait_flag = False

        self.level = None
        self.idx = None
        self.parent = None
        self.prev = None
        self.next = None
        self.children = []

    pass


class NodeAdresses:
    def __init__(self, ip, port, bootstrap_ip, bootstrap_port):
        self.ip = ip
        self.port = port
        self.bootstrap_ip = bootstrap_ip
        self.bootstrap_port = bootstrap_port

    pass


class NodeData:
    def __init__(self):
        self.uuid = -1

    def set_uuid(self, uuid):
        self.uuid = uuid


class NodeJobData:
    pass
