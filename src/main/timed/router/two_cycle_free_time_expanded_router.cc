#include "two_cycle_free_time_expanded_router.hh"

#include "timed_path_set.hh"

namespace
{
  class Label
  {
  private:
    TimedEdge edge;
    double cost;
    bool predLeft;

  public:
    Label(TimedEdge edge,
          double cost,
          bool predLeft)
      : edge(edge),
        cost(cost),
        predLeft(predLeft)
    {}

    Label()
      : cost(inf)
    {}

    static Label empty()
    {
      return Label();
    }

    double getCost() const
    {
      return cost;
    }

    bool getPredLeft() const
    {
      return predLeft;
    }

    TimedEdge getEdge() const
    {
      return edge;
    }

    bool isEmpty() const
    {
      return getCost() == inf;
    }

    TimedVertex getVertex() const
    {
      assert(!isEmpty());

      return edge.getTarget();
    }

    TimedVertex getPrevious() const
    {
      assert(!isEmpty());

      return edge.getSource();
    }

    idx getTime() const
    {
      return getVertex().getTime();
    }

    void update(const Label& other)
    {
      if(isEmpty())
      {
        *this = other;
      }
      else
      {
        assert(getVertex() == other.getVertex());

        if(other.getCost() < getCost())
        {
          *this = other;
        }
      }
    }

  };

  class LargeLabel
  {
  private:
    Label left, right;

    LargeLabel()
      : left(Label::empty()),
        right(Label::empty())
    {}

  public:
    LargeLabel(const Label& label)
      : left(label),
        right(Label::empty())
    {}

    static LargeLabel empty()
    {
      return LargeLabel();
    }

    const Label& getLeft() const
    {
      return left;
    }

    TimedVertex getVertex() const
    {
      assert(!isEmpty());

      return getLeft().getVertex();
    }

    idx getTime() const
    {
      return getVertex().getTime();
    }

    bool isEmpty() const
    {
      return getLeft().isEmpty() && getRight().isEmpty();
    }

    bool isFull() const
    {
      return !(getLeft().isEmpty() || getRight().isEmpty());
    }

    Label& getLeft()
    {
      return left;
    }

    const Label& getRight() const
    {
      return right;
    }

    Label& getRight()
    {
      return right;
    }

    bool sameUnderlyingPredecessor(const TimeExpandedGraph& graph,
                                   const Label& first,
                                   const Label& second) const
    {
      if(first.isEmpty() || second.isEmpty())
      {
        return false;
      }

      assert(first.getVertex() == second.getVertex());

      return graph.underlyingVertex(first.getPrevious()) ==
        graph.underlyingVertex(second.getPrevious());
    }

    bool shouldUseLeftLabel() const
    {
      if(getRight().isEmpty())
      {
        return true;
      }

      assert(!getLeft().isEmpty());

      return getLeft().getCost() < getRight().getCost();
    }

    void update(const TimeExpandedGraph& graph,
                const Label& other)
    {
      assert(!other.isEmpty());
      assert(isValid(graph));

      if(!isEmpty())
      {
        assert(getVertex() == other.getVertex());
      }

      if(getLeft().isEmpty())
      {
        assert(getRight().isEmpty());

        // fill from the left
        getLeft().update(other);

        assert(!getLeft().isEmpty());
        assert(getLeft().getCost() == other.getCost());
      }
      else
      {
        // left non-empty

        if(getRight().isEmpty())
        {
          if(sameUnderlyingPredecessor(graph, getLeft(), other))
          {
            getLeft().update(other);
            assert(!getLeft().isEmpty());
            assert(getRight().isEmpty());
          }
          else
          {
            getRight().update(other);
            assert(!getRight().isEmpty());
          }
        }
        else
        {
          assert(!sameUnderlyingPredecessor(graph, getLeft(), getRight()));

          // both non-empty
          if(sameUnderlyingPredecessor(graph, getLeft(), other))
          {
            getLeft().update(other);
          }
          else if(sameUnderlyingPredecessor(graph, getRight(), other))
          {
            getRight().update(other);
          }
          else
          {
            if(getLeft().getCost() > getRight().getCost())
            {
              getLeft().update(other);
            }
            else
            {
              getRight().update(other);
            }
          }
        }
      }

      assert(isValid(graph));
    }

