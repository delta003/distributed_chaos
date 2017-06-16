import queue
import threading
import time
import communication.request_creator as rc
import random


class NetworkWaitException(Exception):
    pass


def __init__(link_data, address_data, data, job_data, app_logger):
    global links, addresses, node_info, job_info, logger
    links = link_data
    addresses = address_data
    node_info = data
    job_info = job_data
    logger = app_logger


# ping next node every 1 sec
def ping():
    threading.Timer(1, ping).start()

    if links.should_wait():
        return

    failures = 0
    next_edge = links.get_next()
    next_ip = next_edge['ip']
    next_port = next_edge['port']

    prev_edge = links.get_prev()

    try:
        rc.basic_ok(edge=next_edge)
    except Exception as e:
        failures = failures + 1
        if failures == 5:
            try:
                alive = rc.basic_check(edge=prev_edge, ip=next_ip, port=next_port)
                if not alive:
                    reconfigure()
                else:
                    failures = 0
            except Exception as e:
                reconfigure()


def reconfigure():
    can_reset = rc.bootstrap_reset()
    if not can_reset:
        return

    graph_traversal = bfs(node_info.get_uuid(), addresses.get_ip(), addresses.get_port())
    for e in graph_traversal:
        if e['uuid'] == node_info.get_uuid():
            continue
        rc.reset_node(edge=e)
    links.reset()
    time.sleep(5)
    rc.bootstrap_reset_done()


def bfs(uuid, ip, port):
    q = queue.Queue()
    q.put({'uuid': uuid, 'ip': ip, 'port': port})
    visited = {int(uuid): True}
    nodes = []
    edges = []
    while not q.empty():
        curr = q.get()
        nodes.append(curr)
        edge_list = rc.all_edges(edge=curr)
        for e in edge_list:
            edges.append({'start_uuid': curr['uuid'], 'end_uuid': e['uuid'], 'type': e['type']})
            next_uuid = int(e['uuid'])
            if next_uuid in visited:
                continue
            visited[next_uuid] = True
            q.put({'uuid': next_uuid, 'ip': e['ip'], 'port': e['port']})
    return nodes, edges


