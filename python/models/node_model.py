class Edge:
    def __init__(self):
        self._uuid = None
        self._type = None
        self._ip = None
        self._port = None

    def __init__(self, uuid, type, ip, port):
        self._uuid = uuid
        self._type = type
        self._ip = ip
        self._port = port

    def get(self):
        return {'uuid': self._uuid, 'kind': self._type, 'ip': self._ip, 'port': self._port}


class NodeLinks:
    def __init__(self):
        self._wait_flag = False

        self._level = None
        self._idx = None
        self._parent = None
        self._prev = None
        self._next = None
        self._children = []

    def should_wait(self):
        return self._wait_flag

    def set_wait(self, b):
        self._wait_flag = b

    def get_parent(self):
        return self._parent

    def set_parent(self, uuid, ip, port):
        self._parent = Edge(uuid=uuid, ip=ip, port=port, type='parent')

    def get_next(self):
        return self._next

    def set_next(self, uuid, ip, port):
        self._parent = Edge(uuid=uuid, ip=ip, port=port, type='next')

    def get_prev(self):
        return self._prev

    def set_prev(self, uuid, ip, port):
        self._parent = Edge(uuid=uuid, ip=ip, port=port, type='prev')

    def get_children(self):
        return self._children

    def add_child(self, uuid, ip, port):
        child = Edge(uuid=uuid, ip=ip, port=port, type='child')
        self._children.append(child)

    def remove_child_at(self, position):
        del self._children[position]

    def get_edge_as_dict(self, type):
        if type == 'parent':
            if self._parent is None:
                return {}
            return {'edge': self._parent.get()}
        elif type == 'next':
            if self._next is None:
                return {}
            return {'edge': self._next.get()}
        elif type == 'prev':
            if self._prev is None:
                return {}
            return {'edge': self._prev.get()}
        raise Exception('unknown edge type')

    def reset(self):
        self._level = None
        self._idx = None
        self._parent = None
        self._prev = None
        self._next = None
        self._children = []

    def set_edge(self, edge):
        e = Edge(uuid=edge['uuid'], ip=edge['ip'], port=edge['port'], type=edge['type'])
        if edge['type'] == 'parent':
            self._parent = e
        elif edge['type'] == 'prev':
            self._prev = e
        elif edge['type'] == 'next':
            self._next = e
        elif edge['type'] == 'child':
            self._children.append(e)
        raise Exception('unknown edge type')


class NodeAddresses:
    def __init__(self, ip, port, bootstrap_ip, bootstrap_port):
        self._ip = ip
        self._port = port
        self._bootstrap_ip = bootstrap_ip
        self._bootstrap_port = bootstrap_port
        self._manager_ip = None
        self._manager_port = None

    def set_manager_ip(self, ip):
        self._manager_ip = ip

    def set_manager_port(self, port):
        self._manager_port = port

    def get_ip(self):
        return self._ip

    def get_port(self):
        return self._port


class NodeData:
    def __init__(self):
        self._uuid = -1

    def set_uuid(self, uuid):
        self._uuid = uuid

    def get_uuid(self):
        return self._uuid


class Job:
    def __init__(self):
        self._width = None
        self._height = None
        self._p = None
        self._points = None
        self._job_id = None

    def __le__(self, other):
        return self._job_id < other.get_job_id()

    def __eq__(self, other):
        return self._job_id == other.get_job_id()

    def get_job_id(self):
        return self._job_id

    def get_points(self):
        return self._points

    def get_as_dict(self):
        return {'uuid': str(self._job_id), 'points': self._points}

    def add_point(self, x, y):
        self._points.append({'x': x, 'y': y})

    def get_last_point(self):
        return self._points[-1]


class NodeJobData:
    def __init__(self):
        self._jobs = []
        self._my_job = Job()
        self._prev_backup = Job()
        self._next_backup = Job()

    def add_job(self, job):
        if job in self._jobs:
            return
        self._jobs.append(job)
        self._jobs.sort()

    def get_jobs(self):
        return self._jobs

    def add_backup(self, uuid, job_id, point):
        self._prev_backup.add_point(point['x'], point['y'])
        self._next_backup.add_point(point['x'], point['y'])

    def remove_job(self, job_id):
        self._my_job = Job()

    def list_ids(self):
        return [x['jobid'] for x in self._jobs]

    def get_my_points(self):
        return self._my_job.get_points()

    def get_prev_backup(self):
        return self._prev_backup.get_as_dict()

    def get_next_backup(self):
        return self._next_backup.get_as_dict()

    def add_point(self, x, y):
        self._my_job.add_point(x, y)

    def get_last_point(self):
        return self._my_job.get_last_point()
