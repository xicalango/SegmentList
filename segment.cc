#include <cstring>
#include <cassert>
#include <algorithm>
#include <cstdio>

#include "segment.h"


namespace xx {

  SegmentId RawSegmentList::store(char* data, std::size_t length) {
    if (fits_on_current_segment(length)) {
      store_on_same_segment(data, length);
    } else {
      store_on_multi_segments(data, length);
    }

    segment_ids.push_back( cur );
    
    return cur;
  }

  void RawSegmentList::store_on_same_segment(char* data, std::size_t length) {
    //  printf("writing %04X bytes from @%p to @%p\n", length, data, current_segment->data() + cur.offset);
    std::memcpy( current_segment->data() + cur.offset, data, length );
    cur.offset += length;
  }

  void RawSegmentList::store_on_multi_segments(char* data, std::size_t length) {
    
    std::size_t current_read = 0;

    while (current_read < length) {
      
      std::size_t remaining_size = get_remaining_size();
      std::size_t remaining_bytes = length - current_read;

      std::size_t bytes_to_copy = std::min(remaining_size, remaining_bytes);

      // printf("writing %04X bytes from @%p to @%p [remaining_on_segment = %u/%u, remaining_bytes = %u]\n", bytes_to_copy, data + current_read, current_segment->data() + cur.offset, remaining_size, SEGMENT_SIZE, remaining_bytes);

      std::memcpy( current_segment->data() + cur.offset, data + current_read, bytes_to_copy );

      cur.offset += bytes_to_copy;
      current_read += bytes_to_copy;

      if (cur.offset >= SEGMENT_SIZE) {
        assert(cur.offset == SEGMENT_SIZE);

        add_segment();
      }
    }

    assert(current_read == length);
  }

  std::pair<char*, std::size_t> RawSegmentList::restore(std::size_t idx) {
    SegmentId& prev_sid = segment_ids[idx];
    SegmentId& cur_sid = segment_ids[idx + 1];

    if (prev_sid.segment == cur_sid.segment) {
      return restore_same_segment(prev_sid, cur_sid);
    } else {
      return restore_multi_segments(prev_sid, cur_sid);
    }
  }

  std::pair<char*, std::size_t> RawSegmentList::restore_same_segment(SegmentId& prev_sid, SegmentId& cur_sid) {
    std::size_t len = cur_sid.offset - prev_sid.offset;
    segment* s = segments[prev_sid.segment];
    
    char* data = new char[len];

    std::memcpy( data, s->data() + prev_sid.offset, len );

    return std::make_pair(data, len);
  }

  std::pair<char*, std::size_t> RawSegmentList::restore_multi_segments(SegmentId& prev_sid, SegmentId& cur_sid) {
    std::size_t size_on_first_segment = SEGMENT_SIZE - prev_sid.offset;
    std::size_t size_on_last_segment = cur_sid.offset;
    std::size_t full_pages = 0;
    if (cur_sid.segment - prev_sid.segment > 2) {
      full_pages = ( (cur_sid.segment - prev_sid.segment) - 2 ) * SEGMENT_SIZE;
    }

    std::size_t length = size_on_first_segment + full_pages + size_on_last_segment;

    char* data = new char[length];
    char* cur_data = data;

    for(std::size_t i = prev_sid.segment; i <= cur_sid.segment; i++) {
      std::size_t cur_offset = i == prev_sid.segment ? prev_sid.offset : 0;
      std::size_t end_index = i == cur_sid.segment ? cur_sid.offset : SEGMENT_SIZE;
      std::size_t bytes_on_segment = end_index - cur_offset;

      segment* seg = segments[i];

      std::memcpy( cur_data, seg->data() + cur_offset, bytes_on_segment );

      cur_data += bytes_on_segment;
    }

    return std::make_pair(data, length);
  }

  std::size_t RawSegmentList::get_remaining_size() {
    return SEGMENT_SIZE - cur.offset;
  }

  bool RawSegmentList::fits_on_current_segment(std::size_t length) {
    return get_remaining_size() >= length;
  }

  std::size_t RawSegmentList::size() {
    return segment_ids.size() - 1;
  }

  void RawSegmentList::add_segment() {
    segment* new_segment = new segment({});
    
    segments.push_back( new_segment );
    cur.segment++;
    cur.offset = 0;

    current_segment = new_segment;
  }

  void RawSegmentList::debug() {
    for (auto sid : segment_ids) {
      printf("seg: %08X off: %08X\n", sid.segment, sid.offset);
    }

    for(std::size_t i = 0; i < segments.size(); i++) {
      char* ptr = segments[i]->data();
      for (std::size_t j = 0; j < SEGMENT_SIZE; j++) {
        printf("%u -- %p: %02X\n", i, ptr + j, ptr[j]);
      }
    }
  }

}


