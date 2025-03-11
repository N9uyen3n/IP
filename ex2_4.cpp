#include <ilcplex/ilocplex.h>
#include <iostream>
ILOSTLBEGIN

int main() {
    IloEnv env;
    try {
        const int types = 5;
        const int years = 3;

        int investmentCost[types][years] = {
            {10, 20, 15},
            {15, 15, 20},
            {12, 25, 20},
            {9, 15, 15},
            {13, 10, 10},
        };

        int expectedReturn[types] = {90, 120, 100, 80, 130};
        int moneyAvailable[years] = {45, 60, 50};

        IloModel model(env);
        IloArray<IloIntVarArray> x(env, types);
        for (int i = 0; i < types; i++) {
            x[i] = IloIntVarArray(env, years, 0, IloIntMax);
        }

        IloNumVarArray profit(env, years, 0, IloInfinity);

        for (int t = 0; t < years; t++) {
            IloExpr budget(env);
            for (int i = 0; i < types; i++) {
                budget += x[i][t] * investmentCost[i][t];
            }
            model.add(budget <= moneyAvailable[t]);
            budget.end();
        }

        for (int t = 0; t < years; t++) {
            IloExpr totalProfit(env);
            for (int i = 0; i < types; i++) {
                totalProfit += x[i][t] * expectedReturn[i];
            }
            model.add(profit[t] == totalProfit);
            totalProfit.end();
        }

        IloExpr totalProfitObjective(env);
        for (int t = 0; t < years; t++) {
            totalProfitObjective += profit[t];
        }
        model.add(IloMaximize(env, totalProfitObjective));
        totalProfitObjective.end();

        IloCplex cplex(model);
        if (!cplex.solve()) {
            cerr << "Failed to solve the investment optimization problem." << endl;
            return 1;
        }

        cout << "Solution status: " << cplex.getStatus() << std::endl;
        for (int t = 0; t < years; t++) {
            cout << "Year " << t + 1 << "\n";
            for (int i = 0; i < types; i++) {
                cout << "Stock " << i + 1 << ": " << cplex.getValue(x[i][t]) << " units\n";
            }
            cout << "Profit: " << cplex.getValue(profit[t]) << "\n\n";
        }

        cout << "Total Profit after 3 years: " << cplex.getObjValue() << "\n";
    }
    catch (IloException& e) {
        cerr << "Error: " << e << endl;
    }
    env.end();
    return 0;
}