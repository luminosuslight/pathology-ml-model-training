from asyncore import file_dispatcher
import os
from PIL import Image
import cbor2
import csv
import urllib
import requests
import json
from io import BytesIO
import tqdm
from concurrent.futures import ThreadPoolExecutor

exported_images_folder = "exported_thumbnails_pond5"
thumbnails_path = '/nas/client_data/pond5/videos/'
output_file = "pond5_thumbnail_features.cbor"


def images_in_folder(path, extensions=(".jpg", ".jpeg", ".png")):
    return [os.path.join(path, name) for path, subdirs, files in os.walk(path)
            for name in files if name.lower().endswith(extensions)]
    
images = images_in_folder(thumbnails_path)

try:
    os.makedirs(exported_images_folder)
except:
    pass


def get_features(img_path, host='10.101.101.24:5200'):
    params = {'modules': ['search/image_features_ViT_warping']}
    with open(img_path, 'rb') as image:
        r = requests.post(
            'http://%s/system/features/extract'%(host),
            files={'data': image},
            data={'params': json.dumps(params)}
        )
    result = r.json()
    if not "features" in result:
        return None
    return result['features']['search/image_features_ViT_warping']

def get_row(image_path):
    try:
        image = Image.open(image_path)
        image.thumbnail((256, 256))
        image_name = image_path.split("/")[-1].replace(".png", ".jpg")
        image_path = os.path.join(exported_images_folder, image_name)
        image.save(image_path)
    except Exception as e:
        print(e)
        return None, None
    return image_name, get_features(image_path)


with ThreadPoolExecutor(max_workers=20) as executor:
    result_mixed = list(tqdm.tqdm(executor.map(get_row, images), total=len(images)))


image_names = []
features = []

for img_name, feature_vec in result_mixed:
    if not img_name:
        continue
    if not feature_vec:
        continue
    image_names.append(img_name)
    features.append(feature_vec)

with open(output_file, 'wb') as f:
    cbor2.dump((features, image_names), f)

