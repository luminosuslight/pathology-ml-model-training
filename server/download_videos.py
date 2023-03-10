import requests
import tqdm
import csv
import os
from concurrent.futures import ThreadPoolExecutor


with open('mobius_footage_sample.csv', 'r') as csvfile:
    reader = csv.DictReader(csvfile, delimiter=',', quotechar='"')
    csv_file = list(reader)


os.makedirs("/nas/client_data/pond5/videos")


def download_video(item):
    url = item["preview_url"]
    with open(f"/nas/client_data/pond5/videos/{url.split('/')[-1]}", "wb") as file:
        response = requests.get(url)
        file.write(response.content)
    
    return True


with ThreadPoolExecutor(max_workers=20) as executor:
    result_mixed = list(tqdm.tqdm(executor.map(download_video, csv_file), total=len(csv_file)))
    