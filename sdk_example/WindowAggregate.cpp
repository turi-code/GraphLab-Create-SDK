#include <string>
#include <vector>
#include <queue> 
#include <graphlab/flexible_type/flexible_type.hpp>
#include <graphlab/sdk/toolkit_function_macros.hpp>
#include <graphlab/sdk/toolkit_class_macros.hpp>
#include <graphlab/sdk/gl_sarray.hpp>
#include <graphlab/sdk/gl_sframe.hpp>

using namespace graphlab;

class BASE_AGGREGATE {

  protected:
    flexible_type state;
    bool _is_incremental;
    size_t num_elements;
  public: 

    BASE_AGGREGATE() { 
      initiate();
    }
    // init phase 
    virtual void initiate() { 
      state = 0;
      _is_incremental = false;
      num_elements = 0;
    }
    // process phase 
    virtual void add_element(const flexible_type & value) { }

    // delete phase 
    virtual void remove_element(const flexible_type & value) { }
    
    // emit phase 
    virtual flexible_type emit() { return state; } 
        
    virtual bool is_incremental() { 
      return _is_incremental; 
    }

    virtual flex_type_enum output_type(const flex_type_enum & input_type) { 
      return input_type;
    }
};

class AVG : public BASE_AGGREGATE {
  public: 
    AVG(): BASE_AGGREGATE() { 
      initiate();
    }
    // init phase 
    void initiate() { 
      state = 0;
      _is_incremental = true;
      num_elements = 0;
    }

    // process phase 
    void add_element(const flexible_type & value) {
      state += value;
      num_elements++; 
    }
    
    // delete phase 
    void remove_element(const flexible_type & value) {
      state -= value;
      num_elements--;
    }

    // emit phase 
    flexible_type emit() { 
      flex_float state_float = state;
      return state_float / (num_elements) ;
    }
    
    flex_type_enum output_type(const flex_type_enum & input_type) {
      return flex_type_enum::FLOAT;
    }
};

class MAX : public BASE_AGGREGATE {
  public: 
    MAX(): BASE_AGGREGATE() { 
      initiate();
    }
    // init phase 
    void initiate() { 
      num_elements = 0;
      state = 0;
      _is_incremental = false;
    }

    // process phase 
    void add_element(const flexible_type & value) {
      if(num_elements == 0)
        state = value;
      else if(value > state)
        state = value;
      num_elements++;
    }

    // emit phase 
    flexible_type emit() { 
      return state;
    }

};

class MIN : public BASE_AGGREGATE {
  public: 
    MIN(): BASE_AGGREGATE() { 
      initiate();
    }
    // init phase 
    void initiate() { 
      num_elements = 0;
      state = 0;
      _is_incremental = false;
    }

    // process phase 
    void add_element(const flexible_type & value) {
      if(num_elements == 0)
        state = value;
      else if(value < state)
        state = value;
      num_elements++;
    }

    // emit phase 
    flexible_type emit() { 
      return state;
    }

};



class COUNT : public BASE_AGGREGATE {
  public: 
    COUNT(): BASE_AGGREGATE() { 
      initiate();
    }
    // init phase 
    void initiate() { 
      state = 0;
      _is_incremental = true;
    }

    // process phase 
    void add_element(const flexible_type & value) {
      state += 1; 
    }

    // delete phase 
    void remove_element(const flexible_type & value) {
      state -= 1; 
    }

    // emit phase 
    flexible_type emit() { 
      return state;
    }

};

class SUM: public BASE_AGGREGATE {
  public: 
    SUM(): BASE_AGGREGATE() {
      initiate();
    }
    // init phase 
    void initiate() { 
      state = 0;
      _is_incremental = true;
    }

    // process phase 
    void add_element(const flexible_type & value) {
      state += value; 
    }

    // delete phase 
    void remove_element(const flexible_type & value) {
      state -= value; 
    }

    // emit phase 
    flexible_type emit() { 
      return state;
    }
};

size_t COUNT_AGG() { 
  return (size_t)new COUNT();
}

size_t SUM_AGG() { 
  return (size_t)new SUM();
}

size_t MIN_AGG() { 
  return (size_t)new MIN();
}

size_t MAX_AGG() { 
  return (size_t)new MAX();
}

size_t AVG_AGG() { 
  return (size_t)new AVG();
}

gl_sarray window_aggregate(std::function<size_t()> fn,gl_sarray & sa,size_t window_size) { 
  BASE_AGGREGATE * agg = reinterpret_cast<BASE_AGGREGATE*>(fn());
  gl_sarray_writer writer(agg->output_type(sa.dtype()), 1);
  if(window_size <= 0)
    return writer.close();
  
  std::queue<flexible_type> * queue = new std::queue<flexible_type>();

  for (const auto &elem: sa.range_iterator()) {
    queue->push(elem);
    agg->add_element(elem);

    if(queue->size() == window_size) { 
      flexible_type agg_value = agg->emit();
      writer.write(agg_value,0);
      if(agg->is_incremental()) {
        flexible_type & oldest_value = queue->front();
        agg->remove_element(oldest_value);
        queue->pop();
      } else { 
        queue->pop();
        std::queue<flexible_type> * queue2 = new std::queue<flexible_type>();
        agg->initiate();
        while (!queue->empty()){ 
          flexible_type & oldest_value = queue->front();          
          queue2->push(oldest_value);
          agg->add_element(oldest_value);
          queue->pop();
        }
        delete queue;
        queue = queue2;
      }
    }

  }
  delete agg;
  delete queue;
  return writer.close();
}

BEGIN_FUNCTION_REGISTRATION
REGISTER_FUNCTION(SUM_AGG);
REGISTER_FUNCTION(COUNT_AGG);
REGISTER_FUNCTION(MAX_AGG);
REGISTER_FUNCTION(MIN_AGG);
REGISTER_FUNCTION(AVG_AGG);
REGISTER_FUNCTION(window_aggregate, "aggregate_fn","sarray","window_size");
END_FUNCTION_REGISTRATION
