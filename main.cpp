#include "carbonTracker.hpp"
#include <iostream>     
#include <cassert> 
#include "ct2.hpp"

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
    
    cout << "\n*** SIMULATION ***\n" << endl;
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
    
    cout << "Time for Tests!" << endl;
    test_tracking();
    test_get_sources();
    test_get_total();
    test_get_fraction();
    test_addition();
}
