#include <ilcplex/ilocplex.h>
#include <vector>
#include <iostream>

using namespace std;

ILOSTLBEGIN

int main() {
    IloEnv env;
    try {
        // --- Dữ liệu bài toán ---
        int n = 5;
        int L = 20;
        vector<int> l = {3, 4, 2, 6, 1};
        vector<int> d = {3, 2, 1, 1, 1};
        double C = 50;    
        double R = 2;    

        IloModel model(env);

        // y: số lượng cuộn vải sử dụng (biến nguyên không âm)
        IloIntVar y(env, 0, IloIntMax);
        
        // x[i]: số lượng đoạn vải loại i cắt ra từ mỗi cuộn vải (biến nguyên không âm)
        IloIntVarArray x(env, n);
        for (int i = 0; i < n; i++) {
            x[i] = IloIntVar(env, 0, IloIntMax);
        }

        // --- Hàm mục tiêu ---
        IloExpr totalWaste(env);
        for (int i = 0; i < n; i++) {
            totalWaste += x[i] * l[i];
        }
        IloExpr objectiveExpr(env);
        objectiveExpr = y * C + (y * L - totalWaste) * R;
        model.add(IloMinimize(env, objectiveExpr));
        
        totalWaste.end();
        objectiveExpr.end();

        // --- Ràng buộc ---
        for (int i = 0; i < n; i++) {
            IloExpr demandConstraint(env);
            demandConstraint = x[i] * y;
            model.add(demandConstraint >= d[i]);
            demandConstraint.end();
        }

        IloExpr lengthExpr(env);
        for (int i = 0; i < n; i++) {
            lengthExpr += x[i] * l[i];
        }
        model.add(lengthExpr <= L * y);
        lengthExpr.end();

        // --- Giải bài toán ---
        IloCplex cplex(model);
        cplex.setOut(env.getNullStream()); 
        if (cplex.solve()) {
            cout << "Trạng thái giải: " << cplex.getStatus() << endl;
            cout << "Giá trị hàm mục tiêu: " << cplex.getObjValue() << endl;
            cout << "Số cuộn vải sử dụng (y): " << cplex.getValue(y) << endl;
            for (int i = 0; i < n; i++) {
                cout << "Số đoạn cắt ra của loại " << (i+1) << " (x[" << i << "]): " 
                     << cplex.getValue(x[i]) << endl;
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