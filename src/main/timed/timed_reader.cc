#include "timed_reader.hh"
#include "graph/edge_map.hh"
#include "graph/vertex_map.hh"
#include "timed/timed_edge_map.hh"

#include <string>
#include <unordered_map>

#include <boost/tokenizer.hpp>
#include <unordered_set>
#include <utility>

typedef std::unordered_map<std::string, idx> Header;

typedef std::pair<std::string, std::string> NodePair;


struct PairHasher
{
  typedef std::size_t result_type;

  result_type operator()(const std::pair<std::string, std::string>& pair) const
  {
    result_type seed = 0;

    compute_hash_combination(seed, std::hash<std::string>{}(pair.first));
    compute_hash_combination(seed, std::hash<std::string>{}(pair.second));

    return seed;
  }
};

struct Line {
  std::vector<num> travelTimes;
  std::string edgeName;
};

static const std::string fromNodeId = "from_node_id";
static const std::string toNodeId = "to_node_id";
static const std::string linkId = "link_id";

std::vector<std::string> readTokens(const std::string& line)
{
  boost::tokenizer<boost::escaped_list_separator<char>> tokenizer(line);

  std::vector<std::string> tokens(tokenizer.begin(), tokenizer.end());

  return tokens;
}

Header readHeader(const std::string &line)
{
  Header header;

  std::vector<std::string> tokens = readTokens(line);

  for(idx i = 0; i < tokens.size(); ++i)
  {
    const std::string token = tokens[i];

    auto it = header.find(token);

    if(it != std::end(header))
    {
      throw std::invalid_argument("Duplicate column");
    }

    header.insert(std::make_pair(token, i));
  }

  return header;
}

idx requiredColumn(const Header &header, const std::string &name)
{
  auto it = header.find(name);

  if(it == std::end(header))
  {
    throw std::invalid_argument("Missing column");
  }

  return it->second;
}

std::vector<idx> timeColumns(const Header &header, idx& timeHorizon)
{
  std::vector<idx> columns;

  for(idx i = 0;; ++i)
  {
    auto it = header.find(std::to_string(i));

    if(it == std::end(header))
    {
      break;
    }

    timeHorizon = i;

    columns.push_back(it->second);
  }

  if(columns.empty())
  {
    throw std::invalid_argument("Missing column");
  }

  return columns;
}

TimedReadResult readTimedInstance(std::ifstream& input)
{
  std::string line;

  if(!getline(input, line))
  {
    throw std::invalid_argument("Empty input");
  }

  Header header = readHeader(line);

  const idx fromNodeColumn = requiredColumn(header, fromNodeId);
  const idx toNodeColumn = requiredColumn(header, toNodeId);
  const idx linkColumn = requiredColumn(header, linkId);

  idx timeHorizon;

  const std::vector<idx> timeCols = timeColumns(header, timeHorizon);

  std::unordered_map<std::string, idx> nodeMap;

  auto addNode = [&](const std::string& name)
    {
      auto it = nodeMap.find(name);

      if(it != std::end(nodeMap))
      {
        return;
      }

      idx size = nodeMap.size();
      nodeMap.insert(std::make_pair(name, size));
    };

  std::unordered_map<NodePair, Line, PairHasher> lineMap;

  while(getline(input, line))
  {
    std::vector<std::string> tokens = readTokens(line);

    const std::string fromNode = tokens.at(fromNodeColumn);
    const std::string toNode = tokens.at(toNodeColumn);
    const std::string link = tokens.at(linkColumn);

    std::vector<num> travelTimes;
    travelTimes.reserve(timeCols.size());

    addNode(fromNode);
    addNode(toNode);

    for(const idx& time : timeCols)
    {
      travelTimes.push_back(std::stoul(tokens.at(time)));
    }

    NodePair nodePair = std::make_pair(fromNode, toNode);

    auto it = lineMap.find(nodePair);

    if(it != std::end(lineMap))
    {
      throw std::invalid_argument("Duplicate link");
    }

    Line line{travelTimes, link};

    lineMap.insert(std::make_pair(nodePair, line));
  }

  TimedReadResult result;

  const idx numVertices = nodeMap.size();

  result.graph = Graph::complete(numVertices);

  const Graph& graph = result.graph;

  result.vertexNames = VertexMap<std::string>(graph, "");

  for(const auto& pair : nodeMap)
  {
    Vertex vertex = graph.getVertices()[pair.second];
    result.vertexNames(vertex) = pair.first;
  }

  result.edgeNames = EdgeMap<std::string>(graph, "");
  result.travelTimes = TimedEdgeMap<num>(graph);

  for(const Edge& edge : graph.getEdges())
  {
    std::string fromNode = result.vertexNames(edge.getSource());
    std::string toNode = result.vertexNames(edge.getTarget());

    NodePair pair = std::make_pair(fromNode, toNode);

    auto it = lineMap.find(pair);

    if(it != std::end(lineMap))
    {
      const Line& line = it->second;

      result.edgeNames(edge) = line.edgeName;
      result.travelTimes.edgeValues(edge) = line.travelTimes;
    }
    else
    {
      // Disable edge
      result.travelTimes.edgeValues(edge).resize(timeHorizon, timeHorizon + 1);
    }
  }

  return result;
}
