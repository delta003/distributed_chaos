import queue
import threading
import time
import requests.request_creator as rc


def __init__(link_data, address_data, data, job_data):
    global links, addresses, node_info, jobs
    links = link_data
    addresses = address_data
    node_info = data
    jobs = job_data


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
        rc.reset(edge=e)
    links.reset()
    time.sleep(5)
    rc.bootstrap_reset_done()


def bfs(uuid, ip, port):
    q = queue.Queue()
    q.put({'uuid': uuid, 'ip': ip, 'port': port})
    visited = {uuid: True}
    result = []
    while not q.empty():
        curr = q.get()
        result.append(curr)
        edge_list = rc.edges(edge=curr)
        for e in edge_list:
            next_uuid = int(e['uuid'])
            if next_uuid in visited:
                continue
            visited[next_uuid] = True
            q.put({'uuid': next_uuid, 'ip': e['ip'], 'port': e['port']})
    return result


# joining node into network
def join():
    links.set_wait(True)

    try:
        uuid, mng_ip, mng_port = rc.bootstrap_hello(ip=addresses.get_ip(), port=addresses.get_port())
        mng_edge = {'ip': mng_ip, 'port': mng_port}
    except ValueError:
        if node_info.get_uuid() is None:
            node_info.set_uuid(uuid)
        links.set_next(uuid=uuid, ip=addresses.get_ip(), port=addresses.get_port())
        links.set_prev(uuid=uuid, ip=addresses.get_ip(), port=addresses.get_port())
        return
    this = {'uuid': str(uuid), 'ip': addresses.get_ip(), 'port': addresses.get_port()}
    if node_info.get_uuid() is None:
        node_info.set_uuid(uuid)
    try:
        parent_edge = rc.get_edge(edge=mng_edge, type='parent')
    except ValueError:
        # parent is None
        mng_prev = rc.get_edge(edge=mng_edge, type='prev')
        mng_next = rc.get_edge(edge=mng_edge, type='next')

        if mng_prev['uuid'] == mng_next['uuid']:
            x_nxt = rc.set_edge(edge=mng_edge, e=this, type='next')
            rc.set_edge(edge=x_nxt, e=this, type='prev')
            links.set_prev(uuid=x_nxt['uuid'], ip=x_nxt['ip'], port=x_nxt['port'])
            links.set_next(uuid=x_nxt['uuid'], ip=x_nxt['ip'], port=x_nxt['port'])

        else:
            rc.adopt(mng_edge, this)
            mng_edge['type'] = 'parent'
            links.set_edge(mng_edge)
            this['type'] = 'next'
            links.set_edge(this)
            this['type'] = 'prev'
            links.set_edge(this)
        return

    redirected, level_created, children, next = rc.adopt(parent_edge, this, can_redirect=True)
    if redirected:
        rc.adopt(edge=next, e=this, can_redirect=False)
        parent = next

    parent_edge['type'] = 'parent'
    links.set_edge(parent_edge)

    if not level_created:
        x_nxt = rc.set_edge(edge=mng_edge, e=this, type='next')
        rc.set_edge(edge=x_nxt, e=this, type='prev')
        mng_edge['type'] = 'prev'
        links.set_edge(mng_edge)
        x_nxt['type'] = 'next'
        links.set_edge(x_nxt)
        return

    this_prev0 = children[0]
    this_prev1 = children[1]
    this_prev2 = children[2]
    this_prev3 = children[3]

    # link previous level
    new_next = rc.get_edge(edge=mng_edge, type='next')
    new_prev = rc.get_edge(edge=mng_edge, type='prev')

    # change parents
    rc.set_edge(edge=new_next, e=new_prev, type='prev')
    rc.set_edge(edge=new_prev, e=new_next, type='next')

    rc.adopt(edge=this_prev0, e=this_prev2)
    rc.set_edge(edge=this_prev2, e=this_prev0, type='parent')

    rc.adopt(edge=this_prev0, e=this_prev3)
    rc.set_edge(edge=this_prev3, e=this_prev0, type='parent')

    rc.adopt(edge=this_prev1, e=this)
    rc.set_edge(edge=this, e=this_prev1, type='parent')

    # make circles in new layer
    rc.set_edge(edge=this_prev3, e=this, type='next')
    rc.set_edge(edge=this_prev2, e=this, type='prev')
    links.set_next(uuid=this_prev2['uuid'], ip=this_prev2['ip'], port=this_prev2['port'])
    links.set_prev(uuid=this_prev3['uuid'], ip=this_prev3['ip'], port=this_prev3['port'])


def basic_ok_controller():
    return {}


def basic_info_controller():
    return {'uuid': node_info.get_uuid(), 'ip': addresses.get_ip(), 'port': addresses.get_port()}


def basic_check_controller(ip, port):
    try:
        edge = {'ip': ip, 'port': port}
        rc.basic_ok(edge)
    except Exception as e:
        return {'alive': 'false'}  # TODO: zavisi koji je Exception
    return {'alive': 'true'}


def network_edges_controller():
    ret = []
    if links.get_parent() is not None:
        ret.append(links.get_parent())

    if links.get_prev() is not None:
        ret.append(links.get_prev())

    if links.get_next() is not None:
        ret.append(links.get_next())

    ret = ret + links.get_children()
    return {"edges": ret}


def network_get_edge_controller(type):
    return links.get_edge_as_dict(type)


def network_set_edge_controller(edge):
    ret = links.get_edge_as_dict(type)
    links.set_edge(edge)
    return ret


# TODO: refactor responses so that they rely on requests.response_creator.py
def network_adopt_controller(edge, can_redirect):
    if can_redirect == 'true':
        can_redirect = True
    else:
        can_redirect = False
    children_count = len(links.get_children())
    if children_count == 0 or children_count == 1 or children_count == 3:
        links.add_child(uuid=edge['uuid'], ip=edge['ip'], port=edge['port'])
        return {'redirect': 'false', 'create_level': 'false'}
    elif children_count == 2:
        if can_redirect:
            ret = links.get_edge_as_dict('next')
            ret['redirect'] = 'true'
            return ret
        else:
            links.add_child(uuid=edge['uuid'], ip=edge['ip'], port=edge['port'])
            return {'redirect': 'false', 'create_level': 'false'}
    elif children_count == 4:
        links.add_child(uuid=edge['uuid'], ip=edge['ip'], port=edge['port'])
        ret = network_edges_controller()
        ret['redirect'] = 'false'
        ret['create_level'] = 'true'
        for i in range(3):
            links.remove_child_at(2)
        return ret
    raise Exception('Undefined state')


def network_reset_controller():
    ret = links.get_edge_as_dict()
    links.reset()
    join_thrd = threading.Thread(target=join)
    join_thrd.start()
    return ret


def jobs_add_controller(jobid, width, height, p, points):
    pass


def jobs_new_controller(width, height, p, points):
    pass


def jobs_all_controller():
    pass


def jobs_backup_controller(uuid, jobid, point):
    pass


def jobs_remove_controller(jobid):
    pass


def jobs_kill_controller(jobid):
    pass


def jobs_ids_controller():
    pass


def jobs_data_controller(jobid):
    pass


def jobs_visualize_controller(jobid):
    pass