import pandas as pd

iterationCount = 5
for i in range(iterationCount):
    print(f"Predator iteration {i+1}")
    csv = pd.read_csv(f"analysis/predator_output_{i}.csv")
    top_alive = csv.sort_values(by='alive', ascending=True).head(1)
    print(top_alive[['gamma1','gamma2','k','alive']].describe().loc[['mean', 'std']])
    print(f"Prey iteration {i+1}")
    csv = pd.read_csv(f"analysis/prey_output_{i}.csv")
    top_alive = csv.sort_values(by='alive', ascending=False).head(1)
    print(top_alive[['alpha','beta','gamma','delta','alive']].describe().loc[['mean', 'std']])