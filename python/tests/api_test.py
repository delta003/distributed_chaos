import unittest


class ApiTestCase(unittest.TestCase):
    def test_json(self, json_data):
        self.assertIn('status', json_data)
        self.assertIn('message', json_data)
        self.assertIn(json_data['status'], ['ok', 'error', 'wait'])
        if json_data['status'] is 'error':
            self.assertNotEqual(len(json_data['message']), 0)
        pass


class BootstrapApiTest(ApiTestCase):
    def test_default(self):
        pass

    def test_hello(self):
        pass

    def test_reset(self):
        pass

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
