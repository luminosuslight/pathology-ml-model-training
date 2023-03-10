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

exported_images_folder = "exported_images_pond5"
csv_path = 'mobius_photo_sample.csv'
output_file = "pond5_image_features.cbor"

try:
    os.makedirs(exported_images_folder)
except:
    pass


with open(csv_path, 'r') as csvfile:
    reader = csv.DictReader(csvfile, delimiter=',', quotechar='"')
    csv_data = list(reader)


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

def get_row(item):
    url = item['preview_url']
    image_id = url.split("/")[-1].replace(".jpeg", ".jpg")
#    try:
#        image = Image.open(BytesIO(requests.get(url).content), formats=["JPEG"])
#    except:
#        return None, None
#    image.thumbnail((256, 256))
    image_name = image_id
    image_path = os.path.join(exported_images_folder, image_name)
    with open(image_path, "wb") as file:
        file.write(requests.get(url).content)
#    image.save(image_path)
    return image_name, get_features(image_path)


with ThreadPoolExecutor(max_workers=20) as executor:
    result_mixed = list(tqdm.tqdm(executor.map(get_row, csv_data), total=len(csv_data)))


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

