#ifndef HOLE_SET_HH
#define HOLE_SET_HH

#include <iomanip>

#include "time_expanded_router.hh"

#include "large_label.hh"

template<idx size>
class SetForm
{
public:
  typedef std::optional<Vertex> Entry;
  typedef std::array<Entry, size> Entries;
private:
  std::shared_ptr<Entries> entries;

  SetForm()
    : entries(std::make_shared<Entries>())
  {

  }

  Entries& getEntries()
  {
    return *entries;
  }

  Entry& getEntry(idx i)
  {
    return (*entries)[i];
  }

public:
  const Entry& getEntry(idx i) const
  {
    return (*entries)[i];
  }

  const Entries& getEntries() const
  {
    return *entries;
  }

  bool operator<=(const SetForm<size>& other) const
  {
    for(idx i = 0; i < size; ++i)
    {
      if(other.getEntry(i) && getEntry(i) != other.getEntry(i))
      {
        return false;
      }
    }
    return true;
  }

  bool isEmpty() const
  {
    for(idx i = 0; i < size; ++i)
    {
      if(getEntry(i))
      {
        return false;
      }
    }

    return true;
  }

  bool operator==(const SetForm<size>& other) const
  {
    return getEntries() == other.getEntries();
  }

  operator bool() const
  {
    return !isEmpty();
  }

  void insert(idx i, const Vertex& vertex)
  {
    getEntry(i) = std::make_optional(vertex);
  }

  void clear(idx i)
  {
    getEntry(i) = std::optional<Vertex>();
  }

  static SetForm<size> emptyForm()
  {
    return SetForm<size>();
  }

  static SetForm<size> singleton(idx i, const Vertex& vertex)
  {
    SetForm<size> setForm = SetForm<size>::emptyForm();

    assert(setForm.isEmpty());

    setForm.insert(i, vertex);

    assert(!setForm.isEmpty());

    return setForm;
  }

  std::optional<SetForm<size>> intersect(const SetForm<size>& other, const Graph& graph) const
  {
    VertexSet vertices(graph);

    SetForm<size> resultForm = SetForm<size>::emptyForm();

    for(idx i = 0; i < size; ++i)
    {
      const bool present = !!(getEntry(i));
      const bool otherPresent = !!(other.getEntry(i));

      if(!(present || otherPresent))
      {
        continue;
      }

      if(present && otherPresent)
      {
        if(getEntry(i) != other.getEntry(i))
        {
          return {};
        }
      }

      Vertex vertex = present ? *getEntry(i) : *(other.getEntry(i));

      if(vertices.contains(vertex))
      {
        return {};
      }

      vertices.insert(vertex);

      resultForm.insert(i, vertex);
    }


    return resultForm;
  }

  void print(std::ostream& out = std::cout) const
  {
    out << "SetForm(";

    for(idx i = 0; i < size; ++i)
    {
      const auto& entry = getEntry(i);

      if(entry)
      {
        out << std::setw(3) << *entry;
      }
      else
      {
        out << "  .";
      }
    }

    out << ")" << std::endl;
  }
};

template<idx size>
struct SetFormHasher
{
  std::size_t operator()(const SetForm<size>& setForm) const
  {
    std::size_t seed = 0;
    std::hash<Vertex> vertexHash{};

    for(const auto& entry : setForm.getEntries())
    {
      compute_hash_combination(seed, vertexHash(entry.value_or(Vertex())));
    }

    return seed;
  }
};

template<idx size>
class HoleSet
{
private:
  HoleSet()
  {}

  std::vector<SetForm<size>> setForms;

public:

  HoleSet(const std::vector<SetForm<size>>& setForms)
    : setForms(setForms)
  {

  }

  const std::vector<SetForm<size>> getSetForms() const
  {
    return setForms;
  }

  bool isEmpty() const
  {
    return getSetForms().size() == 0;
  }

