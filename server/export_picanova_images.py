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

exported_images_folder = "exported_images_picanova"

try:
    os.makedirs(exported_images_folder)
except:
    pass


with open('image_tags_snapshot.csv', 'r') as csvfile:
    reader = csv.DictReader(csvfile, delimiter=',', quotechar='"')
    picanova_csv = list(reader)


def get_features(img_path, host='10.101.101.22:5200'):
    params = {'modules': ['search/image_features']}
    with open(img_path, 'rb') as image:
        r = requests.post(
            'http://%s/system/features/extract'%(host),
            files={'data': image},
            data={'params': json.dumps(params)}
        )
    result = r.json()
    return result['features']['search/image_features']

def get_row(item):
    url = item['image']
    image_id = url.split("image=")[1].split("&")[0]
    #try:
    #    image = Image.open(BytesIO(requests.get(url).content), formats=["JPEG"])
    #except:
    #    return None, None
    #image.thumbnail((256, 256))
    image_name = image_id + ".jpg"
    image_path = os.path.join(exported_images_folder, image_name)
    #image.save(image_path)
    return image_name, get_features(image_path)


with ThreadPoolExecutor(max_workers=20) as executor:
    result_mixed = list(tqdm.tqdm(executor.map(get_row, picanova_csv), total=len(picanova_csv)))


image_names = []
features = []

for img_name, feature_vec in result_mixed:
    if not img_name:
        continue
    image_names.append(img_name)
    features.append(feature_vec)

with open('picanova_features.cbor', 'wb') as f:
        cbor2.dump((features, image_names), f)

