import unittest
import requests


class ApiTestCase(unittest.TestCase):
    # TODO: read address and port from bootstrap config file
    api_address = 'http://localhost:5000/api/'

    def verify_json(self, json_data):
        self.assertIn('status', json_data)
        self.assertIn(json_data['status'], ['ok', 'error', 'wait'])
        if json_data['status'] == 'error':
            self.assertIn('message', json_data)
            self.assertNotEqual(len(json_data['message']), 0)
        pass

    def check_fields(self, fields, json_data):
        for field in fields:
            self.assertIn(field, json_data)


class BootstrapApiTest(ApiTestCase):
    def test_default(self):
        pass

    def test_hello(self):
        endpoint = self.api_address + 'hello'
        request_data = {"ip": "69.89.31.226", "port": "123"}

        r1 = requests.post(endpoint, json=request_data)
        json_data1 = r1.json()
        self.check_fields(['uuid'], json_data1)
        uuid1 = json_data1['uuid']
        self.verify_json(json_data1)

        r2 = requests.post(endpoint, json=request_data)
        json_data2 = r2.json()
        self.check_fields(['uuid'], json_data2)
        uuid2 = json_data2['uuid']
        self.verify_json(json_data2)

        self.assertEqual(json_data2['ip'], request_data['ip'])
        self.assertEqual(json_data2['port'], request_data['port'])
        self.assertEqual(int(uuid1) + 1, int(uuid2))

    def test_reset(self):
        requests.get(self.api_address + 'reset_done')

        r1 = requests.get(self.api_address + 'reset')
        json_data = r1.json()
        self.verify_json(json_data)
        self.check_fields(['can_reset'], json_data)
        self.assertEqual(json_data['can_reset'], 'true')

        r2 = requests.get(self.api_address + 'reset')
        json_data = r2.json()
        self.verify_json(json_data)
        self.check_fields(['can_reset'], json_data)
        self.assertEqual(json_data['can_reset'], 'false')

        requests.get(self.api_address + 'reset_done')

    def test_reset_done(self):
        pass

    def test_receive(self):
        pass


class NodeApiTest(ApiTestCase):
    def test_default(self):
        pass

    # Basic
    def test_basic_ok(self):
        pass

    def test_bsic_info(self):
        pass

    def test_basic_check(self):
        pass

    # Network
    def test_network_edges(self):
        pass

    def test_network_get_edge(self):
        pass

    def test_network_set_edge(self):
        pass

    def test_network_adopt(self):
        pass

    def test_network_reset(self):
        pass

    def test_network_visualize(self):
        pass

    # Jobs
    def test_jobs_add(self):
        pass

    def test_jobs_new(self):
        pass

    def test_jobs_all(self):
        pass

    def test_jobs_backup(self):
        pass

    def test_jobs_remove(self):
        pass

    def test_jobs_kill(self):
        pass

    def test_jobs_ids(self):
        pass

    def test_jobs_data(self):
        pass

    def test_jobs_visualize(self):
        pass
