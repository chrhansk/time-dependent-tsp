#include "odd_path_free_separator.hh"

#include <unordered_set>
#include <sstream>

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include "log.hh"
#include "scip_utils.hh"

#include "tuples.hh"

static double cutoff = 1e-5;

namespace
{
  struct CandidateSet
  {
    double weight;
    std::vector<Vertex> vertices;

    bool operator<(const CandidateSet& other) const
    {
      return weight < other.weight;
    }

    operator bool() const
    {
      return !vertices.empty();
    }

    bool operator==(const CandidateSet& other) const
    {
      return vertices == other.vertices;
    }

    static CandidateSet empty()
    {
      return CandidateSet{0, std::vector<Vertex>{}};
    }
  };

  class SeparationProblem
  {
  private:
    const TimeExpandedGraph& graph;
    std::vector<Vertex> originalVertices;
    const EdgeFunc<double>& weights;
    SCIP* subscip;
    EdgeVariables subVariables;

    void createVariables();
    void createConstraints();
    void createConstraints(const TimedVertex& vertex);

    void createConstraint(const TimedEdge& first, const TimedEdge& second);

    bool isSubVertex(const TimedVertex& vertex) const;

    std::vector<TimedEdge> collectEdges(SCIP_SOL* solution);

  public:
    SeparationProblem(const TimeExpandedGraph& graph,
                      const std::vector<Vertex>& originalVertices,
                      const EdgeFunc<double>& weights);

    ~SeparationProblem();

    std::vector<TimedEdge> solve();
  };

  SeparationProblem::SeparationProblem(const TimeExpandedGraph& graph,
                                       const std::vector<Vertex>& originalVertices,
                                       const EdgeFunc<double>& weights)
    : graph(graph),
      originalVertices(originalVertices),
      weights(weights),
      subscip(nullptr),
      subVariables(graph, nullptr)
  {
    SCIP_CALL_EXC(SCIPcreate(&subscip));
    SCIP_CALL_EXC(SCIPincludeDefaultPlugins(subscip));

    SCIP_CALL_EXC(SCIPcreateProbBasic(subscip, "odd_path_free_separation"));

    SCIP_CALL_EXC(SCIPsetObjsense(subscip, SCIP_OBJSENSE_MAXIMIZE));

    SCIP_CALL_EXC(SCIPsetIntParam(subscip,
                                  "display/verblevel",
                                  SCIP_VERBLEVEL_NONE));

    createVariables();
    createConstraints();
  }


  void SeparationProblem::createVariables()
  {
    for(const Vertex& originalSource : originalVertices)
    {
      for(const TimedVertex& source : graph.getExpandedVertices(originalSource))
      {
        assert(isSubVertex(source));

        for(const TimedEdge& outgoing : graph.getOutgoing(source))
        {
          assert(!subVariables(outgoing));

          TimedVertex target = outgoing.getTarget();

          if(!isSubVertex(target))
          {
            continue;
          }

          if(weights(outgoing) <= cutoff)
          {
            continue;
          }

          std::ostringstream namebuf;

          SCIP_VAR* var = nullptr;

          namebuf << "y_" << graph.underlyingVertex(source).getIndex()
                  << "_" << graph.underlyingVertex(target).getIndex()
                  << "#" << source.getTime();

          SCIP_CALL_EXC(SCIPcreateVar(subscip,
                                      &var,
                                      namebuf.str().c_str(),
                                      0.0, 1.0,
                                      weights(outgoing),
                                      SCIP_VARTYPE_BINARY,
                                      TRUE,
                                      FALSE,
                                      NULL, NULL, NULL, NULL, NULL));

          SCIP_CALL_EXC(SCIPaddVar(subscip, var));

          subVariables(outgoing) = var;
        }
      }
    }
  }

  void SeparationProblem::createConstraints()
  {
    for(const Vertex& originalVertex : originalVertices)
    {
      for(const TimedVertex& vertex : graph.getExpandedVertices(originalVertex))
      {
        createConstraints(vertex);
      }
    }
  }

  void SeparationProblem::createConstraints(const TimedVertex& vertex)
  {
    for(const TimedEdge& incoming : graph.getIncoming(vertex))
    {
      if(!subVariables(incoming))
      {
        continue;
      }

      for(const TimedEdge& outgoing : graph.getOutgoing(vertex))
      {
        if(!subVariables(outgoing))
        {
          continue;
        }

        if(graph.underlyingVertex(incoming.getSource()) ==
           graph.underlyingVertex(outgoing.getTarget()))
        {
          continue;
        }

        createConstraint(incoming, outgoing);
      }
    }
  }

  void SeparationProblem::createConstraint(const TimedEdge& first,
                                           const TimedEdge& second)
  {
    assert(first.getTarget() == second.getSource());
    assert(subVariables(first));
    assert(subVariables(second));


    std::ostringstream namebuf;
    TimedVertex vertex = first.getTarget();

    namebuf.str("");
    namebuf << "conflict_" << graph.underlyingVertex(vertex)
            << "#" << vertex.getTime();

    SCIP_CONS* cons = nullptr;

    SCIP_CALL_EXC(SCIPcreateConsLinear(subscip,
                                       &cons,
                                       namebuf.str().c_str(),
                                       0,
                                       NULL,
                                       NULL,
                                       -SCIPinfinity(subscip), //lhs
                                       1.0, //rhs
                                       TRUE, TRUE, TRUE, TRUE, TRUE,
                                       FALSE, FALSE, FALSE, FALSE, FALSE));

    SCIP_CALL_EXC(SCIPaddCoefLinear(subscip,
                                    cons,
                                    subVariables(first),
                                    1.0));

    SCIP_CALL_EXC(SCIPaddCoefLinear(subscip,
                                    cons,
                                    subVariables(second),
                                    1.0));

    SCIP_CALL_EXC(SCIPaddCons(subscip, cons));
    SCIP_CALL_EXC(SCIPreleaseCons(subscip, &cons));
  }

