from flask import Flask
from flask import request, abort, send_from_directory
from werkzeug import serving
from flask_cors import CORS
import cbor2
import pickle
import numpy as np
import umap

from hashlib import md5
import os
import os.path
import sys
import threading
import uuid

print("Python version:", sys.version)

UPLOAD_FOLDER = 'uploads'
if not os.path.exists(UPLOAD_FOLDER):
    os.makedirs(UPLOAD_FOLDER)

app = Flask(__name__)
CORS(app)
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER


def log_request(self, *args, **kwargs):
    # don't log progress requests:
    filters = ['training_progress', 'inference_progress']
    if any(filter in self.requestline for filter in filters):
        return
    _log_request(self, *args, **kwargs)


_log_request = serving.WSGIRequestHandler.log_request
serving.WSGIRequestHandler.log_request = log_request


def secure_filename(filename):
    return filename.replace("/", "").replace("\\", "")


@app.route('/version', methods=['GET'])
def version():
    return "1.0", 200


@app.route('/<path:path>', methods=['GET'])
def static_wasm_files(path):
    return send_from_directory('wasm', path), 200


@app.route('/projects/<path:path>', methods=['GET'])
def projects(path):
    return send_from_directory('projects', path), 200


def store_in_uploads(raw_data):
    hash = md5(raw_data).hexdigest()
    path = os.path.join(app.config['UPLOAD_FOLDER'], hash)
    with open(path, 'wb') as file:
        file.write(raw_data)
    return hash


@app.route('/data', methods=['POST'])
def upload():
    raw_data = request.get_data()
    hash = store_in_uploads(raw_data)
    return hash, 200


@app.route('/data/check/<hash>', methods=['GET'])
def check(hash):
    path = os.path.join(app.config['UPLOAD_FOLDER'], hash)
    return ("1" if os.path.isfile(path) else "0"), 200


def get_upload(file_hash):
    data_path = os.path.join(app.config['UPLOAD_FOLDER'], file_hash)
    if not os.path.isfile(data_path):
        abort(404)
    with open(data_path, 'rb') as file:
        content = file.read()
    return content


@app.route('/data/<hash>', methods=['GET'])
def download(hash):
    content = get_upload(hash)
    return content, 200


@app.route('/data/<hash>', methods=['DELETE'])
def delete_data(hash):
    path = os.path.join(app.config['UPLOAD_FOLDER'], hash)
    if not os.path.isfile(path):
        abort(404)

    os.remove(path)
    return '', 204


@app.route('/training_progress', methods=['GET'])
def training_progress():
    #return str(training_tracker.progress), 200
    return str(0.0), 200


@app.route('/inference_progress', methods=['GET'])
def inference_progress():
    #return str(max(NeuralNetwork.progress, TrainedAutoencoder.progress)), 200
    return str(0.0), 200


with open('unsplash_alt_descriptions_with_CLIP_features.pkl', 'rb') as f:
    captions, caption_features = pickle.load(f)


@app.route('/caption', methods=['POST'])
def get_caption():
    raw_data = request.get_data()
    reference_features = cbor2.loads(raw_data)
    dist = caption_features.dot(reference_features)
    caption = captions[np.argsort(dist)[-1]]
    return caption, 200


@app.route('/umap', methods=['POST'])
def run_umap():
    raw_data = request.get_data()
    data = cbor2.loads(raw_data)
    input_data = data["inputData"]
    embedding = umap.UMAP(n_neighbors=data["neighbours"],
                          min_dist=data["minDistance"],
                          metric=data["metric"]).fit_transform(input_data)
    return cbor2.dumps(list(list(float(x) for x in p) for p in embedding)), 200


if __name__ == "__main__":
    app.run(debug=True, host="0.0.0.0", port=55712)
