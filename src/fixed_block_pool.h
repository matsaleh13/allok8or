/**
 * @file fixed_block_pool.h
 * @brief Header for a pool of fixed sized blocks.
 *
 */
#pragma once

// Project headers
#include "types.h"
#include "logging.h"

// Library headers

namespace allok8or {


/**
 * @brief Manages a pool of FixedBlockHeaders.
 * NOTE: Blocks are added only to the head of the doubly-linked list.
 * 
 * @tparam TFixedBlock FixedBlockHeader template instance.
 */
template <typename TFixedBlock>
class FixedBlockPool {
public:
  FixedBlockPool();
  ~FixedBlockPool();

  // No copies; share when appropriate.
  FixedBlockPool(const FixedBlockPool&) = delete;
  FixedBlockPool& operator=(const FixedBlockPool&) = delete;

  bool add(TFixedBlock* block);
  bool remove(TFixedBlock* block);
  bool in_list(TFixedBlock* block) const;
  const TFixedBlock* head() const { return m_head; }
  const TFixedBlock* tail() const { return m_tail; }

  llong_t num_blocks() const { return m_num_blocks; }
  llong_t num_bytes() const { return m_num_bytes; }

private:
  TFixedBlock* m_head;
  TFixedBlock* m_tail;
  llong_t m_num_blocks;
  llong_t m_num_bytes;
};

/**
 * FixedBlockPool ctor
 */
template <typename TFixedBlock>
FixedBlockPool<TFixedBlock>::FixedBlockPool()
    : m_head(nullptr),
      m_tail(nullptr),
      m_num_blocks(0),
      m_num_bytes(0) {}

/**
 * FixedBlockPool<TFixedBlock> dtor
 *
 * Logs an error if any are still in use at shutdown.
 */
template <typename TFixedBlock>
FixedBlockPool<TFixedBlock>::~FixedBlockPool() {
  if (m_num_blocks || m_head || m_tail) {
    LOG_ERROR("Detected memory leaks when deleting FixedBlockPool<TFixedBlock> "
              "[%d]; leaking [%d] bytes.",
              m_num_blocks,
              m_num_bytes);
  }
}

/**
 * @brief Adds a memory block (with header) to the internal list.
 *
 * @param block Pointer to the block to add.
 * @return true when block added.
 * @return false when failed to add block.
 */
template <typename TFixedBlock>
bool FixedBlockPool<TFixedBlock>::add(TFixedBlock* block) {
  assert(block);
  assert(!in_list(block));

  if (!block)
    return false;
  if (in_list(block))
    return false;

  // We only add to the head...
  if (m_head) {
    block->next(m_head);
    m_head->prev(block);
  }
  m_head = block;

  // ... except when we have an empty list.
  if (!m_tail) {
    m_tail = block;
  }

  m_num_blocks++;
  m_num_bytes += block->user_data_size;

  return true;
}

/**
 * @brief Removes a memory block (with header) from the internal list.
 *
 * @param block Pointer to the block to remove.
 * @return true when block removed.
 * @return false when failed to remove block.
 */
template <typename TFixedBlock>
bool FixedBlockPool<TFixedBlock>::remove(TFixedBlock* block) {
  assert(block);
  assert(in_list(block));
  assert(m_head);

  if (!block)
    return false;
  if (!in_list(block))
    return false;
  if (!m_head)
    return false;

  if (block->prev()) {
    auto prev = block->prev();
    assert(prev->next() == block);

    prev->next(block->next());
  }

  if (block->next()) {
    auto next = block->next();
    assert(next->prev() == block);

    next->prev(block->prev());
  }

  if (block == m_head) {
    m_head = block->next();
  }

  if (block == m_tail) {
    m_tail = block->prev();
  }

  block->prev(nullptr);
  block->next(nullptr);

  m_num_blocks--;
  m_num_bytes -= block->user_data_size;

  if (m_num_blocks == 0) {
    m_head = nullptr;
    m_tail = nullptr;
  }

  return true;
}

template <typename TFixedBlock>
bool FixedBlockPool<TFixedBlock>::in_list(TFixedBlock* block) const {
  return (m_head == block || m_tail == block || block->next() || block->prev());
}


} // namespace allok8or