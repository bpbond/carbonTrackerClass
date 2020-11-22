
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
    
    double frac = 0.0;
    for (auto itr = ctmap.begin(); itr != ctmap.end(); itr++) {
        frac += itr->second;
    }
    H_ASSERT(frac - 1.0 < 1e-6, "pool_map must sum to ~1.0")
}

bool CT2::isTracking() const {
    return track;
}

void CT2::setTracking(bool do_track){
    track = do_track;
}

// Accessor: return a string vector of the current sources
std::vector<std::string> CT2::get_sources() const {
    H_ASSERT(track, "get_sources() requires tracking to be on");
    std::vector<std::string> sources;
    for (auto itr = ctmap.begin(); itr != ctmap.end(); itr++) {
        sources.push_back(itr->first);
    }
    return sources;
}

// Accessor: return total amount
Hector::unitval CT2::get_total() const {
    return total;
}

// Accessor: return the fraction corresponding to a specific source
double CT2::get_fraction(string source) const {
    H_ASSERT(track, "get_fraction() requires tracking to be on");
    double val = 0.0;  // 0.0 is returned if not in our map
    auto x = ctmap.find(source);
    if(x != ctmap.end()) {
        val = x->second;
    }
    return val;
}

// Addition: the complicated one
CT2 CT2::operator+(const CT2& flux){
    Hector::unitval new_total = total + flux.total;
    unordered_map<string, double> new_origins;
    
    if(track) {
        H_ASSERT(flux.isTracking(), "tracking mismatch")
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
            if(new_total) {
                new_origins[itr->first] = itr->second / new_total;
            } else {  // uh oh, new total is zero
                new_origins[itr->first] = 1 / new_pools.size();
            }
        }
    } else {
        H_ASSERT(!track, "tracking mismatch")
    }
    
    CT2 addedFlux(new_total, new_origins, track);
    return addedFlux;
}

// Because we track a total and source fractions, subtraction is trivial
CT2 CT2::operator-(const Hector::unitval flux){
    CT2 sub_ct(total - flux, ctmap, track);
    return sub_ct;
}
// We also allow subtraction of a CT2 object (ignoring tracking info of rhs object)
CT2 CT2::operator-(const CT2& ct){
    CT2 sub_ct(total - ct.get_total(), ctmap, track);
    return sub_ct;
}

// Multiplication member function (when this object is lhs, and double rhs)
CT2 CT2::operator*(const double d){
    CT2 ct(total * d, ctmap, track);
    return ct;
}
// ...nonmember function for when object is rhs; just flip and call member function
CT2 operator*(double d, const CT2& ct){
    CT2 x = ct; // need to make non-const
    return x * d;
}

// Division
CT2 CT2::operator/(const double d){
    CT2 ct(total / d, ctmap, track);
    return ct;
}

// Equality: same total, same sources, same fractions
bool CT2::operator==(const CT2& rhs){
    bool same = total == rhs.get_total();
    
    std::vector<std::string> sources = rhs.get_sources();
    same = same || get_sources() == sources;
    
    for (int i = 0; i < sources.size(); i++) {
        same = same || ctmap.at(sources[i]) == rhs.get_fraction(sources[i]);
    }

    return same;
}

// Printing
ostream& operator<<(ostream &out, CT2 &ct ){
    out << ct.total << endl;
    if(ct.isTracking()) {
        std::vector<std::string> sources = ct.get_sources();
        for (int i = 0; i < sources.size(); i++) {
            out << "\t" << sources[i] << ": " << ct.get_fraction(sources[i]) << endl;
        }
    }
    return out;
}
