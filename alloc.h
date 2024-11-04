/// @file
/// @author github.com/SirJonthe
/// @date 2024
/// @copyright Public domain.
/// @license CC0 1.0

#ifndef CC0_ALLOC_H
#define CC0_ALLOC_H

#include <cstdint>

namespace cc0
{

/// @brief A memory allocator which works on a block of memory in-place. All meta-data is also allocated on the memory.
/// @note The allocator does not own the memory it works on, so if it was allocated dynamically then the user is responsible for destroying it when it is no longer in use.
class allocator
{
private:
	/// @brief 
	struct Block
	{
		uint32_t size_flags; // The first bit indicates if the block is occupied or free (1 for occupied). The rest of the bits indicates the number of bytes that are occupied or free.
		uint32_t prev; // The number of bytes offset from the current memory address to get to the previous block. 0 means that there is no previous block.
	};

private:
	uint8_t  *m_mem;             // The memory block to allocate memory from.
	uint32_t  m_size;            // The number of bytes in the memory.
	uint32_t  m_align_bytes;     // The number of bytes to align each new allocation by.
	uint32_t  m_block_alignment; // The number of bytes a block allocates after memory alignment.
	uint32_t  m_occupied;        // The total number of bytes occupied in the memory.

private:
	/// @brief Returns the previous block.
	/// @param b The block.
	/// @return The previous block.
	Block *get_prev(Block *b);

	/// @brief Returns the next block.
	/// @param b The block.
	/// @return The next block.
	Block *get_next(Block *b);

	/// @brief Marks the block as free.
	/// @param b The block.
	void set_free(Block *b);

	/// @brief Marks the block as occupied.
	/// @param b The block.
	void set_occupied(Block *b);
	
	/// @brief Checks if a block is free.
	/// @param b The block.
	/// @return True if free.
	bool is_free(Block *b) const;

	/// @brief Checks if a block is occupied.
	/// @param b The block.
	/// @return True if occupied.
	bool is_occupied(Block *b) const;

	/// @brief Returns the size of the block.
	/// @param b The block.
	/// @return The size in bytes.
	uint32_t get_size(Block *b) const;

	/// @brief Sets the size of the block.
	/// @param b The block.
	/// @param size The size in bytes.
	void set_size(Block *b, uint32_t size);

	/// @brief Aligns the number of bytes requested to the alignment requirements.
	/// @param num_bytes The number of bytes requested.
	/// @return The number of bytes provided after alignment.
	uint32_t align_num_bytes(uint32_t num_bytes) const;

public:
	/// @brief Constructs a new allocator.
	/// @param mem The memory in which to allocate memory.
	/// @param size The size of the memory in bytes.
	/// @param align_bytes The size in bytes of the alignment requirement.
	allocator(void *mem, uint32_t size, uint32_t align_bytes = sizeof(uint64_t));

	/// @brief Allocates a specified number of bytes. May allocate more depending on alignment requirements and headers.
	/// @param num_bytes The number of continuous bytes to allocate.
	/// @return A pointer to the allocated memory. Null if the requested byte count could not fit continuously inside the memory buffer.
	void *alloc(uint32_t num_bytes);

	/// @brief Deletes the memory referred to by the pointer.
	/// @param mem The memory.
	/// @warning The memory pointer must be the pointer returned by a corresponding call to alloc. If not, the memory inside the buffer will be corrupted.
	void free(void *mem);

	/// @brief Returns the number of bytes occupied in the main memory buffer.
	/// @return The number of bytes occupied in the main memory buffer.
	uint32_t occupied_bytes( void ) const;

	/// @brief Returns the total number of bytes (occupied and free) in the main memory buffer.
	/// @return The total number of bytes in the main memory.
	uint32_t total_bytes( void ) const;
};

}

#endif
