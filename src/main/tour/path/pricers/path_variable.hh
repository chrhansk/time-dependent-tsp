#ifndef PATH_VARIABLE_HH
#define PATH_VARIABLE_HH

#include "scip_utils.hh"
#include "timed/timed_path.hh"

class PathVariable
{
private:
  TimedPath timedPath;
  SCIP_VAR* var;

public:
  PathVariable(const TimedPath& timedPath, SCIP_VAR* var)
    : timedPath(timedPath),
      var(var)
  {}

  const TimedPath& getTimedPath() const
  {
    return timedPath;
  }

  SCIP_VAR* getVariable() const
  {
    return var;
  }

  void setVariable(SCIP_VAR* variable)
  {
    var = variable;
  }

};

#endif /* PATH_VARIABLE_HH */
