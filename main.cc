#include <cstdio>
#include <cassert>
#include <cstring>

#include "segment.h"

const std::size_t ELEMENTS = xx::RawSegmentList::SEGMENT_SIZE;

void store_string(xx::RawSegmentList& list, char* str) {
  std::size_t len = std::strlen(str) + 1;
  list.store(str, len);
}


int main(void) {

  xx::RawSegmentList list;

  char* buffer = new char[100];

  for(std::size_t i = 0; i < ELEMENTS; i++) {
    sprintf(buffer, "%lu", i);
    store_string( list, buffer );
  }

  printf("list size: %lu\n", list.size());

  for(std::size_t i = 0; i < list.size(); i++) {
    char* str = std::get<0>( list.restore(i) );
    sprintf(buffer, "%lu", i);
    assert( std::strncmp(str, buffer, 100) == 0 );
  }

  delete[] buffer;

  return 0;
}

