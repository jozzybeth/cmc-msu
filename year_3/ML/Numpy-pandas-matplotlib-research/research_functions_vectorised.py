import numpy as np

def are_multisets_equal(x: np.ndarray, y: np.ndarray) -> bool:
    set1, c1 = np.unique(x, return_counts=True)
    set2, c2 = np.unique(y, return_counts=True)
    return np.array_equal(set1, set2) and np.array_equal(c1,c2)

def max_prod_mod_3(x: np.ndarray) -> int:
    result = x[1:] * x[:-1]
    mask = result % 3 != 0
    result[mask] = -1
    if result.size == 0:
        return -1
    return np.max(result)

def convert_image(image: np.ndarray, weights: np.ndarray) -> np.ndarray:
    w = image * weights
    return np.sum(w, axis=2)
    
def rle_scalar(x: np.ndarray, y: np.ndarray) -> int:
    vec1 = np.repeat(x[:,0],x[:,1])
    vec2 = np.repeat(y[:,0],y[:,1])
    if len(vec1) != len(vec2):
        return -1
    return np.dot(vec1,vec2)     

def cosine_distance(X: np.ndarray, Y: np.ndarray) -> np.ndarray:
    norm_X = np.linalg.norm(X, axis=1, keepdims=True)  # n x 1
    norm_Y = np.linalg.norm(Y, axis=1, keepdims=True)  # m x 1

    mask_X = norm_X.flatten() == 0
    mask_Y = norm_Y.flatten() == 0
    
    norm_X[mask_X] = 1
    norm_Y[mask_Y] = 1

    M = X @ Y.T / (norm_X * norm_Y.T)

    M[mask_X, :] = 1  
    M[:, mask_Y] = 1  

    return M



import numpy as np
import matplotlib.pyplot as plt
from sklearn.linear_model import Ridge
from sklearn.metrics import mean_squared_error
from sklearn.preprocessing import MinMaxScaler

class Reservoir:
    def __init__(self, n_reservoir, n_inputs, spectral_radius=0.9, sparsity=0.1, leaking_rate=0.6):
        self.n_reservoir = n_reservoir
        self.n_inputs = n_inputs
        self.leaking_rate = leaking_rate

        self.W_in = np.random.uniform(-1, 1, (n_reservoir, n_inputs + 1))
        self.W = np.random.uniform(-1, 1, (n_reservoir, n_reservoir))
        mask = np.random.rand(*self.W.shape) > sparsity  # Разреженность матрицы
        self.W[mask] = 0
        eigvals = np.linalg.eigvals(self.W)
        self.W *= spectral_radius / np.max(np.abs(eigvals))  # Нормировка матрицы резервуара

        self.state = np.zeros((n_reservoir, 1))  # Состояние резервуара

    def update_state(self, input_vector):  
        input_vector_with_bias = np.vstack((np.array([[1]]), input_vector))
        state = np.tanh(np.dot(self.W, self.state) + np.dot(self.W_in, input_vector_with_bias))
        self.state = (1 - self.leaking_rate) * self.state + self.leaking_rate * state
        return self.state.flatten()

class EchoStateNetwork:
    def __init__(self, n_inputs, n_reservoir, n_outputs, spectral_radius=0.9, sparsity=0.3, leaking_rate=0.6):
        self.n_inputs = n_inputs
        self.n_reservoir = n_reservoir
        self.n_outputs = n_outputs
        self.reservoir = Reservoir(n_reservoir, n_inputs, spectral_radius, sparsity,leaking_rate)
        self.scaler = MinMaxScaler(feature_range=(-1, 1))

    def fit(self, inputs, outputs, reg=1e-5):
        inputs_scaled = self.scaler.fit_transform(inputs)
        outputs_scaled = self.scaler.transform(outputs)
        states = []
        for input_vector in inputs_scaled:
            state = self.reservoir.update_state(input_vector)
            states.append(state)
        states = np.array(states)
        states_with_bias = np.hstack((np.ones((states.shape[0], 1)), states))  # Добавление единичного столбца для смещения
        ridge = Ridge(alpha=reg)
        ridge.fit(states_with_bias, outputs_scaled)
        self.W_out = ridge.coef_
        self.W_out_bias = ridge.intercept_

    def predict(self, warmup_sequence, n_steps):  # Прогнозирование с использованием W_out
        predictions_scaled = []
        state = np.zeros(self.n_reservoir)
        warmup_scaled = self.scaler.transform(warmup_sequence)
        # Прогреваем сеть
        for input_vector in warmup_scaled:
            state = self.reservoir.update_state(input_vector)

        last_input = warmup_scaled[-1].reshape(-1, 1)
        for _ in range(n_steps):
            state = self.reservoir.update_state(last_input)
            state = state.reshape(-1, 1)
            last_input = last_input.reshape(-1, 1)
            state_with_bias = np.vstack((np.array([[1]]), state))  # Добавление смещения
            prediction = np.dot(self.W_out, state_with_bias) + self.W_out_bias
            predictions_scaled.append(prediction)
            last_input = prediction[-1].reshape(-1, 1)
        predictions_scaled = np.array(predictions_scaled).reshape(-1, 1)
        predictions = self.scaler.inverse_transform(predictions_scaled)
        return predictions.flatten()



def test2():
    n_samples = 400
    n_predict = 200
    n_reservoir = 100
    spectral_radius = 0.8
    sparsity = 0.3
    leaking_rate = 0.35
    n_warmup = 150
    t = np.linspace(0, 20 * np.pi, n_samples)
    y = (np.sin(2.2*t) + np.sin(2*t))/2

    esn = EchoStateNetwork(n_inputs=1, n_reservoir=n_reservoir, n_outputs=1, spectral_radius=spectral_radius, sparsity=sparsity, leaking_rate=leaking_rate)
    esn.fit(y[:-n_predict].reshape(-1, 1), y[1:-n_predict+1].reshape(-1, 1))

    warmup_inputs = y[-n_predict - n_warmup:-n_predict].reshape(-1, 1)
    prediction = esn.predict(warmup_inputs, n_steps=n_predict)


    plt.figure(figsize=(12, 6))
    plt.plot(t, y, label='Синусоида sin(t)', color='blue')

    x_predict = t[-n_predict:]
    plt.plot(x_predict, prediction, label='Предсказания ESN',color='red')

    plt.legend(loc='center', bbox_to_anchor=(0.5, -0.1), ncol=1)
    plt.title('Генерация синусоидального сигнала с помощью ESN')
    plt.xlabel('t')
    plt.ylabel('y')
    plt.grid()
    plt.show()
    mse = mean_squared_error(y[-n_predict:], prediction[-n_predict:])
    print("Среднеквадратическая ошибка (MSE):", mse)

test2()
