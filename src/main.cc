#include <cstdlib>

#include "app_app.h"

///
auto Main(int argc, char **argv) -> int {
  auto app = vh::ponc::App("PON Calculator", argc, argv);

  if (!app.Create()) {
    return EXIT_FAILURE;
  }

  return app.Run();
}

/*
#include <cmath>
#include <iomanip>
#include <iostream>

using namespace std;
#define ND 2
#define NS 2
#define N (ND + NS)
#define N1 (NS * (N + 1))

void init(float x[], int n) {
  int i;

  for (i = 0; i < n; i++) x[i] = 0;
}

int main() {
  int i, j, k, ki, kj, bas[NS];
  float a[NS][N + 1], c[N], cb[NS], th[NS];
  float x[ND], cj, z, t, b, min, max;

  // Initializing the arrays to zero

  init(c, N);
  init(cb, NS);
  init(th, NS);
  init(x, ND);

  for (i = 0; i < NS; i++) init(a[i], N + 1);

  // Now set coefficients for slack variables equal to one
  for (i = 0; i < NS; i++) a[i][i + ND] = 1.0;

  // Now put the stack variables in the basis
  for (i = 0; i < NS; i++) bas[i] = ND + i;

  // Now get the constraints and objective function
  cout << "Enter the constraints" << endl;
  for (i = 0; i < NS; i++) {
    for (j = 0; j < NS; j++) cin >> a[i][j];
    cin >> a[i][N];
  }

  cout << "Enter the objective function" << endl;
  for (j = 0; j < ND; j++) cin >> c[j];
  cout << fixed;

  // Now calculate cj and identify the incoming variables
  while (1) {
    max = 0;
    kj = 0;

    for (j = 0; j < N; j++) {
      z = 0;
      for (i = 0; i < NS; i++) z += cb[i] * a[i][j];
      cj = c[j] - z;
      if (cj > max) {
        max = cj;
        kj = j;
      }
    }

    // Apply the optimality test
    if (max <= 0) break;

    max = 0;

    for (i = 0; i < NS; i++)
      if (a[i][kj] != 0) {
        th[i] = a[i][N] / a[i][kj];
        if (th[i] > max) max = th[i];
      }

    // Now check for unbounded solution

    if (max <= 0) {
      cout << "Unbonded solution";
      return 1;
    }

    // Now search for the outgoing variables

    min = max;
    ki = 0;

    for (i = 0; i < NS; i++)
      if ((th[i] < min) && (th[i] != 0)) {
        min = th[i];
        ki = i;
      }

    // Now a[ki][kj] is the key of element
    t = a[ki][kj];

    // Divide the key row by key element
    for (j = 0; j < N + 1; j++) a[ki][j] /= t;

    // Make all the other elements of key column zero
    for (i = 0; i < NS; i++)
      if (i != ki) {
        b = a[i][kj];
        for (k = 0; k < N + 1; k++) a[i][k] -= a[ki][k] * b;
      }
    cb[ki] = c[kj];
    bas[ki] = kj;
  }

  // Now calculating the optimum value
  for (i = 0; i < NS; i++)
    if ((bas[i] >= 0) && (bas[i] < ND)) x[bas[i]] = a[i][N];

  z = 0;

  for (i = 0; i < ND; i++) z += c[i] * x[i];

  for (i = 0; i < ND; i++) {
    cout << "x[" << setw(3) << i + 1 << "]=";
    cout << setw(7) << setprecision(2) << x[i] << endl;
  }
  cout << "Optimal value=" << setw(7);

  cout << setprecision(2) << z << endl;
  return 0;
}
*/

/*
Enter the constraints
4 2 80
2 5 180
Enter the objective function
3 4
x[ 1]=x[ 2]= 3.00
Optimal value= 147.50
*/