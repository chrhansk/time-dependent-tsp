#include "instance.hh"

#include <random>
#include <unordered_set>

std::vector<InstanceInfo> InstanceInfo::tinyInstances()
{
  std::vector<InstanceInfo> infos{};

  idx numVertices = 6;

  infos.push_back(InstanceInfo(0, numVertices, 143));
  infos.push_back(InstanceInfo(1, numVertices, 266));
  infos.push_back(InstanceInfo(2, numVertices, 259));
  infos.push_back(InstanceInfo(3, numVertices, 258));
  infos.push_back(InstanceInfo(4, numVertices, 251));
  infos.push_back(InstanceInfo(5, numVertices, 279));
  infos.push_back(InstanceInfo(6, numVertices, 363));
  infos.push_back(InstanceInfo(7, numVertices, 298));
  infos.push_back(InstanceInfo(8, numVertices, 335));
  infos.push_back(InstanceInfo(9, numVertices, 265));

  return infos;

}

std::vector<InstanceInfo> InstanceInfo::smallInstances()
{
  std::vector<InstanceInfo> infos{};

  idx numVertices = 10;

  infos.push_back(InstanceInfo(0, numVertices, 269, 202.612,
                               std::vector<double>{246.600, 261.000},
                               1000));
  infos.push_back(InstanceInfo(1, numVertices, 276, 205.071,
                               std::vector<double>{232.143, 253.167},
                               1000));
  infos.push_back(InstanceInfo(2, numVertices, 291, 275.805,
                               std::vector<double>{284.916, 289.167},
                               1000));
  infos.push_back(InstanceInfo(3, numVertices, 340, 288.555,
                               std::vector<double>{303.000, 340.000},
                               1000));
  infos.push_back(InstanceInfo(4, numVertices, 418, 373.181,
                               std::vector<double>{411.667, 411.667},
                               1000));
  infos.push_back(InstanceInfo(5, numVertices, 403, 368.971,
                               std::vector<double>{393.000, 393.000},
                               1000));
  infos.push_back(InstanceInfo(6, numVertices, 386, 369.045,
                               std::vector<double>{382.333, 386.000},
                               1000));
  infos.push_back(InstanceInfo(7, numVertices, 333, 319.270,
                               std::vector<double>{333.000, 333.000},
                               1000));
  infos.push_back(InstanceInfo(8, numVertices, 347, 322.697,
                               std::vector<double>{327.167, 337.714},
                               1000));
  infos.push_back(InstanceInfo(9, numVertices, 298, 284.524,
                               std::vector<double>{291.537, 298.000},
                               1000));

  return infos;
}

std::vector<InstanceInfo> InstanceInfo::mediumInstances()
{
  std::vector<InstanceInfo> infos{};

  idx numVertices = 20;

  for(idx i = 0; i < 50; ++i)
  {
    infos.push_back(InstanceInfo(i, numVertices, -1));
  }

  return infos;
}

std::vector<InstanceInfo> InstanceInfo::largeInstances()
{
  std::vector<InstanceInfo> infos{};

  idx numVertices = 40;

  for(idx i = 0; i < 20; ++i)
  {
    infos.push_back(InstanceInfo(i,
                                 numVertices,
                                 -1));
  }

  return infos;
}

SimpleInstance::SimpleInstance(const InstanceInfo& info)
  : info(info),
    engine(info.seed),
    graph(Graph::complete(info.numVertices)),
    costs(graph, 0),
    dijkstra(graph),
    staticCosts(dijkstra, costs.getValues())
{
  VertexMap<Point> points(graph, Point(0, 0));

  auto coordinates = std::uniform_int_distribution<>(0, 100);

  std::unordered_set<Point> pointSet;

  for(const Vertex& vertex : graph.getVertices())
  {
    while(true)
    {
      Point point(coordinates(engine), coordinates(engine));

      if(pointSet.find(point) != std::end(pointSet))
      {
        continue;
      }

      pointSet.insert(point);

      points(vertex) = point;

      break;
    }
  }

  for(const Edge& edge : graph.getEdges())
  {
    num cost = (num) points(edge.getSource()).distance(points(edge.getTarget()));
    assert(cost > 0);
    costs(edge) = cost;
  }
}


Instance::Instance(const InstanceInfo& info)
  : SimpleInstance(info),
    distribution(0, info.timeSteps),
    timedCosts(AugmentedEdgeFunc::generate(graph,
                                           costs.getValues(),
                                           info.scaleFactor,
                                           info.numBreaks,
                                           info.timeSteps,
                                           [&]() -> idx
                                           {
                                             return distribution(engine);
                                           })),
    timedDistances(graph,
                   graph.getVertices().collect(),
                   timedCosts)
{
}
