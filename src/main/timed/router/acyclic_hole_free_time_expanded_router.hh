#ifndef ACYCLIC_HOLE_FREE_TIME_EXPANDED_ROUTER_HH
#define ACYCLIC_HOLE_FREE_TIME_EXPANDED_ROUTER_HH

#include <array>
#include <unordered_set>

#include "time_expanded_router.hh"
#include "timed_path_set.hh"

#include "hole_set.hh"

template<idx size>
class AcyclicHoleFreeTimeExpandedRouter : public TimeExpandedRouter
{
public:
  AcyclicHoleFreeTimeExpandedRouter(const TimeExpandedGraph& graph,
                                    Vertex originalSource)
    : TimeExpandedRouter(graph, originalSource)
  {
    assert(size <= graph.underlyingGraph().getVertices().size());
  }

  TimeExpandedRouter::Result findShortestPaths(const TimeExpandedRouter::Request& request) override;

};

template <idx size>
TimeExpandedRouter::Result
AcyclicHoleFreeTimeExpandedRouter<size>::findShortestPaths(const TimeExpandedRouter::Request& request)
{
  TimeExpandedRouter::Result result;

  Log(info) << "Finding new " << size << "-cycle free paths";

  VertexMap<LabelSet<size>> labels(graph, LabelSet<size>());

  VertexMap<ReverseLabel> lowerBounds =findLowerBounds(request);

  TimedVertex timedSource = graph.getVertex(originalSource, 0);

  labels(timedSource).insert(std::make_shared<LargeLabel<size>>(timedSource, originalSource, 0));

  idx numLabels = 0;

  TimedPathSet bestPaths(request.maxNumPaths);

  auto insertLabel = [&](const LabelPtr<size>& label)
    {
      const double value = label->getCost();

      if(value < bestPaths.cutoffValue().value_or(inf))
      {
        bestPaths.insert(label->createPath(), value);
      }
    };

  idx numDiscarded = 0;
  idx numImproved = 0;

  for(const TimedVertex& timedVertex : graph.getTopologicalOrdering())
  {
    if(graph.underlyingVertex(timedVertex) == originalSource &&
       timedVertex.getTime() > 0)
    {
      if(cmp::gt(timedVertex.getTime(), request.upperTimeBound.value_or(inf)))
      {
        continue;
      }

      if(cmp::lt(timedVertex.getTime(), request.lowerTimeBound.value_or(-inf)))
      {
        continue;
      }

      for(const LabelPtr<size>& currentLabel : labels(timedVertex))
      {
        if(cmp::lt(currentLabel->getCost(), request.cutoffCost.value_or(inf)))
        {
          insertLabel(currentLabel);
        }
      }

      continue;
    }

    for(const LabelPtr<size>& currentLabel : labels(timedVertex))
    {
      if(currentLabel->getCost() == inf)
      {
        continue;
      }

      {
        auto boundLabel = lowerBounds(timedVertex);

        // no target reachable from the current vertex
        if(boundLabel.getCost() == inf)
        {
          continue;
        }

        const double lowerBound = boundLabel.getCost() + currentLabel->getCost();

        if(lowerBound > bestPaths.bestValue().value_or(inf))
        {
          ++numDiscarded;
          continue;
        }

        // try extending the path

        LabelPrefix<size> prefix = currentLabel->getPrefix();
        idx i = 0;

        bool canExtend = true;

        auto currentBoundLabel = boundLabel;

        while(currentBoundLabel.getTimedEdge())
        {
          TimedVertex nextVertex = currentBoundLabel.getTimedEdge()->getTarget();
          Vertex underlyingVertex = graph.underlyingVertex(nextVertex);

          if(contains(prefix, underlyingVertex))
          {
            canExtend = false;
            break;
          }

          prefix[i] = underlyingVertex;

          i = (i+1) % size;
          currentBoundLabel = lowerBounds(nextVertex);
        }

        if(canExtend)
        {
          TimedPath nextPath = currentLabel->createPath();

          currentBoundLabel = boundLabel;

          while(currentBoundLabel.getTimedEdge())
          {
            nextPath.append(*currentBoundLabel.getTimedEdge());
            TimedVertex nextVertex = currentBoundLabel.getTimedEdge()->getTarget();
            currentBoundLabel = lowerBounds(nextVertex);
          }

          if(debuggingEnabled())
          {
            assert(graph.underlyingVertex(nextPath.getSource()) == originalSource);
            assert(graph.underlyingVertex(nextPath.getTarget()) == originalSource);

            assert(nextPath.getSource().getTime() == 0);
            assert(nextPath.getTarget().getTime() > 0);

            assert(nextPath.girth(graph) > size);
            assert(cmp::eq(lowerBound, nextPath.cost(request.costs)));
          }

          ++numImproved;

          if(cmp::lt(lowerBound, request.cutoffCost.value_or(inf)))
          {
            if(lowerBound < bestPaths.cutoffValue().value_or(inf))
            {
              bestPaths.insert(nextPath, lowerBound);
            }
          }
        }
      }

      {
        const Graph& originalGraph = graph.underlyingGraph();
        const HoleSet<size> currentHoleSet = HoleSet<size>::fromPrefix(currentLabel->getPrefix());

        assert(currentHoleSet.getSetForms().size() < originalGraph.getVertices().size());

        // holeSet.print();

        HoleSet<size> runningHoleSet(std::vector<SetForm<size>>{SetForm<size>::emptyForm()});

        bool isDominated = false;

        std::unordered_set<SetForm<size>, SetFormHasher<size>> discardableForms;

        for(const LabelPtr<size>& otherLabel : labels(timedVertex))
        {
          if(otherLabel == currentLabel)
          {
            continue;
          }

          if(otherLabel->getCost() > currentLabel->getCost())
          {
            continue;
          }

          HoleSet<size> otherHoleSet = HoleSet<size>::fromPrefix(otherLabel->getPrefix());

          runningHoleSet = runningHoleSet.intersect(otherHoleSet, originalGraph);

          std::vector<SetForm<size>> reducedForms;

          for(const SetForm<size>& setForm : runningHoleSet.getSetForms())
          {
            if(contains(discardableForms, setForm))
            {
              assert(setForm <= currentHoleSet);
              continue;
            }
            else
            {
              if(setForm <= currentHoleSet)
              {
                discardableForms.insert(setForm);
                continue;
              }
            }

            reducedForms.push_back(setForm);
          }

          runningHoleSet = HoleSet<size>(reducedForms);

          if(runningHoleSet.isEmpty())
          {
            isDominated = true;
            break;
          }
        }

        if(isDominated)
        {
          continue;
        }
      }

      // ...

      for(const TimedEdge& outgoing : graph.getOutgoing(timedVertex))
      {
        if(request.forbiddenEdges.contains(graph.underlyingEdge(outgoing)))
        {
          continue;
        }

        if(!currentLabel->canExtend(graph.underlyingVertex(outgoing.getTarget())))
        {
          continue;
        }

        const double edgeCost = request.costs(outgoing);

        LabelPtr<size> nextLabel = std::make_shared<LargeLabel<size>>(outgoing,
                                                                      graph.underlyingVertex(outgoing.getTarget()),
                                                                      currentLabel->getCost() + edgeCost,
                                                                      currentLabel);

        ++numLabels;

        assert(nextLabel->getVertex() == outgoing.getTarget());
        assert(nextLabel->getPrefix().back() == graph.underlyingVertex(outgoing.getTarget()));

        labels(outgoing.getTarget()).insert(nextLabel);
      }
    }
  }

  Log(debug) << "Created "
             << numLabels
             << " labels, discarded "
             << numDiscarded
             << " and found "
             << numImproved
             << " improved bounds";

  for(const auto& timedPath : bestPaths.getPaths())
  {
    result.paths.push_back(timedPath);
  }

  result.minCost = bestPaths.bestValue();

  return result;
}


#endif /* ACYCLIC_HOLE_FREE_TIME_EXPANDED_ROUTER_HH */
