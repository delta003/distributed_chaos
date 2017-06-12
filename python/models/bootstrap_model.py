class BootstrapData:
    def __init__(self):
        self.uuid = 0
        self.last_ip = None
        self.last_port = None
        self.can_reset = True

    def inc_uuid(self):
        self.uuid = self.uuid + 1

    def set_last_ip(self, ip):
        self.last_ip = ip

    def set_last_port(self, port):
        self.last_port = port

    def set_can_reset(self, b):
        self.can_reset = b

