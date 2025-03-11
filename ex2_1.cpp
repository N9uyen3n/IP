#include <ilcplex/ilocplex.h>
#include <vector>
#include <iostream>

using namespace std;

ILOSTLBEGIN

int main() {
    IloEnv env;
    try {
        // --- Dữ liệu bài toán ---
        int numFoods = 5;
        int numDrinks = 3;
        double W = 30, L = 10;
        vector<double> c = {2, 3, 1.5, 2.5, 3.2}; // Chi phí thực phẩm
        vector<double> d = {1.8, 2.2, 2.5}; // Chi phí đồ uống
        vector<double> M = {10, 12, 8, 15, 9}; // Giới hạn tiêu thụ thực phẩm
        vector<double> U = {5, 7, 6}; // Giới hạn tiêu thụ đồ uống

        IloModel model(env);

        IloIntVarArray x(env, numFoods);
        for (int i = 0; i < numFoods; i++) {
            x[i] = IloIntVar(env, 0, M[i]);
        }

        IloIntVarArray y(env, numDrinks);
        for (int j = 0; j < numDrinks; j++) {
            y[j] = IloIntVar(env, 0, U[j]);
        }

        IloIntVarArray z(env, numDrinks);
        for (int j = 0; j < numDrinks; j++) {
            z[j] = IloIntVar(env, 0, 1);
        }

        IloExpr objective(env);
        for (int i = 0; i < numFoods; i++) {
            objective += c[i] * x[i];
        }
        for (int j = 0; j < numDrinks; j++) {
            objective += d[j] * y[j];
        }
        model.add(IloMinimize(env, objective));
        objective.end();

        IloExpr totalConsumption(env);
        for (int i = 0; i < numFoods; i++) {
            totalConsumption += x[i];
        }
        for (int j = 0; j < numDrinks; j++) {
            totalConsumption += y[j];
        }
        model.add(totalConsumption >= W + L);
        totalConsumption.end();

        IloExpr drinkSelection(env);
        for (int j = 0; j < numDrinks; j++) {
            drinkSelection += z[j];
        }
        model.add(drinkSelection <= 2);
        drinkSelection.end();

        for (int j = 0; j < numDrinks; j++) {
            model.add(y[j] <= U[j] * z[j]);
        }

        IloCplex cplex(model);
        cplex.setOut(env.getNullStream()); 
        if (cplex.solve()) {
            cout << "Trạng thái giải: " << cplex.getStatus() << endl;
            cout << "Giá trị hàm mục tiêu: " << cplex.getObjValue() << endl;
            for (int i = 0; i < numFoods; i++) {
                cout << "Số ounce thực phẩm loại " << (i+1) << " (x[" << i << "]): " 
                     << cplex.getValue(x[i]) << endl;
            }
            for (int j = 0; j < numDrinks; j++) {
                cout << "Số ounce đồ uống loại " << (j+1) << " (y[" << j << "]): " 
                     << cplex.getValue(y[j]) << endl;
            }
        } else {
            cout << "Không tìm được lời giải khả thi!" << endl;
        }
    }
    catch (IloException& e) {
        cerr << "IloException: " << e << endl;
    }
    catch (...) {
        cerr << "Unknown exception caught" << endl;
    }
    env.end();
    return 0;
}