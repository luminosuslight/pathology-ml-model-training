eval "$(conda shell.bash hook)"
conda activate fastai_env
export FLASK_APP=main.py
python3 -m flask run --host='::' --port=55712 --cert=luminosus_websocket.cert --key=luminosus_websocket.key
conda deactivate 
