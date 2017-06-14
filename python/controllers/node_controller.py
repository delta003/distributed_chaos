from controllers.request_creator import create_request, create_bootstrap_request
import threading
import time
import queue


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
    prev_ip = prev_edge['ip']
    prev_port = prev_edge['port']

    try:
        create_request(method='GET', endpoint='/api/basic/ok', ip=next_ip, port=next_port)
    except Exception as e:
        failures = failures + 1
        if failures == 5:
            try:
                [alive] = create_request(method='GET', endpoint='/api/basic/check', ip=prev_ip, port=prev_port,
                                         data={'ip': next_ip, 'port': next_port}, fields=['alive'])
                if alive == 'false':
                    reconfigure()
                else:
                    failures = 0
            except Exception as e:
                reconfigure()


def reconfigure():
    [can_reset] = create_bootstrap_request(method='GET', endpoint='/api/reset', fields=['can_reset'])
    if can_reset == 'false':
        return

    graph_traversal = bfs(node_info.get_uuid(), addresses.get_ip(), addresses.get_port())
    for node in graph_traversal:
        if node['uuid'] == node_info.get_uuid():
            continue
        create_request(method='GET', endpoint='/api/network/reset', ip=node['ip'], port=node['port'])

    links.reset()
    time.sleep(5)
    create_bootstrap_request(method='GET', endpoint='/api/reset_done')


def bfs(uuid, ip, port):
    q = queue.Queue()
    q.put({'uuid': uuid, 'ip': ip, 'port': port})
    visited = {uuid: True}
    result = []
    while not q.empty():
        curr = q.get()
        result.append(curr)
        [edges] = create_request(method='GET', endpoint='/api/network/edges',
                                 ip=curr['ip'], port=curr['port'], fields=['edges'])

        for e in edges:
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
        [mng_ip, mng_port, uuid] = \
            create_bootstrap_request(method='POST', endpoint='/api/hello', fields=['ip', 'port', 'ip'],
                                     data={'ip': addresses.get_ip(), 'port': addresses.get_port()})
    except ValueError:
        if node_info.get_uuid() is None:
            node_info.set_uuid(uuid)
        links.set_next(uuid=uuid, ip=addresses.get_ip(), port=addresses.get_port())
        links.set_prev(uuid=uuid, ip=addresses.get_ip(), port=addresses.get_port())
        return
    if node_info.get_uuid() is None:
        node_info.set_uuid(uuid)

    try:
        parent_edge = create_request(method='POST', ip=mng_ip, port=mng_port, endpoint='/api/network/get_edge',
                                     fields=['edge'], data={'type': 'parent'})
    except ValueError:
        # parent is None
        mng_prev = create_request(method='POST', ip=mng_ip, port=mng_port, endpoint='/api/network/get_edge',
                                  fields='edge', data={'type': 'prev'})
        mng_next = create_request(method='POST', ip=mng_ip, port=mng_port, endpoint='/api/network/get_edge',
                                  fields='edge', data={'type': 'next'})

        if mng_prev['uuid'] == mng_next['uuid']:
            request_data = {'edge': {'uuid': str(uuid), 'ip': addresses.get_ip(), 'port': addresses.get_port()},
                            'type': 'next'}
            x_nxt = create_request(method='POST', ip=mng_ip, port=mng_port, endpoint='api/network/set_edge',
                                   fields='oldedge', data=request_data)

            create_request('POST', ip=x_nxt['ip'], port=x_nxt['port'])
        return


# parent = x.get_edge(parent)
# 	if parent is null:
# 		edges = x.edges  // Edges of x
# 		if edges.prev == edges.next:
# 			// Node joins top level circle
# 			xNext = x.set_edge(next, this)  // na mngr->next = this
# 			xNext.set_edge(prev, this)  // This should return x, you can assert
# 			this.prev = X
# 			this.next = xNext
# 		else:
# 			// Node is first in second level
# 			x.adopt(this)
# 			this.parent = x
# 			this.next = this;
# 			this.prev = this;



# 	else:
# 		// Standard join
# 		parent.adopt(this, can_redirect=true)
# 		if redirected to y:
# 			y.adopt(this, can_redirect=false)
# 			parent = y
# 		this.parent = parent
# 		if not create_level:
# 			// Join layer
# 			xNext = x.set_edge(next, this)
# 			xNext.set_edge(prev, this)  // This should return x, you can assert
# 			this.prev = x
# 			this.next = xNext
# 			return
# 		// New level should be created, adopt returned edges with 5 children
# 		thisPrev0 = edges.child[0] // Zero based index, this one gets two new children
# 		thisPrev1 = edges.child[1] // This one gets one child
# 		thisPrev2 = edges.child[2]  // First node in new level
# 		thisPrev3 = edges.child[3] // This should be x, you can assert
# 		// Exclude thisPrev2 and thisPrev3 from layer
# 		newNext = x.get_edge(next)
# 		newPrev = thisPrev2.get_edge(prev)
# 		// Previous level linking
# 		newNext.set_edge(prev, newPrev)
# 		newPrev.set_edge(next, newNext)
# 		// Change parents
# 		thisPrev0.adopt(thisPrev2) // this will succeed, you can assert
# 		thisPrev2.set_edge(parent, thisPrev0)
# 		thisPrev0.adopt(thisPrev3) // this will succeed, you can assert
# 		thisPrev3.set_edge(parent, thisPrev0)
# 		thisPrev1.adopt(this) // this will succeed, you can assert
# 		this.parent = thisPrev1
# 		// Make circle in new layer
# 		thisPrev3.set_edge(next, this)
# 		thisPrev2.set_edge(prev, this)
# 		this.next = thisPrev2
# 		this.prev = thisPrev3
# 		// Log something happy :)

def basic_ok_controller():
    return {}


def basic_info_controller():
    return {'uuid': node_info.get_uuid(), 'ip': addresses.get_ip(), 'port': addresses.get_port()}


def basic_check_controller(ip, port):
    try:
        create_request(method='GET', endpoint='/api/basic/ok', ip=ip, port=port)
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
