#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

int main() {
    IloEnv env;
    try {
        const int days = 7;
        const int types = 5;

        int matrix[days][types] = {
            {1, 0, 1, 0, 0},
            {1, 0, 0, 0, 1},
            {0, 0, 1, 1, 0},
            {0, 1, 0, 0, 1},
            {0, 1, 0, 1, 1},
            {0, 1, 0, 1, 0},
            {1, 0, 1, 0, 0}
        };

      
        int weekly_wake[types] = {525, 470, 550, 500, 425}; // tiền mỗi loại lịch.
        int nurses_required[days] = {20, 25, 26, 26, 30, 30, 35}; // số y tá cần mỗi ngày.

        IloModel model(env);

        IloNumVarArray nurses(env, types, 0, IloInfinity, ILOINT); // y tá ở mỗi ngày.

        for (int i = 0; i < days; i++) {
            IloExpr coverage(env);
            for (int j = 0; j < types; j++) {
                coverage += matrix[i][j] * nurses[j];
            }
            model.add(coverage >= nurses_required[i]); // lượng y tá cần có thể lớn hơn.
            coverage.end();
        }

        IloExpr totalCost(env);
        for (int j = 0; j < types; j++) {
            totalCost += weekly_wake[j] * nurses[j];
        }
        model.add(IloMinimize(env, totalCost));
        totalCost.end();

        IloCplex cplex(model);
        if (cplex.solve()) {
            env.out() << "Optimal solution found:\n";
            for (int j = 0; j < types; j++) {
                env.out() << "Schedule type " << (j + 1) << ": " << cplex.getValue(nurses[j]) << "\n";
            }
            env.out() << "Total cost: " << cplex.getObjValue() << "\n";
        } else {
            env.out() << "No optimal solution found.\n";
        }
    }
    catch (IloException& e) {
        env.out() << "Error: " << e << "\n";
    }
    
    env.end();
    return 0;
}