# joining node into network
def join():
    links.set_wait(True)

    uuid, mng_ip, mng_port = rc.bootstrap_hello(ip=addresses.get_ip(), port=addresses.get_port())
    mng_edge = {'ip': mng_ip, 'port': mng_port}

    if node_info.get_uuid() is None:
        node_info.set_uuid(uuid)

    if mng_ip is None:
        if node_info.get_uuid() is None:
            node_info.set_uuid(uuid)
        links.set_next(uuid=uuid, ip=addresses.get_ip(), port=addresses.get_port())
        links.set_prev(uuid=uuid, ip=addresses.get_ip(), port=addresses.get_port())

        links.set_wait(False)
        return
    mng_edge['uuid'] = rc.basic_info(mng_edge)[0]
    this = {'uuid': str(uuid), 'ip': addresses.get_ip(), 'port': addresses.get_port()}

    parent_edge = rc.get_edge(edge=mng_edge, type='parent')
    if not parent_edge:
        mng_prev = rc.get_edge(edge=mng_edge, type='prev')
        mng_next = rc.get_edge(edge=mng_edge, type='next')

        if mng_prev['uuid'] == mng_next['uuid']:
            # Node joins top level circle
            x_nxt = rc.set_edge(edge=mng_edge, e=this, type='next')
            rc.set_edge(edge=x_nxt, e=this, type='prev')
            mng_edge['type'] = 'prev'
            links.set_edge(mng_edge)
            links.set_next(uuid=x_nxt['uuid'], ip=x_nxt['ip'], port=x_nxt['port'])

        else:
            rc.adopt_child(mng_edge, this)
            mng_edge['type'] = 'parent'
            links.set_edge(mng_edge)
            this['type'] = 'next'
            links.set_edge(this)
            this['type'] = 'prev'
            links.set_edge(this)

        links.set_wait(False)
        return

    redirected, level_created, children, next = rc.adopt_child(parent_edge, this, can_redirect=True)
    if redirected:
        rc.adopt_child(edge=next, e=this, can_redirect=False)
        parent_edge = next

    parent_edge['type'] = 'parent'
    links.set_edge(parent_edge)
    if not level_created:
        x_nxt = rc.set_edge(edge=mng_edge, e=this, type='next')
        rc.set_edge(edge=x_nxt, e=this, type='prev')
        mng_edge['type'] = 'prev'
        links.set_edge(mng_edge)
        x_nxt['type'] = 'next'
        links.set_edge(x_nxt)

        links.set_wait(False)
        return
    print(children)
    # New level should be created, adopt returned edges with 5 children
    this_prev0 = children[0]
    this_prev1 = children[1]
    this_prev2 = children[2]
    this_prev3 = children[3]

    # Exclude thisPrev2 and thisPrev3 from layer
    new_next = rc.get_edge(edge=mng_edge, type='next')
    new_prev = rc.get_edge(edge=this_prev2, type='prev')

    # Previous level linking
    rc.set_edge(edge=new_next, e=new_prev, type='prev')
    rc.set_edge(edge=new_prev, e=new_next, type='next')

    # Change parents
    rc.adopt_child(edge=this_prev0, e=this_prev2)
    rc.set_edge(edge=this_prev2, e=this_prev0, type='parent')

    rc.adopt_child(edge=this_prev0, e=this_prev3)
    rc.set_edge(edge=this_prev3, e=this_prev0, type='parent')

    rc.adopt_child(edge=this_prev1, e=this)
    links.set_parent(uuid=this_prev1['uuid'], ip=this_prev1['ip'], port=this_prev1['port'])

    # Make circle in new layer
    rc.set_edge(edge=this_prev3, e=this, type='next')
    rc.set_edge(edge=this_prev2, e=this, type='prev')
    links.set_next(uuid=this_prev2['uuid'], ip=this_prev2['ip'], port=this_prev2['port'])
    links.set_prev(uuid=this_prev3['uuid'], ip=this_prev3['ip'], port=this_prev3['port'])

    links.set_wait(False)


def basic_ok_controller():
    return {}


def basic_info_controller():
    if links.should_wait():
        raise NetworkWaitException()
    return {'uuid': str(node_info.get_uuid()), 'ip': addresses.get_ip(), 'port': addresses.get_port()}


def basic_check_controller(ip, port):
    try:
        edge = {'ip': ip, 'port': port}
        rc.basic_ok(edge)
    except Exception as e:
        return {'alive': 'false'}  # TODO: zavisi koji je Exception
    return {'alive': 'true'}


def network_edges_controller():
    if links.should_wait():
        raise NetworkWaitException()
    ret = []
    if links.get_parent():
        ret.append(links.get_parent())

    if links.get_prev():
        ret.append(links.get_prev())

    if links.get_next():
        ret.append(links.get_next())

    ret = ret + links.get_children()
    return {"edges": ret}


def network_get_edge_controller(type):
    if links.should_wait():
        raise NetworkWaitException()
    return {'edge': links.get_edge_as_dict(type)}


def network_set_edge_controller(edge):
    if links.should_wait():
        raise NetworkWaitException()
    ret = links.get_edge_as_dict(edge['type'])
    links.set_edge(edge)
    return {'oldedge': ret}


