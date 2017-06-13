class BootstrapData:
    def __init__(self):
        self._uuid = 0
        self._last_ip = None
        self._last_port = None
        self._can_reset = True

    def inc_uuid(self):
        self._uuid = self._uuid + 1

    def set_last_ip(self, ip):
        self._last_ip = ip

    def set_last_port(self, port):
        self._last_port = port

    def set_can_reset(self, b):
        self._can_reset = b

    def can_reset(self):
        return self._can_reset

    def get_uuid(self):
        return self._uuid

    def get_last_ip(self):
        return self._last_ip

    def get_last_port(self):
        return self._last_port