    bool isValid(const TimeExpandedGraph& graph) const
    {
      if(getLeft().isEmpty())
      {
        return getRight().isEmpty();
      }

      if(getRight().isEmpty())
      {
        return true;
      }

      return !sameUnderlyingPredecessor(graph, getLeft(), getRight());
    }

  };

  TimedPath createPath(const Label& label,
                       const TimedVertex& timedSource,
                       const VertexMap<LargeLabel>& labels)
  {
    TimedPath path;

    const Label* currentLabel = &label;

    while(currentLabel->getVertex() != timedSource)
    {
      path.prepend(currentLabel->getEdge());

      if(currentLabel->getPredLeft())
      {
        currentLabel = &labels(currentLabel->getEdge().getSource()).getLeft();
      }
      else
      {
        currentLabel = &labels(currentLabel->getEdge().getSource()).getRight();
      }
    }

    return path;
  }

  typedef std::vector<LargeLabel> Labels;
}

TimeExpandedRouter::Result
TwoCycleFreeTimeExpandedRouter::findShortestPaths(const Request& request)
{
  Result result;

  const TimedVertex timedSource = graph.getVertex(originalSource, 0);

  const EdgeSet& forbiddenEdges = request.forbiddenEdges;

  VertexMap<LargeLabel> labels(graph, LargeLabel::empty());

  LargeLabel sourceLabel(Label(TimedEdge(timedSource,
                                         timedSource,
                                         inf),
                               0,
                               false));

  labels(timedSource) = sourceLabel;

  TimedPathSet bestPaths(request.maxNumPaths);

  auto insertLabel = [&](const Label& label)
    {
      const double value = label.getCost();

      if(value < bestPaths.cutoffValue().value_or(inf))
      {
        TimedPath timedPath = createPath(label, timedSource, labels);

        bestPaths.insert(timedPath, value);
      }
    };

  for(const TimedVertex& currentVertex : graph.getTopologicalOrdering())
  {
    const LargeLabel& currentLabel = labels(currentVertex);

    assert(currentLabel.isValid(graph));

    if(currentLabel.isEmpty())
    {
      continue;
    }

    assert(currentLabel.getVertex() == currentVertex);

    if(graph.underlyingVertex(currentVertex) == originalSource &&
       currentLabel.getTime() > 0)
    {
      if(cmp::gt(currentVertex.getTime(), request.upperTimeBound.value_or(inf)))
      {
        continue;
      }

      if(cmp::lt(currentVertex.getTime(), request.lowerTimeBound.value_or(-inf)))
      {
        continue;
      }

      if(!currentLabel.getLeft().isEmpty())
      {
        if(cmp::lt(currentLabel.getLeft().getCost(), request.cutoffCost.value_or(inf)))
        {
          insertLabel(currentLabel.getLeft());
        }

        if(cmp::lt(currentLabel.getRight().getCost(), request.cutoffCost.value_or(inf)))
        {
          insertLabel(currentLabel.getRight());
        }
      }

      continue;
    }

    const bool predLeft = currentLabel.shouldUseLeftLabel();
    const Label& bestLabel = predLeft ? currentLabel.getLeft() : currentLabel.getRight();
    const Label& otherLabel = predLeft ? currentLabel.getRight() : currentLabel.getLeft();

    for(const TimedEdge& outgoing : graph.getOutgoing(currentLabel.getVertex()))
    {
      if(forbiddenEdges.contains(graph.underlyingEdge(outgoing)))
      {
        continue;
      }

      const TimedVertex nextVertex = outgoing.getTarget();

      const double edgeCosts = request.costs(outgoing);

      if(graph.underlyingVertex(nextVertex) ==
         graph.underlyingVertex(bestLabel.getEdge().getSource()))
      {
        if(!otherLabel.isEmpty())
        {
          assert(graph.underlyingVertex(nextVertex) !=
                 graph.underlyingVertex(otherLabel.getEdge().getSource()));

          Label nextLabel(outgoing,
                          otherLabel.getCost() + edgeCosts,
                          !predLeft);

          labels(nextVertex).update(graph, nextLabel);
        }
      }
      else
      {
        Label nextLabel(outgoing,
                        bestLabel.getCost() + edgeCosts,
                        predLeft);

        labels(nextVertex).update(graph, nextLabel);
      }
    }
  }

  for(const TimedPath& timedPath : bestPaths.getPaths())
  {
    result.paths.push_back(timedPath);
  }

  result.minCost = bestPaths.bestValue();

  return result;
}
