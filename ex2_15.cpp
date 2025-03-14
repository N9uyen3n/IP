#include <ilcplex/ilocplex.h>
#include <vector>
#include <iostream>
#include <random>

using namespace std;

ILOSTLBEGIN

int main() {
    IloEnv env;
    try {
        const int t = 7;
        vector<int> d = {200, 150, 240, 230, 450, 200, 410}; // Nhu cầu của 7 giai đoạn.
        vector<double> c = {1.2, 3.0, 2.1, 1.1, 1.4, 1.0, 2.1}; // Chi phí sản xuất.
        vector<double> h = {1.2, 3.0, 2.1, 1.1, 1.4, 1.0, 2.1}; // Chi phí lưu kho.


        // Khởi tạo mô hình CPLEX
        IloModel model(env);

        IloIntVarArray x(env, t, 0, 10000); // Số lượng sản phẩm cần phải sản xuất trong giai đoạn t;
        IloIntVarArray a(env, t, 0, 10000); //Số lượng sản phẩm

        model.add(a[0] = x[0]);

        for (int i = 1; i < tt; ++i)
        {
            a[i] = a[i - 1] + x[i] - d[i- 1];
        }

    } catch (IloException &e) {
        cerr << "IloException: " << e << endl;
    } catch (...) {
        cerr << "Unknown exception caught" << endl;
    }
    env.end();
    return 0;
}
