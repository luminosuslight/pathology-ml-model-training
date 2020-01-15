eval "$(conda shell.bash hook)"
conda activate fastai_env
export FLASK_APP=main.py
python3 -m flask run --host=0.0.0.0 --port=5000
conda deactivate 
