#!/bin/bash

for f in videos/*.mp4;
do
    # -hide_banner -loglevel error makes it quiet
    ffmpeg -hide_banner -loglevel error -ss "$(bc -l <<< "$(ffprobe -loglevel error -of csv=p=0 -show_entries format=duration "$f")*0.5")" -i "$f" -frames:v 1 "$f.png"
done
