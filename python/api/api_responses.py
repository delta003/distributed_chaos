from flask import jsonify, request


class InvalidJsonRequest(Exception):
    pass


def error_response(message):
    return jsonify({'status': 'error', 'message': message})


def ok_response(response):
    status = {'status': 'ok'}
    return jsonify({**response, **status})


def extract_data(request, fields):
    json_data = request.get_json()
    result = []
    for field in fields:
        if field not in json_data:
            raise InvalidJsonRequest()
        result.append(json_data[field])
    return result
