#include <algorithm>
#include <limits>
#include <string>

#include  "stdint.h"  // <--- to prevent uint64_t errors! 

#include "Pipes.hh"
#include "TemplateFactory.hh"
#include "StringUtils.hh"

class WordCountMapper : public HadoopPipes::Mapper {
 public:
  // Constructor: does nothing
  WordCountMapper(HadoopPipes::TaskContext& context) {}
  // Map function: Receives a line, outputs (word,"1") to reducer.
  void map(HadoopPipes::MapContext& context) {
    // Get line of text
    std::string line = context.getInputValue();
    // Split into words
    std::vector<std::string> words = HadoopUtils::splitString(line, " ");
    // Emit each word tuple (word, "1" )
    for (unsigned int i = 0; i < words.size(); i++) {
      context.emit(words[i], HadoopUtils::toString(1));
    }
  }
};

class WordCountReducer : public HadoopPipes::Reducer {
 public:
  // Constructor: does nothing
  WordCountReducer(HadoopPipes::TaskContext& context) {}
  // Reduce function
  void reduce(HadoopPipes::ReduceContext& context) {
    int count = 0;
    // Get all tuples with the same key, and count their numbers
    while (context.nextValue()) {
      count += HadoopUtils::toInt(context.getInputValue());
    }
    // Emit (word, count)
    context.emit(context.getInputKey(), HadoopUtils::toString(count));
  }
};

int main(int argc, char *argv[]) {
  return HadoopPipes::runTask(
    HadoopPipes::TemplateFactory<WordCountMapper, WordCountReducer>()
  );
}