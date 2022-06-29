// 1). This implementation of a simple circular buffer
#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <functional>

template <size_t n_buffers, size_t buf_size>
class RingBuffer {
 private:
  // Single buffer
  struct Buf {
    std::array<uint8_t, buf_size> data;
    // data sequence number (unique, monotonic increment)
    std::atomic_ullong sequence_num;
    // flog to check if write to buffer is active
    std::atomic_bool write_active;
    // number write operations
    std::atomic_uint num_write_ops;
    // number of active readers
    std::atomic_uint num_readers;

    constexpr Buf()
        : sequence_num(0),
          write_active(false),
          num_write_ops(0),
          num_readers(0) {}
  };

 public:
  // Callback for writing, takes a buffer to fill and its size.
  // Returns non-zero on error.
  typedef std::function<int(uint8_t*, size_t)> write_callback;

  // Callback for reading, takes a buffer to read, its size, and sequence num.
  // Returns non-zero on error.
  typedef std::function<int(const uint8_t*, size_t, unsigned long long)>
      read_callback;

  // Write using callback
  int Write(write_callback write) {
    // Get next sequence num in atomic increments
    unsigned long long sequence_num = m_seq_num++;
    // Select buffer to write
    unsigned int write_index = advance_head();
    Buf& buf_to_write = m_circular_buffer.at(write_index);
    // Do the write by setting write active and writing to buffer
    // TODO (Related to processor architecture ARM vs x86) c++ can technically
    // reorder these operations, being atomic they will get written to memory,
    // but perhaps in opposite order. Investigate usage of
    // std::memory_thread_fence(std::memory_order_release) or modify the store()
    // and load() calls using memory_order_release and memory_order_acquire.
    buf_to_write.write_active.store(true);
    buf_to_write.num_write_ops++;
    // Store write success or fail
    int retval = write(buf_to_write.data.data(), buf_size);
    buf_to_write.seq_num.store(sequence_num);
    // Increment number of writes at the end and reset write_active flag
    buf_to_write.num_write_ops++;
    buf_to_write.write_active.store(false);
    return retval;
  }

  // Read using callback
  int Read(read_callback read, bool& corrupt) {
    // Select buffer to read (head-1)
    unsigned int head = m_head.load();
    unsigned int read_index = (head == 0) ? (n_buffers - 1) : (head - 1);
    Buf& buf_to_read = m_circular_buffer.at(read_index);
    // Do the read and detect whether writing happend during reading
    bool write_at_start = buf_to_read.write_active.load();
    unsigned int ops_at_start = buf_to_read.num_write_ops.load();
    buf_to_read.num_readers++;
    int retval = read(buf_to_read.data.data(), buf_size, buf_to_read.seq_num);
    buf_to_read.num_readers--;
    unsigned int ops_at_end = buf_to_read.num_writeops.load();
    bool write_at_end = buf_to_read.write_active.load();
    corrupt = (write_at_start || write_at_end || (ops_at_start != ops_at_end));
    return retval;
  }

 private:
  // Advance the head pointer in a thread-safe way
  unsigned int advance_head() {
    unsigned int next_head;
    unsigned int current_head = m_head.load();
    do {
      next_head = (current_head == n_buffers - 1) ? 0 : current_head + 1;
    } while (!m_head.compare_exchange_weak(current_head, next_head));
    return next_head;
  }

  std::array<Buf, n_buffers> m_circular_buffer;
  // Buff being written
  std::atomic_uint m_head = {0};
  // sequence counter
  std::atomic_ullong m_seq_num = {0};
}
