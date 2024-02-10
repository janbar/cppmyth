#include <iostream>

#include "include/testmain.h"
#include "include/hashvalue.c"
#include "include/sample_stream.c"

#include <private/compressor.h>

size_t _flush_compressed_data(Myth::Compressor * c, char * out)
{
  size_t l = 0;
  int w = 0;
  // check for available encoded data and flush out
  while ((w = c->ReadOutput(out, 1024)) > 0)
  {
    out += w;
    l += w;
  }
  return l;
}

size_t _flush_decompressed_data(Myth::Decompressor * d, char * out)
{
  size_t l = 0;
  int w = 0;
  // check for available encoded data and flush out
  while ((w = d->ReadOutput(out, 1024)) > 0)
  {
    out += w;
    l += w;
  }
  return l;
}

TEST_CASE("Compress Z data")
{
  Myth::Compressor * c =  new Myth::Compressor((const char*)stream_raw, stream_raw_len);
  char * zip = new char[stream_raw_len];
  size_t lz = _flush_compressed_data(c, zip);
  REQUIRE(c->HasStreamError() == false);
  REQUIRE(c->HasBufferError() == false);
  REQUIRE(c->IsCompleted() == true);
  REQUIRE((lz > 0 && lz < stream_raw_len));

  char * raw = new char[stream_raw_len];
  Myth::Decompressor * d =  new Myth::Decompressor((const char*)zip, lz);
  size_t lr = _flush_decompressed_data(d, raw);
  REQUIRE(d->HasStreamError() == false);
  REQUIRE(d->HasBufferError() == false);
  REQUIRE(d->IsCompleted() == true);
  REQUIRE(lr == stream_raw_len);
  REQUIRE((hashvalue(0xffffffff, raw, lr) == hashvalue(0xffffffff, (const char *)stream_raw, stream_raw_len)));

  delete d;
  delete c;
  delete [] zip;
  delete [] raw;
}