# TODO: refactor responses so that they rely on requests.response_creator.py
def network_adopt_controller(edge, can_redirect):
    if links.should_wait():
        raise NetworkWaitException()
    if can_redirect == 'true':
        can_redirect = True
    else:
        can_redirect = False
    children_count = len(links.get_children())
    print(children_count)
    if children_count == 0 or children_count == 1 or children_count == 3:
        links.add_child(uuid=edge['uuid'], ip=edge['ip'], port=edge['port'])
        return {'redirect': 'false', 'create_level': 'false'}
    elif children_count == 2:
        if can_redirect:
            ret = {'next': links.get_edge_as_dict('next'), 'redirect': 'true'}
            return ret
        else:
            links.add_child(uuid=edge['uuid'], ip=edge['ip'], port=edge['port'])
            return {'redirect': 'false', 'create_level': 'false'}
    elif children_count == 4:
        links.add_child(uuid=edge['uuid'], ip=edge['ip'], port=edge['port'])
        ret = {'edges': links.get_children(), 'redirect': 'false', 'create_level': 'true'}
        for i in range(3):
            links.remove_child_at(2)
        return ret
    raise Exception('Undefined state')


def network_reset_controller():
    if links.should_wait():
        raise NetworkWaitException()
    ret = network_edges_controller()
    links.reset()
    join_thrd = threading.Thread(target=join)
    join_thrd.start()
    return ret


def network_visualize_controller():
    if links.should_wait():
        raise NetworkWaitException()
    nodes, edges = bfs(uuid=node_info.get_uuid(), ip=addresses.get_ip(), port=addresses.get_port())
    return {'nodes': nodes, 'edges': edges}


# Job controllers
def job_handler():
    threading.Timer(1, ping).start()

    last_point = job_info.get_last_point()
    last_x = float(last_point['x'])
    last_y = float(last_point['y'])

    next_point = random.choice(job_info.get_starting_points())
    next_x = float(next_point['x'])
    next_y = float(next_point['y'])
    p = float(job_info.get_p())
    x = last_x + (next_x - last_x) * p
    y = last_y + (next_y - last_y) * p

    new_point = {'x': str(x), 'y': str(y)}
    job_info.add_point(new_point)

    # TODO: add a queue for sending jobs
    rc.backup_jobs(edge=links.get_edge('next'), uuid=node_info.get_uuid(),
                   job_id=job_info.get_my_job_id(), point=new_point)
    rc.backup_jobs(edge=links.get_edge('prev'), uuid=node_info.get_uuid(),
                   job_id=job_info.get_my_job_id(), point=new_point)


def jobs_add_controller(job_id, width, height, p, points):
    job_info.add_job(job_id, width, height, p, points)
    return network_edges_controller()


def jobs_new_controller(width, height, p, points):
    job_id = node_info.get_uuid()
    job_info.add_job(job_id, width, height, p, points)
    nodes, _ = bfs(uuid=node_info.get_uuid(), ip=addresses.get_ip(), port=addresses.get_port())
    for node in nodes:
        rc.add_job(edge=node, job_id=job_id, width=width, height=height, p=p, points=points)
    return {'job_id', str(job_id)}


def jobs_all_controller():
    ret = []
    return job_info.get_jobs()


def jobs_backup_controller(uuid, job_id, point):
    job_info.add_backup(uuid=uuid, job_id=job_id, point=point)

    last_point = job_info.get_last_point()
    starting_points = job_info.get_starting_points()


def jobs_remove_controller(job_id):
    job_info.remove_job(job_id)
    return network_edges_controller()


def jobs_kill_controller(job_id):
    graph_traversal = bfs(node_info.get_uuid(), addresses.get_ip(), addresses.get_port())
    for node in graph_traversal:
        rc.remove_job(node, job_id)
    return {}


def jobs_ids_controller():
    return {'jobids': job_info.list_ids()}


def jobs_data_controller(job_id):
    uuid = node_info.get_uuid()
    points = job_info.get_points()
    backup = [job_info.get_prev_backup(), job_info.get_next_backup()]
    edges = network_edges_controller()
    return str(uuid), points, backup, edges


def jobs_visualize_controller(job_id):
    graph_traversal = bfs(node_info.get_uuid(), addresses.get_ip(), addresses.get_port())
    for node in graph_traversal:
        rc.job_data(edge=node, job_id=job_id)
