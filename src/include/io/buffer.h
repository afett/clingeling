#pragma once

#include <stddef.h>

/*

     <--------------- capacity -------------->
    +-----------------------------------------+
    | | | | | | | | | |D|D|D|D|D| | | | | | | |
    +^-----------------^---------^------------+
     |                 |         |
     |                 |         |
   data             rstart     wstart

*/

namespace IO {

class Buffer {
public:
	/* create a new iobuf */
	Buffer() = default;

	/* create a new iobuf with the given wsize*/
	explicit Buffer(size_t);

	/* free iobuf and it's data */
	~Buffer();

	/* call reserve before writing at most iobuf_wsize() bytes
	 * starting at iobuf_wstart()
	 *
	 * this will invalidate any results from previous calls to
	 * rsize, rstart, wsize, wstart
	 */
	void reserve(size_t);

	/* call fill after writing at most iobuf_wsize() bytes
	 * starting at iobuf_wstart()
	 *
	 * this will invalidate any results from previous calls to
	 * rsize and wsize
	 */
	void fill(size_t);

	/* maximum size that may be read from rstart, and passed
	 * to drain
	 */
	size_t rsize() const;

	/* start of data to read */
	void *rstart() const;

	/* call drain after you read at most iobuf_rsize() bytes
	 * starting at iobuf_rstart()
	 *
	 * this will invalidate any results from previous calls to
	 * rsize and wsize
	 */
	void drain(size_t);

	/* maximum size that may be written to wstart */
	size_t wsize() const;

	/* start of data to write */
	void *wstart() const;

	bool empty() const;
	bool full() const;

private:
	void grow(size_t);
	void reclaim();

	void *data_ = nullptr;
	size_t capacity_ = 0;
	size_t rstart_ = 0;
	size_t wstart_ = 0;
};

}
