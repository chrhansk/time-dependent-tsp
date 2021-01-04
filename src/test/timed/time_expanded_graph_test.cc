#include <gtest/gtest.h>

#include "timed/time_expanded_graph.hh"

class Fixture : public testing::Test
{
protected:
  Graph underlyingGraph;
  std::vector<Vertex> vertices;
  Edge underlyingEdge;

  TimeExpandedGraph graph;
  TimedVertex source;
  TimedVertex target;
  TimedEdge edge;

public:
  Fixture()
    : underlyingGraph(3, {}),
      vertices(underlyingGraph.getVertices().collect()),
      underlyingEdge(underlyingGraph.addEdge(*vertices.begin(), *vertices.rbegin())),
      graph(underlyingGraph),
      source(graph.addVertex(*vertices.begin(), 5)),
      target(graph.addVertex(*vertices.rbegin(), 8)),
      edge(graph.addEdge(source, target, underlyingEdge))
  {

  }
};




TEST(TimeExpandedGraphTest, testConstruction)
{
  Graph underlyingGraph(3, {});

  TimeExpandedGraph graph(underlyingGraph);

  ASSERT_EQ(0, graph.getVertices().size());
  ASSERT_EQ(0, graph.getEdges().size());
}

TEST(TimeExpandedGraphTest, testVertexAddition)
{
  Graph underlyingGraph(3, {});

  TimeExpandedGraph graph(underlyingGraph);

  TimedVertex vertex = graph.addVertex(*underlyingGraph.getVertices().begin(), 5);

  ASSERT_EQ(1, graph.getVertices().size());
  ASSERT_EQ(5, vertex.getTime());
}

TEST(TimeExpandedGraphTest, testUnderlyingVertex)
{
  Graph underlyingGraph(3, {});

  TimeExpandedGraph graph(underlyingGraph);

  Vertex vertex = *underlyingGraph.getVertices().begin();

  TimedVertex timedVertex = graph.addVertex(vertex, 5);

  ASSERT_EQ(vertex, graph.underlyingVertex(timedVertex));
}

TEST(TimeExpandedGraphTest, testEdgeAddition)
{
  Graph underlyingGraph(3, {});
  std::vector<Vertex> vertices = underlyingGraph.getVertices().collect();
  Edge simpleEdge = underlyingGraph.addEdge(*vertices.begin(), *vertices.rbegin());

  TimeExpandedGraph graph(underlyingGraph);

  TimedVertex source = graph.addVertex(*vertices.begin(), 5);
  TimedVertex target = graph.addVertex(*vertices.rbegin(), 8);

  graph.addEdge(source, target, simpleEdge);

  ASSERT_EQ(2, graph.getVertices().size());
  ASSERT_EQ(1, graph.getEdges().size());
}

TEST(TimeExpandedGraphTest, testNegativeEdgeLength)
{
  Graph underlyingGraph(3, {});

  TimeExpandedGraph graph(underlyingGraph);
  std::vector<Vertex> vertices = underlyingGraph.getVertices().collect();

  TimedVertex source = graph.addVertex(*vertices.begin(), 10);
  TimedVertex target = graph.addVertex(*vertices.rbegin(), 8);

  Edge edge = underlyingGraph.addEdge(graph.underlyingVertex(source),
                                      graph.underlyingVertex(target));

  ASSERT_ANY_THROW({graph.addEdge(source, target, edge);});
}

TEST_F(Fixture, testOutgoing)
{
  ASSERT_EQ(std::vector<TimedEdge>{edge}, graph.getOutgoing(source).collect());
}

TEST_F(Fixture, testIncoming)
{
  ASSERT_EQ(std::vector<TimedEdge>{edge}, graph.getIncoming(target).collect());
}


TEST_F(Fixture, testOutgoingEmpty)
{
  ASSERT_EQ(std::vector<TimedEdge>{}, graph.getOutgoing(target).collect());
}

TEST_F(Fixture, testIncomingEmpty)
{
  ASSERT_EQ(std::vector<TimedEdge>{}, graph.getIncoming(source).collect());
}

TEST_F(Fixture, getExistingVertex)
{
  ASSERT_EQ(source, graph.getVertex(graph.underlyingVertex(source),
                                    source.getTime(), false));
}


TEST_F(Fixture, addNonExistingVertex)
{
  TimedVertex nextVertex = graph.getVertex(graph.underlyingVertex(source), 17, true);

  ASSERT_EQ(graph.underlyingVertex(nextVertex), graph.underlyingVertex(source));
  ASSERT_NE(source, nextVertex);
  ASSERT_EQ(17, nextVertex.getTime());
}


TEST_F(Fixture, getNonExistingVertex)
{
  ASSERT_ANY_THROW({graph.getVertex(graph.underlyingVertex(source), 17, false);});
}
