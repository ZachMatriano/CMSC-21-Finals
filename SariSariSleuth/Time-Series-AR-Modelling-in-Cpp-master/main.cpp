/*

 ©EsterHlav

 Autoregressive Modelling for Time Series in C++

 Created by EsterHlav on 8/8/18.
 Copyright © 2018 EsterHlav. All rights reserved.

 */

#include "ARmodel.h"
using namespace std;

int main() {

    ///////////////////////////////
    //  Time Series AR modelling //
    ///////////////////////////////

    testData();

    testMatrix();

    testAR2();
    testAR5();

    // Generate test data
    double phis[] = {0.5, -0.3}; // Example AR(2) coefficients
    double initial_x[] = {1.0, 2.0}; // Initial values
    int order = 2;
    int N = 100; // Number of data points
    double sigma = 0.1; // Standard deviation of noise

    double* test_data = generate(phis, initial_x, order, N, sigma);
    Data data(test_data, N, "Test Data");

    // Create AR model and fit
    ARmodel model(data);
    model.fit(order);

    // Print results
    model.print();

    // Clean up
    delete[] test_data;

    return 0;
}
