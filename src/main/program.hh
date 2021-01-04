#ifndef PROGRAM_HH
#define PROGRAM_HH

#include <string>

#include <objscip/objscip.h>
#include <scip/scip.h>

#include "solution_stats.hh"

class LPObserver;

class Program
{
public:
  struct Settings
  {
    bool solverOutput;
    bool collect;
    std::string setFile;
    bool solveRelaxation;

    Settings()
      : solverOutput(true),
        collect(false),
        setFile(""),
        solveRelaxation(false)
    {}

    Settings& withSetFile(const std::string& file)
    {
      setFile = file;
      return *this;
    }

    Settings& doSolveRelaxation(bool value = true)
    {
      solveRelaxation = value;
      return *this;
    }

    Settings& withSolverOutput(bool output = true)
    {
      solverOutput = output;
      return *this;
    }

    Settings& collectStats(bool doCollect = true)
    {
      collect = doCollect;
      return *this;
    }

  };

private:
  const Settings settings;
  LPObserver* observer;

protected:
  SCIP* scip;

public:
  Program(const std::string& name,
          const Settings& settings);

  Program(const Program&) = delete;

  Program& operator=(const Program&) = delete;

  SCIP* getSCIP() const
  {
    return scip;
  }

  SolutionStats getStats() const;

  void addEventHandler(scip::ObjEventhdlr* eventHandler);

  virtual ~Program();

  void write(const std::string& name);

  void writeOriginal(const std::string& name);

  void writeTransformed(const std::string& name);

  double lowerBound() const
  {
    return SCIPgetLowerbound(scip);
  }

  double upperBound() const
  {
    return SCIPgetCutoffbound(scip);
  }

  const Settings getSettings() const
  {
    return settings;
  }

};

#endif /* PROGRAM_HH */
