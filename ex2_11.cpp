#include <ilcplex/ilocplex.h>
#include <vector>
#include <iostream>
#include <random>

using namespace std;

ILOSTLBEGIN

int main() {
    IloEnv env;
    try {
        const int markets = 20;
        const int stores = 500;

        // Tạo chi phí vận chuyển ngẫu nhiên
        vector<vector<int>> c(stores, vector<int>(markets));
        vector<int> d(stores);
        vector<int> f = {1, 2, 4, 5, 1, 2, 5, 2, 6, 1,
                         4, 2, 2, 0, 5, 12, 7, 8, 1, 3}; // Chi phí cố định

        srand(time(0)); 

        // Sinh dữ liệu ngẫu nhiên
        for (int i = 0; i < stores; ++i) {
            for (int j = 0; j < markets; ++j) {
                c[i][j] = rand() % 11; 
            }
            d[i] = rand() % 100 + 10; // Nhu cầu từ 10 đến 100
        }

        // Khởi tạo mô hình CPLEX
        IloModel model(env);

        // Biến quyết định
        IloArray<IloIntVarArray> k(env, markets);  // Số chai từ trung tâm i đến đối tác j
        IloArray<IloBoolVarArray> x(env, markets); // Biến nhị phân chỉ định có vận chuyển hay không

        for (int i = 0; i < markets; ++i) {
            k[i] = IloIntVarArray(env, stores, 0, 1000);
            x[i] = IloBoolVarArray(env, stores);
        }

        // Hàm mục tiêu
        IloExpr objective(env);
        for (int i = 0; i < markets; ++i) {
            for (int j = 0; j < stores; ++j) {
                objective += c[j][i] * k[i][j] + f[i] * x[i][j];
            }
        }
        model.add(IloMinimize(env, objective));
        objective.end();

        // Ràng buộc: Mỗi đối tác bán lẻ phải nhận đúng nhu cầu
        for (int j = 0; j < stores; ++j) {
            IloExpr quantity(env);
            for (int i = 0; i < markets; ++i) {
                quantity += k[i][j];
                model.add(k[i][j] <= d[j] * x[i][j]); // Ràng buộc liên kết
            }
            model.add(quantity == d[j]); // Tổng số lượng phải đáp ứng đúng nhu cầu
            quantity.end();
        }


        // Khởi tạo solver
        IloCplex cplex(model);
        cplex.setOut(env.getNullStream()); // Tắt output nếu không cần
        if (cplex.solve()) {
            cout << "Tìm thấy phương án khả thi, \n";
            cout << "Tổng giá: " << cplex.getObjValue() << endl;
        } else {
            cout << "Không tìm thấy phương án nào.\n";
        }

    } catch (IloException &e) {
        cerr << "IloException: " << e << endl;
    } catch (...) {
        cerr << "Unknown exception caught" << endl;
    }
    env.end();
    return 0;
}
