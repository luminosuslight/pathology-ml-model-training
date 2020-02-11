from flask import Flask
from flask import request, abort, send_from_directory
from werkzeug import serving
from flask_cors import CORS
import cbor2

from hashlib import md5
import os
import os.path
import sys
import threading

from inference import NeuralNetwork
from train import unpack_data_and_train, training_tracker

print("Python version:", sys.version)

# default_network = NeuralNetwork('/home/tim/Masterarbeit/artificial_data/input')
default_network = NeuralNetwork('models/7c0c7084ac8007ab0c24a3ee563e349c/input')

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


@app.route('/wasm/<path:path>', methods=['GET'])
def wasm(path):
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


# @app.route('/model', methods=['GET'])
# def all_models(model_id):
#    # get list of models
#    return list_of_models


@app.route('/model', methods=['POST'])
def train():
    raw_data = request.get_data()
    params = cbor2.loads(raw_data)
    print(params)

    base_model = params['baseModel']
    if base_model:
        model_id = f"{base_model}-{params['trainDataHash']}"
    else:
        model_id = params['trainDataHash']

    train_data = cbor2.loads(get_upload(params['trainDataHash']))
    valid_data = cbor2.loads(get_upload(params['validDataHash']))

    thread = threading.Thread(target=unpack_data_and_train,
                              args=(params, model_id, base_model, train_data, valid_data))
    thread.start()

    return model_id, 200


@app.route('/training_progress', methods=['GET'])
def training_progress():
    return str(training_tracker.progress), 200


# @app.route('/model/<model_id>', methods=['GET'])
# def model_metadata(model_id):
#    # get metadata
#    return model_metadata


@app.route('/model/<model_id>/prediction/<img_hash>/<left>/<top>/<right>/<bottom>', methods=['GET'])
def predict(model_id, img_hash, left, top, right, bottom):
    left, top, right, bottom = map(int, (left, top, right, bottom))
    model = default_network
    if model_id != "default":
        model = NeuralNetwork('models/' + model_id + '/input')

    path = os.path.join(app.config['UPLOAD_FOLDER'], img_hash)
    if not os.path.isfile(path):
        print("Image not found:", img_hash)
        abort(404)

    print(f"Doing inference with model '{model_id}' and file {img_hash}...")

    output_img_data, centers = model.get_output_and_centers(path, left, top, right, bottom)

    if model_id != "default":
        model.destroy()

    print(f"Inference complete, now storing result and sending it back...")

    output_hash = store_in_uploads(output_img_data)

    result = {'outputImageHash': output_hash,
              'cellCenters': centers}

    result_cbor = cbor2.dumps(result)
    return result_cbor, 200


@app.route('/inference_progress', methods=['GET'])
def inference_progress():
    return str(default_network.progress), 200

# @app.route('/model/train/<data_hash>', methods=['GET'])
# def train(data_hash):
#    data = None
#    train, val = data
#    epoch_count = -1
#    # create dir
#    # unpack data there
#    # create metadata file
#    # train in other thread
#    return model_id
