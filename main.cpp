#include "carbonTracker.hpp"
#include <iostream>     
#include <cassert> 
#include "ct2.hpp"
#include <chrono>

using namespace std;

void test_tracking(){
    cout << "test_tracking" << endl;
    CT2 x(Hector::unitval(), "x");
    H_ASSERT(!x.isTracking(), "Tracker doesn't start off false");
    x.setTracking(true);
    H_ASSERT(x.isTracking(), "Turning on tracking doesn't work");
    x.setTracking(false);
    H_ASSERT(!x.isTracking(), "Turning off tracking doesn't work");
    
    x.setTracking(true);
    CT2 y(Hector::unitval(), "y");
    // TODO: test that this throws a 'tracking mismatch' error
}

void test_get_sources(){
    cout << "test_get_sources" << endl;
    Hector::unitval c1(1, Hector::U_PGC);
    CT2 x(c1, "x");
    // TODO: test that x throws an error if we try to get_sources() w/o tracking on
    x.setTracking(true);
    vector<string> x_sources = x.get_sources();
    H_ASSERT(x_sources.size() == 1, "wrong size");
    H_ASSERT(x_sources[0] == "x", "wrong source");
    
    CT2 y(c1, "y");
    y.setTracking(true);
    CT2 z = x + y;
    vector<string> z_sources = z.get_sources();
    H_ASSERT(z_sources.size() == 2, "wrong size");
    H_ASSERT(z_sources[0] == "x", "wrong source");
    H_ASSERT(z_sources[1] == "y", "wrong source");
}

void test_get_total(){
    cout << "test_get_total" << endl;
    Hector::unitval c1(1, Hector::U_PGC);
    CT2 x(c1, "x");
    Hector::unitval total = x.get_total();
    H_ASSERT(total == c1, "Total doesn't match");
}

void test_get_fraction(){
    cout << "test_get_fraction" << endl;
    Hector::unitval c1(1, Hector::U_PGC);
    CT2 x(c1, "x");
    // TODO: test that x throws an error if we try to get_fraction() w/o tracking on
    x.setTracking(true);
    double frac = x.get_fraction("x");
    H_ASSERT(frac == 1, "initial frac not 1");
}

void test_addition(){
    cout << "test_addition" << endl;
    Hector::unitval c1(1, Hector::U_PGC);
    CT2 x(c1, "x");
    CT2 y(c1, "y");
    
    // addtion without tracking
    CT2 z = x + y;
    Hector::unitval c2 = c1 + c1;
    H_ASSERT(z.get_total() == c2, "untracked total not right");
    
    // tracking mismatch
    x.setTracking(true);
    // TODO: addition throws tracking mismatch error
    x.setTracking(false);
    y.setTracking(true);
    // TODO: addition throws tracking mismatch error
    
    // addition with tracking
    x.setTracking(true);
    y.setTracking(true);
    z = x + y;
    H_ASSERT(z.get_total() == c2, "tracked total not right");
    H_ASSERT(z.get_fraction("x") == 0.5, "x fraction not right");
    H_ASSERT(z.get_fraction("y") == 0.5, "x fraction not right");
    
    z = z + x;
    H_ASSERT(z.get_total() == c1 + c2, "tracked total not right");
    double xfrac = z.get_fraction("x");
    double yfrac = z.get_fraction("y");
    H_ASSERT(xfrac == yfrac * 2, "x/y fractions not right");
    
    // add 0
    Hector::unitval c0(0, Hector::U_PGC);
    CT2 zero(c0, "zero");
    zero.setTracking(true);
    CT2 x2 = x + zero;
    H_ASSERT(x == x2, "adding zero doesn't work");
    H_ASSERT(x2.get_fraction("x") == 1, "x2 fraction not right");
    
    // add 0 and 0
    CT2 zerozero = zero + zero;
    H_ASSERT(zerozero == zero, "adding zeros doesn't work");
}

void test_subtraction(){
    cout << "test_subtraction" << endl;
    Hector::unitval c0(0, Hector::U_PGC);
    Hector::unitval c1(1, Hector::U_PGC);
    CT2 x(c1, "x");
    x.setTracking(true);
    CT2 y = x - x;
    H_ASSERT(y.get_total() == c0, "CT2 subtraction doesn't work");
    H_ASSERT(y.get_fraction("x") == x.get_fraction("x"), "fractions not preserved");
    
    // can also subtract a unitval
    CT2 y2 = x - c1;
    H_ASSERT(y.get_total() == y2.get_total(), "unitval subtraction doesn't work");
    CT2 x2 = x - c0;
    H_ASSERT(x.get_total() == x2.get_total(), "unitval 0 subtraction doesn't work");
}

