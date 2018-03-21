#ifndef BOOST_LOCKER_SDFS23DSF_H_
#define BOOST_LOCKER_SDFS23DSF_H_ 

#include <boost/thread.hpp>  
#include <boost/thread/recursive_mutex.hpp>  
#include <boost/thread/mutex.hpp>
 
typedef boost::shared_mutex            WRMutex;  
typedef boost::unique_lock<WRMutex>    WriteLock;  
typedef boost::shared_lock<WRMutex>    ReadLock;  

/*
WRMutex  socket_wr_mutex_;  
*/

#endif // BOOST_LOCKER_SDFS23DSF_H_