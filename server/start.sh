eval "$(conda shell.bash hook)"
conda activate fastai_env
export FLASK_APP=main.py
python3 -m flask run --host='::' --port=5000 --cert=adhoc
conda deactivate 