void test_mult_div(){
    cout << "test_multiplication and division" << endl;
    Hector::unitval c0(0, Hector::U_PGC);
    Hector::unitval c1(1, Hector::U_PGC);
    Hector::unitval c2(2, Hector::U_PGC);
    CT2 x(c2, "x");
    x.setTracking(true);
    
    CT2 x2 = x * 2;
    H_ASSERT(x2.get_total() == x.get_total() * 2.0, "member multiplication doesn't work");
    H_ASSERT(x.get_fraction("x") == x2.get_fraction("x"), "mult fractions not preserved");
    CT2 x2a = 2 * x;
    H_ASSERT(x2.get_total() == x2a.get_total(), "nonmember mult doesn't work");
    x2 = x / 2.0;
    H_ASSERT(x2.get_total() == x.get_total() / 2.0, "division doesn't work");
    H_ASSERT(x.get_fraction("x") == x2.get_fraction("x"), "div fractions not preserved");
    CT2 x5a = x * 0.2;
    CT2 x5b = x / 5.0;
    H_ASSERT(x5a.get_total() == x5b.get_total(), "mult or division doesn't work");
    CT2 x1 = x * 1;
    H_ASSERT(x1.get_total() == x.get_total(), "multiplication identity doesn't work");
    x1 = x / 1;
    H_ASSERT(x1.get_total() == x.get_total(), "division identity doesn't work");
    CT2 x0 = x * 0;
    H_ASSERT(x0.get_total() == c0, "multiplication by zero doesn't work");
    x0 = x / 0;
    H_ASSERT(isinf(x0.get_total()), "division by zero doesn't work");
    
}

void test_equality(){
    cout << "test_equality" << endl;
    Hector::unitval c1(1, Hector::U_PGC);
    CT2 x(c1, "x");
    CT2 y(c1, "x");
    
    H_ASSERT(x == y, "equality doesn't work");
    H_ASSERT(!(x != y), "inequality doesn't work");
    y = x * 2;
    H_ASSERT(x != y, "equality doesn't work");
    H_ASSERT(!(x == y), "inequality doesn't work");
}

void test_identicality(){
    cout << "test_identicality" << endl;
    Hector::unitval c1(1, Hector::U_PGC);
    CT2 x(c1, "x");
    CT2 x1(c1, "x");
    CT2 y(c1, "y");
    
    H_ASSERT(x.identical(x1), "identical doesn't work");
    
    x.setTracking(true);
    H_ASSERT(!x.identical(x1), "identical tracking doesn't work");
    H_ASSERT(!x1.identical(x), "identical tracking doesn't work");
    x1.setTracking(true);
    H_ASSERT(x.identical(x1), "identical tracking doesn't work");
    x.setTracking(false);
    H_ASSERT(!x.identical(x1), "identical tracking doesn't work");
    H_ASSERT(!x1.identical(x), "identical tracking doesn't work");
    x.setTracking(true);
    
    // different sources
    y.setTracking(true);
    H_ASSERT(!x.identical(y), "identical sources doesn't work");
    H_ASSERT(!y.identical(x), "identical sources doesn't work");
    
    // different fraction
    Hector::unitval c0(0, Hector::U_PGC);
    CT2 x0(c0, "x");
    x0.setTracking(true);
    H_ASSERT(!x.identical(x0), "identical fractions doesn't work");
    H_ASSERT(!x0.identical(x), "identical fractions doesn't work");
}

int main(int argc, char* argv[]){
    
    Hector::unitval carbon10(10, Hector::U_PGC);
    CT2 x(carbon10, "x");
    x.setTracking(true);
    Hector::unitval carbon20(20, Hector::U_PGC);
    CT2 y(carbon20, "y");
    y.setTracking(true);
    CT2 z = x + y;
    
    cout << "x is " << x << endl;
    cout << "y is " << y << endl;
    cout << "z is " << z << endl;
    z = z - carbon10;
    cout << "z=z-10 is " << z << endl;
    CT2 z2 = z / 2;
    cout << "z/2 is " << z2 << endl;
    CT2 zpoint5 = z2 * 0.5;
    cout << "z/2*0.5 is " << zpoint5 << endl;
    const CT2 test = z;
    CT2 twoxz = 2.0 * test;
    cout << "2 * z is " << twoxz << endl;
    
    cout << "\n---------- SIMULATION ----------\n" << endl;
    CT2 dest(carbon10, "dest");
    dest.setTracking(true);
    cout << "z = " << z << endl;
    cout << "dest = " << dest << endl;
    for(int i = 0; i < 5; i++) {
        cout << "----------------- i = " << i << endl;
        cout << "z = " << z << endl;
        CT2 flux = z * 0.1;
        z = z - flux;
        dest = dest + flux;
        cout << "flux from z to dest = " << flux << endl;
        cout << "z = " << z << endl;
        cout << "dest = " << dest << endl;
    }
    
    cout << "\n---------- TIMING ----------\n" << endl;
    
    for(int tracking = 0; tracking <= 1; tracking++) {
        x = CT2(carbon10, "x");
        x.setTracking(tracking);
        y = CT2(carbon10, "y");
        x.setTracking(tracking);
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0; i < 1000; i++) {
            CT2 flux = x * 0.01;
            x = x - flux;
            y = y + flux;
        }
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        cout << "tracking = " << tracking << " time = " << duration.count() << endl;
    }
    
    cout << "\n---------- Time for Tests! ----------\n" << endl;
    test_tracking();
    test_get_sources();
    test_get_total();
    test_get_fraction();
    test_addition();
    test_subtraction();
    test_mult_div();
    test_equality();
    test_identicality();
}
