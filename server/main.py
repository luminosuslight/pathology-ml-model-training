from flask import Flask
from flask import request, abort

from hashlib import md5
import os
import os.path

UPLOAD_FOLDER = 'uploads'

app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER


def secure_filename(filename):
    return filename.replace("/", "").replace("\\", "")


@app.route('/version', methods=['GET'])
def version():
    return "1.0"


@app.route('/data', methods=['POST'])
def upload():
    raw_data = request.get_data()
    hash = md5(raw_data).hexdigest()
    path = os.path.join(app.config['UPLOAD_FOLDER'], hash)
    with open(path, 'wb') as file:
        file.write(raw_data)
    return hash


@app.route('/data/check/<hash>', methods=['GET'])
def check(hash):
    path = os.path.join(app.config['UPLOAD_FOLDER'], hash)
    return "1" if os.path.isfile(path) else "0"


@app.route('/data/<hash>', methods=['GET'])
def download(hash):
    path = os.path.join(app.config['UPLOAD_FOLDER'], hash)
    if not os.path.isfile(path):
        abort(404)

    with open(path, 'rb') as file:
        content = file.read()

    return content


@app.route('/data/<hash>', methods=['DELETE'])
def delete_data(hash):
    path = os.path.join(app.config['UPLOAD_FOLDER'], hash)
    if not os.path.isfile(path):
        abort(404)

    os.remove(path)
    return


#@app.route('/model', methods=['GET'])
#def all_models(model_id):
#    # get list of models
#    return list_of_models


#@app.route('/model/<model_id>', methods=['GET'])
#def model_metadata(model_id):
#    # get metadata
#    return model_metadata


#@app.route('/model/<model_id>/prediction/<hash>', methods=['GET'])
#def predict(model_id, hash):
#    # check if image was already uploaded
#    # load image data from disk
#    # predict
#    cbor = [full_output_hash, mask_hash?, centers]
#    return cbor


#@app.route('/model/train/<data_hash>', methods=['GET'])
#def train(data_hash):
#    data = None
#    train, val = data
#    epoch_count = -1
#    # create dir
#    # unpack data there
#    # create metadata file
#    # train in other thread
#    return model_id
