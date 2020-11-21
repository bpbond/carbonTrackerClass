
#include <sstream>
#include <unordered_map>
#include "ct2.hpp"
#include "unitval.hpp"

using namespace std;

// Public constructor
CT2::CT2(Hector::unitval total, string pool){
    track = false;
    this->total = total;
    ctmap[pool] = 1;
}

// Private constructor with explicit source pool map
CT2::CT2(Hector::unitval total, unordered_map<string, double> pool_map, bool do_track){
    track = do_track;
    this->total = total;
    ctmap = pool_map;
}

bool CT2::isTracking() const {
    return track;
}

void CT2::setTracking(bool do_track){
    track = do_track;
}


std::vector<std::string> CT2::get_sources() const {
    std::vector<std::string> sources;
    for (auto itr = ctmap.begin(); itr != ctmap.end(); itr++) {
        sources.push_back(itr->first);
    }
    return sources;
}

double CT2::get_fraction(string source) const {
    double val = 0.0;  // 0.0 is returned if not in our map
    auto x = ctmap.find(source);
    if(x != ctmap.end()) {
        return x->second;
    }
    return val;
}

CT2 CT2::operator+(const CT2& flux){
    Hector::unitval totC = total + flux.total;
    unordered_map<string, double> new_origins;
    
    if(track) {
        unordered_map<string, Hector::unitval> new_pools;

        // Look through *our* sources, and if any in other object, add
        for (auto itr = ctmap.begin(); itr != ctmap.end(); itr++) {
            new_pools[itr->first] = total * itr->second + flux.total * flux.get_fraction(itr->first);
        }
        
        // Look through the *other* object sources, and if any NOT in our map, add
        std::vector<std::string> sources = flux.get_sources();
        for (int i = 0; i < sources.size(); i++) {
            const string src = sources[i];
            if(ctmap.find(src) == ctmap.end()) {  // source that's not in our map
                new_pools[src] = flux.total * flux.get_fraction(src);
            }
        }
        
        // Now that we have the new pool values, compute new fractions
        for (auto itr = new_pools.begin(); itr != new_pools.end(); itr++) {
            new_origins[itr->first] = itr->second / totC;
        }
    } // if(track)
    
    CT2 addedFlux(totC, new_origins, track);
    return addedFlux;
}

// Because we track a total and source fractions, subtraction is trivial
CT2 CT2::operator-(const Hector::unitval flux){
    CT2 ct(total - flux, ctmap, track);
    return ct;
 }

// member function (this object on left of operator, double on right)
CT2 CT2::operator*(const double d){
    CT2 ct(total * d, ctmap, track);
    return ct;
}
// when object is on right, just flip and call member function
CT2 operator*(double d, const CT2& ct){
    CT2 x = ct; // need to make non-const
    return x * d;
}

// division
CT2 CT2::operator/(const double d){
    CT2 ct(total / d, ctmap, track);
    return ct;
}


// printing
ostream& operator<<(ostream &out, CT2 &ct ){
    out << ct.total << endl;
    std::vector<std::string> sources = ct.get_sources();
    for (int i = 0; i < sources.size(); i++) {
        out << "\t" << sources[i] << ": " << ct.get_fraction(sources[i]) << endl;
    }
    return out;
}
