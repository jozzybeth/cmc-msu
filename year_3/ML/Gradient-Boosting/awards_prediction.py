from sklearn.feature_extraction.text import CountVectorizer
import pandas as pd
import numpy as np
from catboost import CatBoostRegressor
from sklearn.preprocessing import StandardScaler
from hyperopt import fmin, tpe, hp, Trials, STATUS_OK
from sklearn.model_selection import train_test_split
from hyperopt.pyll.base import scope
import warnings
from sklearn.metrics import mean_absolute_error

warnings.filterwarnings('ignore')


def preprocess_data(df):
    cat_col = ['genres', 'directors', 'filming_locations', 'keywords']
    gen_col = ['actor_0_gender', 'actor_1_gender', 'actor_2_gender']

    for col in cat_col:
        df[col] = df[col].apply(
            lambda x: ['unknown'] if isinstance(x, str) and x.lower() == 'unknown' else x)

    for col in cat_col:
        df[col] = df[col].apply(
            lambda x: ','.join(x) if isinstance(x, list) and all(isinstance(i, str) for i in x) else str(x))
        df[col] = df[col].fillna('')
        df[col] = df[col].astype(str)
        vect = CountVectorizer(token_pattern=r'[^,]+', max_features=50)
        transformed = vect.fit_transform(df[col]).toarray()
        df[[f"{col}_{i}" for i in range(transformed.shape[1])]] = transformed

    df = df.drop(cat_col, axis=1)

    for col in gen_col:
        df[col] = df[col].astype('category')

    for i in range(3):
        df[f'actor_{i}_popularity'] = df[f'actor_{i}_postogramm'] * df[f'actor_{i}_known_movies']
        df[f'actor_{i}_experience'] = df[f'actor_{i}_age'] / df[f'actor_{i}_known_movies'].clip(lower=1)

    num_cols = [
        'potions', 'questions', 'runtime', 'critics_liked', 'preorders',
        'release_year'
    ] + [
        f'actor_{i}_{feat}' for i in range(3)
        for feat in ['known_movies', 'postogramm', 'age']
    ]

    scaler = StandardScaler()
    df[num_cols] = scaler.fit_transform(df[num_cols])

    return df


def train_model_and_predict(train_file: str, test_file: str) -> np.ndarray:
    df_train = pd.read_json(train_file, lines=True)

    df_test = pd.read_json(test_file, lines=True)

    y_train = df_train['awards']
    df_train = df_train.drop(['awards'], axis=1)

    train_size = df_train.shape[0]
    dataset = pd.concat([df_train, df_test], axis=0, ignore_index=True)

    dataset = preprocess_data(dataset)

    X_train = dataset.iloc[:train_size]
    X_test = dataset.iloc[train_size:]

    cat_features = list(X_train.select_dtypes(include=['category']).columns)

    model = CatBoostRegressor(
        iterations=800,
        depth=8,
        learning_rate=0.030075199381415414,
        l2_leaf_reg=9.133616939858815,
        random_strength=0.5633298019963034,
        bagging_temperature=0.28057917901092694,
        cat_features=cat_features,
        verbose=False,
        train_dir='/tmp/catboost_info',
        random_seed=42
    )

    model.fit(X_train, y_train)
    return model.predict(X_test)
