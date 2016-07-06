#ifndef __SEGMENT_H
#define __SEGMENT_H

#include <vector>
#include <array>
#include <cstdint>
#include <utility>

namespace xx {

const std::size_t SEGMENT_SIZE = 4 * 1024 * 1024;

typedef std::array<char, SEGMENT_SIZE> segment;

struct SegmentId {
  uint32_t segment;
  uint32_t offset;
};

class RawSegmentList {

  std::vector<segment*> segments;
  std::vector<SegmentId> segment_ids;
  segment* current_segment;
  SegmentId cur;

  public: 

  RawSegmentList() {

      segment* first_segment = new segment({});
      segments.push_back(first_segment);

      cur.segment = 0;
      cur.offset = 0;

      segment_ids.push_back(cur);

      current_segment = first_segment;
  }

  ~RawSegmentList() {
    for (auto s : segments) {
      delete s;
    }
  }

  SegmentId store(char* data, std::size_t length);

  std::pair<char*, std::size_t> restore(std::size_t index);

  std::size_t size();

  private:

  std::pair<char*, std::size_t> restore_same_segment(SegmentId& prev_sid, SegmentId& cur_sid);
  std::pair<char*, std::size_t> restore_multi_segments(SegmentId& prev_sid, SegmentId& cur_sid);

  void store_on_same_segment(char* data, std::size_t length);
  void store_on_multi_segments(char* data, std::size_t length);

  std::size_t get_remaining_size();
  bool does_segment_fit_on_current(std::size_t length);

  void add_segment();

  public:

  void debug();

};

}

#endif //__SEGMENT_H

