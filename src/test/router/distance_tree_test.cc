#include <vector>
#include <stdexcept>

#include <gtest/gtest.h>

#include "router/distance_tree.hh"

const idx size = 10;

class DistanceTreeTest : public testing::Test
{
public:
  DistanceTreeTest();

protected:
  Graph graph;
  EdgeMap<num> costs;
  std::vector<Vertex> vertices;
  Vertex root;

};

DistanceTreeTest::DistanceTreeTest()
  : graph(size, {}),
    vertices(graph.getVertices().collect())
{
  for(idx i = 0; i < size - 1; ++i)
  {
    graph.addEdge(vertices[i], vertices[i + 1]);
  }

  costs = EdgeMap<num>(graph, 0);

  for(const Edge &edge : graph.getEdges())
  {
    costs(edge) = 1;
  }

  root = vertices[0];
}


TEST_F(DistanceTreeTest, test)
{
  auto costValues = costs.getValues();

  DistanceTree<Direction::OUTGOING> distanceTree(graph, costValues, root);

  distanceTree.extend();

  for(idx i = 0; i < vertices.size(); ++i)
  {
    ASSERT_EQ(distanceTree.distance(vertices[i]), i);
  }
}

TEST_F(DistanceTreeTest, testExtension)
{
  auto costValues = costs.getValues();

  DistanceTree<Direction::OUTGOING> distanceTree(graph, costValues, root);

  for(idx i = 1; i < vertices.size(); ++i)
  {
    ASSERT_THROW(distanceTree.distance(vertices[i]), std::invalid_argument);
  }
}


TEST_F(DistanceTreeTest, testRoots)
{
  auto costValues = costs.getValues();

  DistanceTree<Direction::OUTGOING> distanceTree(graph,
                                                 costValues,
                                                 vertices.begin(),
                                                 vertices.end());

  distanceTree.extend();

  for(idx i = 1; i < vertices.size(); ++i)
  {
    ASSERT_EQ(distanceTree.distance(vertices[i]), 0);
  }
}

TEST_F(DistanceTreeTest, testVertexExtension)
{
  auto costValues = costs.getValues();

  DistanceTree<Direction::OUTGOING> distanceTree(graph,
                                                 costValues,
                                                 root);

  for(idx i = 1; i < vertices.size(); ++i)
  {
    distanceTree.extend(vertices[i]);
    ASSERT_EQ(distanceTree.distance(vertices[i]), i);
  }
}

TEST_F(DistanceTreeTest, testFilter)
{
  auto costValues = costs.getValues();

  DistanceTree<Direction::OUTGOING> distanceTree(graph,
                                                 costValues,
                                                 root);

  distanceTree.extend(NoEdgeFilter());

  for(idx i = 1; i < vertices.size(); ++i)
  {
    ASSERT_THROW(distanceTree.distance(vertices[i]), std::invalid_argument);
  }
}


TEST_F(DistanceTreeTest, testInitialDistance)
{
  auto costValues = costs.getValues();

  DistanceTree<Direction::OUTGOING> distanceTree(graph,
                                                 costValues);

  distanceTree.add(root, 10);

  distanceTree.extend();

  for(idx i = 1; i < vertices.size(); ++i)
  {
    ASSERT_EQ(distanceTree.distance(vertices[i]), 10 + i);
  }
}
