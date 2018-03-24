# stock-market-prediction
Stock market prediction for the next day closing price 

It uses a hybrid technique acquired from Particle Swarm Optimization along with Back Propagation algorithm of Artificial neural network to learn from the historical data and predict the next day's closing price.

Input: Taken from https://finance.yahoo.com/

Pre-Processing: Along with the given input like Open, High, Low, Close, Volume and Adj-Close I have included SMA5 (Simple Moving Average for last five days' data) and EMA5 (Exponential Moving Average for last five days' data).

Learning: Uses 8-30-1 Neural network configuration 

Output: Next day closing price

Accuracy: Approx 98% on the tested stocks like SBI, ICICI, etc
