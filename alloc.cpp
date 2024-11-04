/// @file
/// @author github.com/SirJonthe
/// @date 2024
/// @copyright Public domain.
/// @license CC0 1.0

#include <string.h>
#include "alloc.h"

cc0::allocator::Block *cc0::allocator::get_prev(Block *b)
{
	void *p = (uint8_t*)b - b->prev;
	if (p < m_mem) {
		return nullptr;
	}
	return b->prev > 0 ? (Block*)p : nullptr;
}

cc0::allocator::Block *cc0::allocator::get_next(Block *b)
{
	const uint32_t next = get_size(b);
	void *p = (uint8_t*)b + next;
	if (p >= m_mem + m_size) {
		return nullptr;
	}
	return next > 0 ? (Block*)p : nullptr;
}

void cc0::allocator::set_free(Block *b)
{
	b->size_flags &= 0x7fffffff;
}

void cc0::allocator::set_occupied(Block *b)
{
	b->size_flags |= 0x80000000;
}


bool cc0::allocator::is_free(Block *b) const
{
	return (b->size_flags & 0x80000000) == 0;
}

bool cc0::allocator::is_occupied(Block *b) const
{
	return b->size_flags & 0x80000000;
}

uint32_t cc0::allocator::get_size(Block *b) const
{
	return b->size_flags & 0x7fffffff;
}

void cc0::allocator::set_size(Block *b, uint32_t size)
{
	b->size_flags = (b->size_flags & 0x80000000) | (size & 0x7fffffff);
}

uint32_t cc0::allocator::align_num_bytes(uint32_t num_bytes) const
{
	return (num_bytes / m_align_bytes) * m_align_bytes + m_align_bytes * (num_bytes % m_align_bytes > 0);
}

cc0::allocator::allocator(void *mem, uint32_t size, uint32_t align_bytes) : m_mem((uint8_t*)mem), m_size(size), m_align_bytes(align_bytes > 0 ? align_bytes : 1), m_occupied(0)
{
	memset(m_mem, 0, m_size);

	Block *b = (Block*)m_mem;
	b->prev = 0;
	m_block_alignment = align_num_bytes(sizeof(Block));
	set_size(b, m_size - m_block_alignment);
	set_free(b);
	m_occupied += m_block_alignment;
}

void *cc0::allocator::alloc(uint32_t num_bytes)
{
	if (num_bytes == 0) {
		return nullptr;
	}

	num_bytes = align_num_bytes(num_bytes);
	Block *b = (Block*)m_mem;
	while (b != nullptr && (is_occupied(b) || get_size(b) < num_bytes)) {
		b = get_next(b);
	}
	if (b == nullptr) {
		return nullptr;
	}

	set_occupied(b);

	if (get_size(b) >= num_bytes + m_align_bytes + m_block_alignment) {
		uint32_t free_size = get_size(b) - num_bytes - m_block_alignment;
		set_size(b, num_bytes); // Shrink the size of the block.
		Block *n = get_next(b);
		set_free(n);
		set_size(n, free_size);
		n->prev = num_bytes;
		m_occupied += m_block_alignment;
	}
	m_occupied += get_size(b);
	return (uint8_t*)b + m_block_alignment;
}

void cc0::allocator::free(void *mem)
{
	mem = (uint8_t*)mem - m_block_alignment;
	if (mem >= m_mem + m_size || mem < m_mem) {
		return;
	}

	Block *b = (Block*)mem;
	set_free(b);
	m_occupied -= get_size(b);

	bool remove_block = false;

	Block *p = get_prev(b);
	if (p != nullptr && is_free(p)) {
		set_size(p, get_size(b) + get_size(p));
		b = p;
		remove_block = true;
	}

	Block *n = get_next(b);
	if (n != nullptr && is_free(n)) {
		set_size(b, get_size(b) + get_size(n));
		remove_block = true;
	}

	if (remove_block) {
		m_occupied -= m_block_alignment;
	}
}

uint32_t cc0::allocator::occupied_bytes( void ) const
{
	return m_occupied;
}

uint32_t cc0::allocator::total_bytes( void ) const
{
	return m_size;
}
