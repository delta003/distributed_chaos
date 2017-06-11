from flask import jsonify


def error_response(message):
    return jsonify({"status": "error", "message": message})
