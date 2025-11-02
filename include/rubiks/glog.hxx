#ifndef GLOG_HXX
#define GLOG_HXX
#include <ostream>

class GlobalLog : public std::ostream {
public:
  GlobalLog();

protected:
  using std::ostream::ostream;
};

extern GlobalLog glog;

#endif // GLOG_HXX
