# Introduction #

Below is the byte-level description of the MGA binary file format.  This will change depending on the work being done regarding changes to the MGA "special sauce" and the binary format itself, as described in [MGA\_v2\_to\_v3\_Changes](MGA_v2_to_v3_Changes.md).  Bytes  are all ordered little endian.

# MGA Binary File Format - Version 3 #

## Preamble ##

0-15: Uuid (16 bytes) - The Uuid of the corresponding MetaProject.

16-23: Location of Options (8 bytes as uint64\_t)

24-27: Size of Options (4 bytes as uint32\_t)


## Options ##

0: Is Encryption enabled flag (1 byte as uint8\_t)

1: Is Compression enabled flag (1 byte as uint8\_t)

2-17: Uuid of the root object for this project (16 bytes as Uuid)

18-25: Start of object index (8 bytes as uint64\_t)

26-33: Number of objects in index (8 bytes as uint64\_t)


## Index Entries (Each) ##

  * Uuid (16 bytes as Uuid)

  * File Position (8 bytes as uin64\_t)

  * Object Size (4 bytes as uint32\_t)


### Layout of Individual Objects ###

0-1: MetaID (2 bytes - MetaID\_t)<br>

<h3>Start of Attributes (within an Object)</h3>

0-1: ValueType (1 byte - uint8_t)<br>
2-3: AttrID (2 bytes - AttrID_t)<br>

-- Read Attribute Value<br>
<ul><li>Long: 4 bytes - int32_t<br>
</li><li>Real: 8 bytes - double<br>
</li><li>String: 4 bytes for length (uint32_t) + length number of bytes<br>
</li><li>LongPointer: 16 bytes (Uuid)<br>
</li><li>Collection: 4 bytes size (uint32_t) + 16 bytes (Uuid) per item in collection<br>
</li><li>Pointer: 16 bytes (Uuid)</li></ul>

<h3>End of Attributes when ValueType equals VALUETYPE_NONE</h3>

<h2>End of Objects</h2>

<h1>Loading a v3 MGA File</h1>

Files are loaded via the BinFile::Load() method.  One fstream read is performed to get the first 28 bytes of the file.  This will get the preamble (metaProject Uuid, options location and options size).  The Uuid is compared to that of the CoreMetaProject passed in.  The second read reads the options using the BinFile::ReadOptions() method.  This covers if encryption and compression are used, the Uuid of the root object, and the position of the index and the number of objects in the index. The number of objects in the index is passed to BinFile::ReadIndex() which calculates the size of the index and performs a single read to read in the entire index.  All reading after this is done on a per-object basis.<br>
<br>
<h1>MGA File Limitations</h1>

There should be as few limitations as possible with the new v3 file format.  Here are a couple that I know of:<br>
<br>
<ul><li>Max File Size: 64-bit addressing (16 million terabytes - really big)<br>
</li><li>Max Number of Objects: 2^64 objects (1.8e19 objects - really big)<br>
</li><li>Max Size of One Object: 2^32 bytes (4GB per object - not too likely to be a problem)