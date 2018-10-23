#ifndef BOOST_MUTEX_H_
#define BOOST_MUTEX_H_

#include <boost/interprocess/sync/interprocess_sharable_mutex.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

//------------
// BoostMutex
//------------
struct BoostMutex
{
    //--------------
    // type defines
    //--------------

    // mutex type
    typedef boost::interprocess::interprocess_sharable_mutex    MutexType;

    // lock type
    typedef boost::interprocess::sharable_lock<MutexType>       ReadLockType;
    typedef boost::interprocess::scoped_lock<MutexType>         WriteLockType;
};

//---------------------
// BoostReadWriteMutex
//---------------------
class BoostReadWriteMutex
{
public:

    // constrcutor
    BoostReadWriteMutex(): mutex_() {}

    // read lock
    BoostMutex::ReadLockType ReadLock() { return BoostMutex::ReadLockType(mutex_); }

    // write lock
    BoostMutex::WriteLockType WriteLock() { return BoostMutex::WriteLockType(mutex_); }

private:

    // the read-write mutex
    BoostMutex::MutexType  mutex_;
};

#endif // BOOST_MUTEX_H_