#ifndef CARBONTRACKER_HPP
#define CARBONTRACKER_HPP
#include <sstream>
#include <unordered_map> 
#include "unitval.hpp"

using namespace std;

  /**
   * \brief CarbonTracker Class: class to track origin of carbon in various carbon pools as it moves throughout the carbon cycle
   * in simple climate model Hector
   * 
   * Designed so that it can be dropped in place of a unitval in the Hector C++ code base
   */
  class CarbonTracker{
   public:

    // All of the pools of carbon within Hector - by default they are asigned increasing, consecuative integer values with 
    // SOIL = 0 as the starting point - DO NOT CHANGE DEFAULT VALUES
    // ALSO LEAVE 'LAST' as the last value!
    enum Pool {
      SOIL, ATMOSPHERE, DEEPOCEAN, TOPOCEAN, LAST
    };



   private:

    // Total amount of carbon in a pool represented by a CarbonTracker object - in petagrams carbon (U_PGC)
    Hector::unitval totalCarbon;

    // array containing each of the sub-pools within Hector
    // indicies correspond to indices of array within 
    double originFracs[LAST];

    // boolean to signify if tracker should be tracking carbon movement
    static bool track;

    /**
      *\brief parameterized constructor - useful for initializing fluxes with predetermined maps -
              ONLY FOR USE WITHIN CPP NOT FOR GENERAL USE TO AVOID INITIALIZATION ISSUES
      *\param totC unitval (units pg C) that expresses total amount of carbon in the pool
      *\param pool_map pointer to a double array - usually the originFracs array of the pool the flux is leaving
      */
    CarbonTracker(Hector::unitval totC, double* pool_map);

   public:

    /**
      *\brief parameterized constructor - initialize pools of carbon with pg carbon (unitvals)
      *\param totC unitval (units pg C) that expresses total amount of carbon in the pool
      *\param subPool Pool object to set as inital origin of carbon in the pool at time of creation
      */
    CarbonTracker(Hector::unitval totC, Pool subPool);

    // ~CarbonTracker(); DO I NEED THIS??


    /**
      * \brief copy constructor
      * \param ct reference to carbon tracker to be copied
      */
    CarbonTracker(const CarbonTracker &ct);

    /**
      * \brief assignment operator
      * \param ct carbon tracker to be copied
      * \returns sets "this" instance variables to be the same as ct's
      */ 
    CarbonTracker& operator=(CarbonTracker ct);


    /**
      * \brief addition between two carbon tracker objects (one pool and one flux)
      *         - if tracking, then total carbon is the sum of the two total carbons and the map of the new CarbonTracker 
      *           object will reflect percentages of added pools
      *         - else only the total carbon's will be added and the pool's array will be used (flux must be created using 
      *           fluxFromTrackerPool and if not tracking array will be all 0s)
      * \param flux carbon tracker object that is being added to 'this', needs total carbon unitval (unit pg C)
      * \returns CarbonTracker object with updated total carbon and map
      */ 
    CarbonTracker operator+(const CarbonTracker& flux);


    /**
      * \brief subtraction between two CarbonTracker objects - if tracking, total carbon will be reduced and the map
      *        will be updated to new proportions - else only total carbon is updated
      * \param flux carbon tracker object that is being subtracted from 'this', needs total carbon unitval (unit pg C) and valid map
      * \return CarbonTracker object with decreased total carbon and upated map
      */ 
    CarbonTracker operator-(const CarbonTracker& flux);

    /**
      * \brief subtraction between CarbonTracker object and a unitval - decreases total carbon and leaves map the same -
      *  new carbon is removed evenly from current sub-pools
      * \param flux unitval with units pg C
      * \return CarbonTracker object with decreased total carbon and unchanged map from 'this'
      */ 
    CarbonTracker operator-(const Hector::unitval flux);

    /**
      * \brief setter for total carbon within CarbonTracker object
      * \param totalCarbon unitval with units (pg C)
      */ 
    void setTotalCarbon(Hector::unitval totalCarbon);

    // /**
    //   * \brief setter for map object within CarbonTracker object
    //   * \param origin_frac map with MultiKeys key and double values
    //   */ 
    // void setOriginFracs(double* origin_frac);

    /**
      * \brief getter for CarbonTracker total carbon
      * \return unitval with units (pg C)
      */ 
    Hector::unitval getTotalCarbon();

    /**
      * \brief getter for entire CarbonTracker map
      * \return returns map object with MultiKey keys and double values
      */ 
    double* getOriginFracs();

    /**
      * \brief getter for indiviudal key-value pairs within a CarbonTracker map object
      * \param origin Origin pool
      * \return value associated with key param, if no key matches returns 0
      */ 
    Hector::unitval getPoolCarbon(Pool origin);

     /**
      * \brief getter for static boolean track that shows if the carbon pools are tracking yet
      * \return boolean value of track object
      */ 
    static bool isTracking();

    /**
      * \brief starts tracking and makes CarbonTracker param track = true
      */ 
    static void startTracking();

      /**
      * \brief stops tracking and makes CarbonTracker param track = false
      */ 
    static void stopTracking();

    

   /**
    * \brief makes a flux of 'flux' carbon from a CarbonTracker object so that it can be added to a sub-pool of a CarbonTracker object
    * \param flux unitval with units (pg C)
    * \return CarbonTracker object with total carbon set to flux and a map that is the same as the pool the carbon is coming from
    */ 
  CarbonTracker fluxFromTrackerPool(const Hector::unitval flux);

   /**
    * \brief makes a flux of 'flux' carbon using the input array so that it can be added or subtracted from a sub-pool of a 
    *        CarbonTracker object - usful for removing more of specific sub-pools than others (i.e. isotopes)
    * \param fluxAmount unitval with units (pg C)
    * \param fluxProportions double array that hold proportions that you want to add/remove subpools of carbon in
    * \return CarbonTracker object with total carbon set to flux and a map that is the same fluxProportions
    */ 
  CarbonTracker fluxFromTrackerPool(const Hector::unitval fluxAmount, double* fluxProportions);
  
   /**
    * \brief Prints the total amount of carbon within each subpool 
    * \param out output stream 
    * \param ct carbon tracker object that will be printed
    * \return CarbonTracker object with total carbon set to flux and a map that is the same fluxProportions
    */ 
  friend ostream& operator<<(ostream &out, CarbonTracker &ct);
  };


  //MIGHT WANT TO ADD ONE WHERE YOU CAN SET THE AMOUNT YOU TAKE FROM EACH?? INSTEAD OF IT COMING FROM EXACT SAME ARRAY AS ORIGIN

  /**
    * \brief multiplication between double and CarbonTracker object - usually used to get fraction of a pool
    * \param d double (usually a fractional value to get sub-section of pool)
    * \param ct CarbonTracker object with unitval (pg C) total carbon and valid map
    * \return CarbonTracker object with d*(ct totalCarbon) and unchanged map 
    */ 
  CarbonTracker operator*(const double d, CarbonTracker& ct);

  /**
    * \brief multiplication between CarbonTracker object and double - usually used 
    * opposite order of paramters from above
    * \param d double (usually a fractional value to get sub-section of pool)
    * \param ct CarbonTracker object with unitval (pg C) total carbon and valid map
    * \return CarbonTracker object with d*(ct totalCarbon) and unchanged map 
    */ 
  CarbonTracker operator*(const CarbonTracker& ct, double d);

  /**
    * \brief divison of a CarbonTracker object by a double - usually used to get fraction of a pool
    * opposite order of paramters from above - implemnted to make sure unitval operations are still allowed
    * \param d double (usually a fractional value to get sub-section of pool)
    * \param ct CarbonTracker object with unitval (pg C) total carbon and valid map
    * \return CarbonTracker object with (ct totalCarbon)/d and unchanged map 
    */ 
  CarbonTracker operator/(CarbonTracker& ct, const double d);

#endif