  bool contains(const SetForm<size>& form, const Graph& graph) const
  {
    assert(setForms.size() < graph.getVertices().size());
    
    for(const auto& setForm : setForms)
    {
      if(form <= setForm)
      {
        return true;
      }
    }
    
    return false;
  }

  static HoleSet<size> fromPrefix(const LabelPrefix<size>& prefix)
  {
    HoleSet holeSet;

    idx prefixSize = 0;

    for(idx i = 0; i < size; ++i)
    {
      Vertex vertex = prefix[i];

      if(vertex == Vertex())
      {
        continue;
      }
      
      ++prefixSize;

      for(uint j = 0; j <= i; ++j)
      {
        holeSet.setForms.push_back(SetForm<size>::singleton(j, vertex));
      }
    }

    if(size == prefixSize)
    {
      assert(holeSet.getSetForms().size() == (size*(size + 1)/2));      
    }
    else
    {
      assert(holeSet.getSetForms().size() <= (size*(size + 1)/2));
    }

    return holeSet;
  }

  HoleSet<size> intersect(const HoleSet& other, const Graph& graph) const
  {
    const std::vector<SetForm<size>>& forms = setForms;
    const std::vector<SetForm<size>>& otherForms = other.setForms;


    std::vector<SetForm<size>> simpleForms;
    std::vector<SetForm<size>> otherSimpleForms;

    std::vector<SetForm<size>> resultForms;

    // check for inclusion...

    for(const auto& form : forms)
    {
      bool formIsSubSet = false;

      for(const auto& otherForm : otherForms)
      {
        if(form <= otherForm)
        {
          formIsSubSet = true;
          break;
        }
      }

      if(formIsSubSet)
      {
        resultForms.push_back(form);
      }
      else
      {
        simpleForms.push_back(form);
      }
    }

    for(const auto& otherForm : otherForms)
    {
      bool otherFormIsSubSet = false;

      for(const auto& form : forms)
      {
        if(otherForm <= form)
        {
          otherFormIsSubSet = true;
          break;
        }
      }

      if(otherFormIsSubSet)
      {
        resultForms.push_back(otherForm);
      }
      else
      {
        otherSimpleForms.push_back(otherForm);
      }
    }


    for(const auto& form : simpleForms)
    {
      for(const auto& otherForm : otherSimpleForms)
      {
        auto inter = form.intersect(otherForm, graph);

        if(inter)
        {
          resultForms.push_back(*inter);
        }
      }
    }

    // find undominated set forms...

    std::vector<SetForm<size>> undominatedForms;
    std::vector<bool> dominatedForms(resultForms.size(), false);

    for(idx i = 0; i < resultForms.size(); ++i)
    {
      bool dominated = false;

      for(idx j = 0; j < resultForms.size(); ++j)
      {
        if(i == j || dominatedForms[j])
        {
          continue;
        }

        if(resultForms[i] <= resultForms[j])
        {
          dominated = true;
          break;
        }
      }

      if(!dominated)
      {
        undominatedForms.push_back(resultForms[i]);
      }
      else
      {
        dominatedForms[i] = true;
      }
    }

    if(debuggingEnabled())
    {
      for(const auto& form : undominatedForms)
      {
        assert(contains(form, graph));
        assert(other.contains(form, graph));
      }
    }
    
    return HoleSet<size>(undominatedForms);
  }

  void print(std::ostream& out = std::cout) const
  {
    out << "HoleSet(" << std::endl;
    for(const auto& setForm : getSetForms())
    {
      out << "  ";
      setForm.print(out);
    }

    out << ")" << std::endl;
  }
};

template<idx size>
bool operator<=(const SetForm<size>& setForm, const HoleSet<size>& holeSet)
{
  for(const SetForm<size>& other : holeSet.getSetForms())
  {
    if(setForm <= other)
    {
      return true;
    }
  }

  return false;
}

#endif /* HOLE_SET_HH */
