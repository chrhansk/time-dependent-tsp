#include "program.hh"

#include <stdio.h>

#include <scip/scipdefplugins.h>

#include "scip_utils.hh"

#include "lp_observer.hh"

static SCIP_DECL_MESSAGEWARNING(printMsg)
{
  // redirect stdout to stderr
  if(file == stdout)
  {
    fputs(msg, stderr);
  }
  else
  {
    fputs(msg, file);
  }

  // std::cerr << msg;
}

Program::Program(const std::string& name,
                 const Program::Settings& settings)
  : settings(settings),
    observer(nullptr)
{
  SCIP_CALL_EXC(SCIPcreate(&scip));
  SCIP_CALL_EXC(SCIPincludeDefaultPlugins(scip));

  {
    std::string fname = BASE_DIRECTORY;
    fname += "/tdtsp.set";

    SCIP_CALL_EXC(SCIPreadParams(scip, fname.c_str()));
  }

  if(settings.setFile != "")
  {
    SCIP_CALL_EXC(SCIPreadParams(scip, settings.setFile.c_str()));
  }


  SCIP_CALL_EXC(SCIPcreateProbBasic(scip, name.c_str()));

  SCIP_CALL_EXC(SCIPsetObjsense(scip, SCIP_OBJSENSE_MINIMIZE));

  if(settings.collect)
  {
    observer = new LPObserver(scip);

    addEventHandler(observer);
  }

  if(settings.solverOutput)
  {
    SCIP_MESSAGEHDLR* handler;
    SCIP_CALL_EXC(SCIPmessagehdlrCreate(&handler,
                                        FALSE,
                                        NULL,
                                        FALSE,
                                        printMsg,
                                        printMsg,
                                        printMsg,
                                        NULL,
                                        NULL));

    SCIP_CALL_EXC(SCIPsetMessagehdlr(scip, handler));

    SCIPmessagehdlrRelease(&handler);
  }
  else
  {
    SCIP_CALL_EXC(SCIPsetIntParam(scip,
                                  "display/verblevel",
                                  SCIP_VERBLEVEL_NONE));
  }
}

void Program::addEventHandler(scip::ObjEventhdlr* eventHandler)
{
  SCIP_CALL_EXC(SCIPincludeObjEventhdlr(scip,
                                        eventHandler,
                                        TRUE));
}

Program::~Program()
{
}

SolutionStats Program::getStats() const
{
  if(!observer)
  {
    return SolutionStats::empty();
  }

  return SolutionStats(scip, observer->getStats());
}

void Program::write(const std::string& name)
{
  writeOriginal("orig_" + name);
  writeTransformed("trans_" + name);
}

void Program::writeOriginal(const std::string& name)
{
  SCIP_CALL_EXC(SCIPwriteOrigProblem(scip, name.c_str(), nullptr, false));
}

void Program::writeTransformed(const std::string& name)
{
  SCIP_CALL_EXC(SCIPwriteTransProblem(scip, name.c_str(), nullptr, false));
}
