#include <ilcplex/ilocplex.h>
#include <vector>
#include <iostream>

using namespace std;

ILOSTLBEGIN

int main() {
    IloEnv env;
    try {
        vector<int> d = {200, 150, 60, 20, 240}; // Số lượng sinh viên mỗi tòa.
        vector<int> f = {2, 10, 5, 3, 1, 2}; // chi phí bảo chì bãi đỗ xe.
        vector<int> s = {100, 250, 300, 220, 450, 230}; // Sức chứa mỗi bãi đổ xe
        const int m = s.size();
        const int n = d.size();
        vector<vector<int> > a = {{3, 5, 2, 1, 1}, // Khoảng cách của bãi đỗ xe i đến tòa j.
                                {2, 3, 2, 1, 4},
                                {5, 5, 2, 7, 1},
                                {3, 2, 2, 1, 4},
                                {4, 2, 1, 7, 1},
                                {1, 5, 2, 2, 1}};

        IloModel model(env);

        //Các biến quyết định
         IloBoolVarArray x(env, m); // Biến nhị phân xem có chọn bãi xây bãi đỗ đó không.
         IloArray<IloIntVarArray> y(env, m); // số sinh viên từ bãi đỗ i đến j (sẽ = 0 nếu bãi đỗ đó không được chọn).
         for (int i = 0; i < m; ++i)
         {
             y[i] = IloIntVarArray(env, n, 0, 10000);
         }

         IloExpr objective(env); // Hàm mục tiêu.
         for (int i = 0; i < m; ++i)
         {
            objective += f[i] * x[i]; // Tổng chi phí bảo chì.
            for (int j = 0; j < n; ++j)
            {
                objective += a[i][j] * y[i][j];
            }
         }
        model.add(IloMinimize(env, objective));
        objective.end();

         //Ràng buộc
        for (int j = 0; j < n; ++j)
        {
            IloExpr students(env);
            for (int i = 0; i < m; ++i)
            {
                students += y[i][j];
            }
            model.add(students == d[j]);
            students.end();
        }


        for (int i = 0; i < m; ++i)
        {
            IloExpr capacity(env);
            for (int j = 0; j < n; ++j)
            {
                capacity += y[i][j];
            }
            model.add(s[i] * x[i] >= capacity);
            capacity.end(); 
        }

        // Giải bài toán
        IloCplex cplex(model);
        cplex.setOut(env.getNullStream()); 
        if (!cplex.solve()) {
            cout << "Solver status: " << cplex.getStatus() << endl;

            cerr << "Không thể tìm thấy lời giải tối ưu." << endl;
        } else {
            cout << "Chi phí tối thiểu: " << cplex.getObjValue() << endl;
            for (int i = 0; i < m; i++) {
                if (cplex.getValue(x[i]) > 0.5) { 
                    cout << "Bãi đỗ xe " << i + 1 << " được xây." << endl;
                    for (int j = 0; j < n; j++) {
                        int students = cplex.getValue(y[i][j]); // Đọc giá trị nguyên
                        if (students > 0) {
                            cout << "  - Phục vụ " << students << " sinh viên đến tòa " << j + 1 << endl;
                        }
                    }
                }
            }
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