  bool SeparationProblem::isSubVertex(const TimedVertex& vertex) const
  {
    return contains(originalVertices, graph.underlyingVertex(vertex));
  }

  std::vector<TimedEdge> SeparationProblem::collectEdges(SCIP_SOL* solution)
  {
    assert(solution);

    std::vector<TimedEdge> edges;

    for(const Vertex& originalSource : originalVertices)
    {
      for(const TimedVertex& source : graph.getExpandedVertices(originalSource))
      {
        for(const TimedEdge& outgoing : graph.getOutgoing(source))
        {
          SCIP_VAR* var = subVariables(outgoing);

          if(var)
          {
            double value = SCIPgetSolVal(subscip, solution, var);

            if(value >= 0.5)
            {
              edges.push_back(outgoing);
            }
          }
        }
      }
    }

    return edges;
  }

  std::vector<TimedEdge> SeparationProblem::solve()
  {
    Log(debug) << "Solving separation problem";

    SCIP_CALL_EXC(SCIPsolve(subscip));

    Log(debug) << "Solved separation problem";

    SCIP_SOL* solution = SCIPgetBestSol(subscip);

    if(!solution)
    {
      return {};
    }

    return collectEdges(solution);
  }

  SeparationProblem::~SeparationProblem()
  {
    for(const Vertex& originalSource : originalVertices)
    {
      for(const TimedVertex& source : graph.getExpandedVertices(originalSource))
      {
        for(const TimedEdge& outgoing : graph.getOutgoing(source))
        {
          SCIP_VAR* var = subVariables(outgoing);

          if(var)
          {
            SCIP_CALL_ASSERT(SCIPreleaseVar(subscip, &var));
          }

          subVariables(outgoing) = nullptr;
        }
      }
    }

    if(debuggingEnabled())
    {
      for(const TimedEdge& edge : graph.getEdges())
      {
        assert(!subVariables(edge));
      }
    }

    SCIP_CALL_ASSERT(SCIPfree(&subscip));
  }
}

namespace std
{
  template <>
  struct hash<CandidateSet>
  {
    typedef std::size_t result_type;

    result_type operator()(const CandidateSet& candidates) const
    {
      result_type seed = 0;
      for(const Vertex& vertex : candidates.vertices)
      {
        compute_hash_combination(seed, std::hash<Vertex>{}(vertex));
      }

      return seed;
    }
  };
}

std::vector<OddPathFreeSet>
OddPathFreeSeparator::separate(const EdgeFunc<double>& weights,
                               int maxNumSets)
{
  std::vector<OddPathFreeSet> pathSets;

  std::vector<Vertex> originalVertices;

  for(const Vertex& originalVertex : originalGraph.getVertices())
  {
    if(originalVertex != originalSource)
    {
      originalVertices.push_back(originalVertex);
    }
  }

  const idx size = 3;

  int numSets = 0;

  EdgeMap<double> combinedWeights = graph.combinedValues(weights);
  VertexMap<CandidateSet> candidateSets(originalGraph, CandidateSet::empty());

  for(const auto& currentVertices : tuples(originalVertices, size))
  {
    assert(currentVertices.size() == size);
    assert(!contains(currentVertices, originalSource));

    double weight = getWeight(currentVertices, combinedWeights.getValues());

    if(weight < 1 + cutoff)
    {
      continue;
    }

    for(const Vertex& vertex : currentVertices)
    {
      if(weight > candidateSets(vertex).weight)
      {
        candidateSets(vertex) = CandidateSet{weight, currentVertices};
      }
    }
  }

  std::unordered_set<CandidateSet> bestSets;

  for(const Vertex& vertex : originalGraph.getVertices())
  {
    if(candidateSets(vertex))
    {
      bestSets.insert(candidateSets(vertex));
    }
  }

  if(!bestSets.empty())
  {
    Log(debug) << "Computing odd path-free sets for " << bestSets.size() << " candidate sets";
  }

  for(const CandidateSet& candidateSet : bestSets)
  {
    SeparationProblem separationProblem(graph, candidateSet.vertices, weights);

    std::vector<TimedEdge> edges = separationProblem.solve();

    double actualWeight = 0;

    for(const TimedEdge& timedEdge : edges)
    {
      actualWeight += weights(timedEdge);
    }

    if(actualWeight >= 1 + cutoff)
    {
      Log(debug) << "Found a violated cut, weight = "
                 << actualWeight;

      pathSets.push_back(OddPathFreeSet(edges, candidateSet.vertices));
      ++numSets;

      if(maxNumSets != -1 && numSets >= maxNumSets)
      {
        return pathSets;
      }
    }
  }

  return pathSets;
}


double OddPathFreeSeparator::getWeight(const std::vector<Vertex> originalVertices,
                                       const EdgeFunc<double>& originalWeights)
{
  double value = 0;

  for(const Vertex& vertex : originalVertices)
  {
    for(const Edge& outgoing : originalGraph.getOutgoing(vertex))
    {
      if(contains(originalVertices, outgoing.getTarget()))
      {
        value += originalWeights(outgoing);
      }
    }
  }

  return value;
}
